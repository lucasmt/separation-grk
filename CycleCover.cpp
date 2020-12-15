#include "CycleCover.h"

#include <numeric>

namespace SGrk {

CycleCover::CycleCover(std::shared_ptr<CUDD::Cudd> mgr,
                       std::shared_ptr<VarMgr> vars,
                       const SeparationGrkSpec& spec,
                       const SpaceConnectivity& connectivity)
	  : mgr_(std::move(mgr))
	  , vars_(std::move(vars)) {
	ComputeCounts(spec);
	ComputeCycles(spec, connectivity);
	ComputeCoverage(spec, connectivity);
	ComputeStrategy(spec, connectivity);
	ComputeIndices(spec);
	ComputeContinuation(spec);
}

CUDD::BDD CycleCover::CoversAllCycles() const {
	return covers_all_cycles_;
}

const std::vector<CUDD::BDD>& CycleCover::AssumptionComponents() const {
	return can_satisfy_assumptions_;
}
	
const std::vector<CUDD::BDD>& CycleCover::GuaranteeComponents() const {
	return can_satisfy_guarantees_;
}
	
const std::vector<CUDD::BDD>& CycleCover::CycleCoverage() const {
	return covers_cycles_;
}
	
const std::vector<PermissiveStrategy>& CycleCover::CycleStrategy() const {
	return cycle_strategy_;
}

const std::vector<CUDD::ADD>& CycleCover::CycleContinuation() const {
	return cycle_continuation_;
}

CUDD::BDD CycleCover::HasCycle(const CUDD::BDD& connected,
                               const CUDD::BDD& prop) const {
	// Exists s' . Connected(s, s') & Prop(s')
	return vars_->Exists(vars_->PrimedVars(),
	                     connected & vars_->UnprimedToPrimed(prop));
}

void CycleCover::ComputeCounts(const SeparationGrkSpec& spec) {
	const std::vector<SeparationGrkImplication>& justice_implications =
		spec.JusticeImplications();

	implication_count_ = justice_implications.size();

	for (const SeparationGrkImplication& implication : justice_implications) {
		assumption_count_.push_back(implication.Assumptions().size());
		guarantee_count_.push_back(implication.Guarantees().size());
	}
}

void CycleCover::ComputeCycles(const SeparationGrkSpec& spec,
                               const SpaceConnectivity& connectivity) {
	CUDD::BDD environment_bipath_relation =
		connectivity.EnvironmentBipathRelation();
	CUDD::BDD system_bipath_relation =
		connectivity.SystemBipathRelation();
	CUDD::BDD environment_valid_states = connectivity.EnvironmentValidStates();
	CUDD::BDD system_valid_states = connectivity.SystemValidStates();
	
	const std::vector<SeparationGrkImplication>& justice_implications =
		  spec.JusticeImplications();

	can_satisfy_assumption_.resize(implication_count_);
	can_satisfy_guarantee_.resize(implication_count_);
	
	for (int i = 0; i < implication_count_; ++i) {
		const std::vector<CUDD::BDD>& assumptions =
			justice_implications[i].Assumptions();
		const std::vector<CUDD::BDD>& guarantees =
			justice_implications[i].Guarantees();

		can_satisfy_assumption_[i].resize(assumption_count_[i]);
		can_satisfy_guarantee_[i].resize(guarantee_count_[i]);

		for (int j = 0; j < assumption_count_[i]; ++j) {
			can_satisfy_assumption_[i][j] =
				environment_valid_states &
				HasCycle(environment_bipath_relation, assumptions[j]);
		}

		for (int j = 0; j < guarantee_count_[i]; ++j) {
			can_satisfy_guarantee_[i][j] =
				system_valid_states &
				HasCycle(system_bipath_relation, guarantees[j]);
		}
	}
}

void CycleCover::ComputeCoverage(const SeparationGrkSpec& spec,
                                 const SpaceConnectivity& connectivity) {
	CUDD::BDD valid_states = connectivity.ValidStates();
	
	can_satisfy_assumptions_.resize(implication_count_);
	can_satisfy_guarantees_.resize(implication_count_);
	covers_cycles_.resize(implication_count_);

	CUDD::BDD bdd_one = mgr_->bddOne();
	auto bdd_and = [] (const CUDD::BDD& b1, const CUDD::BDD& b2) {
		return b1 & b2;
	};
	
	for (int i = 0; i < implication_count_; ++i) {
		can_satisfy_assumptions_[i] =
			std::accumulate(can_satisfy_assumption_[i].begin(),
			                can_satisfy_assumption_[i].end(),
			                bdd_one, bdd_and);

		can_satisfy_guarantees_[i] =
			std::accumulate(can_satisfy_guarantee_[i].begin(),
			                can_satisfy_guarantee_[i].end(),
			                bdd_one, bdd_and);

		covers_cycles_[i] = valid_states & (!can_satisfy_assumptions_[i] |
		                                    can_satisfy_guarantees_[i]);
	}

	covers_all_cycles_ =
		std::accumulate(covers_cycles_.begin(), covers_cycles_.end(),
		                bdd_one, bdd_and);
}

void CycleCover::ComputeIndices(const SeparationGrkSpec& spec) {
	int index = 0;

	guarantee_index_.resize(implication_count_);
	
	for (int i = 0; i < implication_count_; ++i) {
		guarantee_index_[i].resize(guarantee_count_[i]);
		
		for (int j = 0; j < guarantee_count_[i]; ++j) {
			guarantee_index_[i][j] = index;
			++index;
		}
	}
}

void CycleCover::ComputeStrategy(const SeparationGrkSpec& spec,
                                 const SpaceConnectivity& connectivity) {
	CUDD::BDD transition_relation = spec.SafetyGuarantees();
	CUDD::BDD bipath_relation = connectivity.SystemBipathRelation();
	
	for (const auto& implication : spec.JusticeImplications()) {
		for (const CUDD::BDD& guarantee : implication.Guarantees()) {
			// States_0(s) = Guarantee(s)
			CUDD::BDD states = guarantee;
			// Moves_0(s, s') = States_0(s)
			CUDD::BDD moves = states;

			while (true) {
				// Moves_i(s, s') = Moves_{i-1}(s, s') | (!States_{i-1}(s) &
				//                                        Bipath(s, s') &
				//                                        States_{i-1}(s'))
				CUDD::BDD new_moves = moves | (!states & bipath_relation &
				                               vars_->UnprimedToPrimed(states));

				// States_i(s) = Exists y' . Moves_i(s, s')
				CUDD::BDD new_states = vars_->Exists(vars_->PrimedVars(), new_moves);

				if (new_states == states) {
					CUDD::BDD winning_states = new_states;
					CUDD::BDD winning_moves = new_moves;
					
					cycle_strategy_.push_back(PermissiveStrategy(winning_states,
					                                             winning_moves));
					break;
				}

				states = new_states;
				moves = new_moves;
			}
		}
	}
}

void CycleCover::ComputeContinuation(const SeparationGrkSpec& spec) {
	const std::vector<SeparationGrkImplication>& justice_implications =
		spec.JusticeImplications();

	for (int i = 0; i < implication_count_; ++i) {
		const std::vector<CUDD::BDD>& guarantees =
			justice_implications[i].Guarantees();

		std::size_t last = guarantees.size() - 1;
		
		for (int j = 0; j < last; ++j) {
			CUDD::ADD continuation =
				(!guarantees[j]).Add() * mgr_->constant(guarantee_index_[i][j]) +
				guarantees[j].Add() * mgr_->constant(guarantee_index_[i][j+1]);
			
			cycle_continuation_.push_back(continuation);
		}

		// Continuation for (i, last)
		CUDD::ADD advancing_continuation = mgr_->constant(0);
		CUDD::BDD no_continuation_yet = mgr_->bddOne();
		
		for (int k = i + 1; k < implication_count_; ++k) {
			CUDD::BDD continue_to_k = no_continuation_yet & covers_cycles_[k];

			advancing_continuation +=
				continue_to_k.Add() * mgr_->constant(guarantee_index_[k][0]);
			
			no_continuation_yet &= !covers_cycles_[k];
		}

		for (int k = 0; k <= i; ++k) {
			CUDD::BDD continue_to_k = no_continuation_yet & covers_cycles_[k];

			advancing_continuation +=
				continue_to_k.Add() * mgr_->constant(guarantee_index_[k][0]);
			
			no_continuation_yet &= !covers_cycles_[k];
		}

		CUDD::ADD last_continuation =
			(!guarantees[last]).Add() * mgr_->constant(guarantee_index_[i][last]) +
			guarantees[last].Add() * advancing_continuation;

		cycle_continuation_.push_back(last_continuation);
	}
}
}
