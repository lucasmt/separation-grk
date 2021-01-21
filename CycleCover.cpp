#include "CycleCover.h"

#include <numeric>

namespace SGrk {

CycleCover::CycleCover(std::shared_ptr<CUDD::Cudd> mgr,
                       std::shared_ptr<VarMgr> vars,
                       const SeparationGrkSpec& spec,
                       const SpaceConnectivity& connectivity)
	  : mgr_(std::move(mgr))
	  , vars_(std::move(vars))
	  , covered_region_(ComputeCoveredRegion(spec, connectivity))
	  , cycle_strategy_(ComputeCycleStrategy(spec, connectivity)) {}

CUDD::BDD CycleCover::CoveredRegion() const {
	return covered_region_;
}

const CycleStrategy& CycleCover::Strategy() const {
	return cycle_strategy_;
}

MemorylessStrategy CycleCover::ComputeReachabilityStrategy(
    const CUDD::BDD& transition_relation,
    const CUDD::BDD& bipath_relation,
    const CUDD::BDD& goal) const {
	// States_0(s) = Goal(s)
	CUDD::BDD states = goal;

	// This strategy might be called when the game is already in a goal state,
	// so need to be sure the move from the goal state can come back to the goal
	//
	// Moves_0(s, s') = States_0(s) & Bipath(s, s')
	CUDD::BDD moves = states & bipath_relation;

	while (true) {
		// Moves_i(s, s') = Moves_{i-1}(s, s') | (!States_{i-1}(s) & T(s, s') &
		//                                        Bipath(s, s') &
		//                                        States_{i-1}(s'))
		CUDD::BDD new_moves = moves |	(!states & transition_relation &
			                             bipath_relation &
			                             vars_->UnprimedToPrimed(states));

		// States_i(s) = Exists y' . Moves_i(s, s')
		CUDD::BDD new_states = vars_->Exists(vars_->PrimedVars(), new_moves);

		if (new_states == states) {
			CUDD::BDD winning_moves = new_moves;

			return MemorylessStrategy::Determinize(mgr_, vars_, winning_moves);
		}

		states = new_states;
		moves = new_moves;
	}
}

PathStrategy CycleCover::ComputePathStrategy(
    const CUDD::BDD& transition_relation,
    const CUDD::BDD& bipath_relation,
    const std::vector<CUDD::BDD>& goals) const {
	std::vector<MemorylessStrategy> strategy_parts;
	std::vector<CUDD::BDD> stopping_conditions;
	CUDD::BDD realizable_region = mgr_->bddOne();
	
	for (const CUDD::BDD& goal : goals) {
		MemorylessStrategy strategy_part =
			ComputeReachabilityStrategy(transition_relation, bipath_relation, goal);

		strategy_parts.push_back(strategy_part);
		stopping_conditions.push_back(vars_->UnprimedToPrimed(goal));
		realizable_region &= strategy_part.RealizableRegion();
	}

	return PathStrategy(realizable_region, std::move(strategy_parts), goals);
}

CycleStrategy CycleCover::ComputeCycleStrategy(
    const SeparationGrkSpec& spec,
    const SpaceConnectivity& connectivity) const {
	CUDD::BDD transition_relation = spec.SafetyGuarantees();
	CUDD::BDD bipath_relation = connectivity.SystemBipathRelation();

	MemorylessStrategy idle_strategy =
		MemorylessStrategy::Determinize(mgr_, vars_,
		                                transition_relation & bipath_relation);

	CycleStrategy cycle_strategy(mgr_, vars_, idle_strategy);
	
	for (const auto& implication : spec.JusticeImplications()) {
		PathStrategy path_strategy = ComputePathStrategy(transition_relation,
		                                                 bipath_relation,
		                                                 implication.Guarantees());

		cycle_strategy.Merge(path_strategy);
	}

	return cycle_strategy;
}

CUDD::BDD CycleCover::HasCycle(const CUDD::BDD& connected,
                               const CUDD::BDD& prop) const {
	// Exists s' . Connected(s, s') & Prop(s')
	return vars_->Exists(vars_->PrimedVars(),
	                     connected & vars_->UnprimedToPrimed(prop));
}

CUDD::BDD CycleCover::ComputeCoveredRegion(
    const SeparationGrkSpec& spec,
    const SpaceConnectivity& connectivity) const {
	CUDD::BDD valid_states =
		connectivity.ValidStates();
	CUDD::BDD bipath_relation =
		connectivity.BipathRelation();
	CUDD::BDD environment_bipath_relation =
		connectivity.EnvironmentBipathRelation();
	CUDD::BDD system_bipath_relation =
		connectivity.SystemBipathRelation();

	CUDD::BDD cycle_states = valid_states & vars_->Exists(vars_->PrimedVars(),
	                                                      bipath_relation &
	                                                      vars_->Reflexive());

	CUDD::BDD covered_region = cycle_states;

	for (const auto& implication : spec.JusticeImplications()) {
		CUDD::BDD can_satisfy_assumptions = mgr_->bddOne();
		
		for (const auto& assumption : implication.Assumptions()) {
			can_satisfy_assumptions &=
				HasCycle(environment_bipath_relation, assumption);
		}

		CUDD::BDD can_satisfy_guarantees = mgr_->bddOne();

		for (const auto& guarantee : implication.Guarantees()) {
			can_satisfy_guarantees =
				HasCycle(system_bipath_relation, guarantee);
		}

		covered_region &= !can_satisfy_assumptions | can_satisfy_guarantees;
	}

	return covered_region;
}

}
