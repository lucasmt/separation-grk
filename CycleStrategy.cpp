#include "CycleStrategy.h"

#include <cassert>
#include <utility>

#include "Util.h"

namespace SGrk {

PathStrategy::PathStrategy(
    CUDD::BDD realizable_region,
    std::vector<MemorylessStrategy> strategy_parts,
    std::vector<CUDD::BDD> stopping_conditions)
	: realizable_region_(std::move(realizable_region))
	, strategy_parts_(std::move(strategy_parts))
	, stopping_conditions_(std::move(stopping_conditions)) {}

CUDD::BDD PathStrategy::RealizableRegion() const {
	return realizable_region_;
}

const std::vector<MemorylessStrategy>& PathStrategy::StrategyParts() const {
	return strategy_parts_;
}

const std::vector<CUDD::BDD>& PathStrategy::StoppingConditions() const {
	return stopping_conditions_;
}

CycleStrategy::CycleStrategy(std::shared_ptr<CUDD::Cudd> mgr,
                             std::shared_ptr<VarMgr> vars,
                             MemorylessStrategy idle_strategy)
	  : mgr_(std::move(mgr))
	  , vars_(std::move(vars)) {
	strategy_parts_.push_back(idle_strategy);
	stopping_conditions_.push_back(mgr_->bddOne());
	continuations_.push_back(mgr_->constant(0));
	reset_.push_back(mgr_->bddOne());
	idle_region_ = mgr_->bddOne();
}

void CycleStrategy::Merge(const PathStrategy& path_strategy) {
	CUDD::BDD realizable_region =
		vars_->UnprimedToPrimed(path_strategy.RealizableRegion());
	const std::vector<MemorylessStrategy>& more_strategy_parts =
		path_strategy.StrategyParts();
	const std::vector<CUDD::BDD>& more_stopping_conditions =
		path_strategy.StoppingConditions();

	assert(!more_strategy_parts.empty());
	assert(more_strategy_parts.size() == more_stopping_conditions.size());
	
	strategy_parts_.insert(strategy_parts_.end(),
	                       more_strategy_parts.begin(),
	                       more_strategy_parts.end());
	
	stopping_conditions_.insert(stopping_conditions_.end(),
	                            more_stopping_conditions.begin(),
	                            more_stopping_conditions.end());

	std::size_t n = continuations_.size();
	CUDD::BDD new_busy_states = idle_region_ & realizable_region;
	CUDD::ADD last_continuation =
		(new_busy_states.Add() * mgr_->constant(n)) +
		((!new_busy_states).Add() * continuations_[n - 1]);

	for (int i = 1; i < n; ++i) {
		CUDD::BDD needs_update = reset_[i] & realizable_region;
		
		continuations_[i] =
			((!needs_update).Add() * continuations_[i]) +
			(needs_update.Add() * mgr_->constant(n));

		reset_[i] &= !realizable_region;
	}

	int last_index = n + more_strategy_parts.size() - 1;
	
	for (int i = n; i < last_index; ++i) {
		continuations_.push_back(mgr_->constant(i + 1));
		reset_.push_back(mgr_->bddZero());
	}

	continuations_.push_back(last_continuation);
	reset_.push_back(mgr_->bddOne());
	idle_region_ &= !realizable_region;
}

int CycleStrategy::InitialIndex(std::vector<int>& transition_vector)
	  const {
	return Util::EvalAdd(continuations_.back(), transition_vector);
}

int CycleStrategy::Step(std::vector<int>& transition_vector, int i) const {
	strategy_parts_[i].Update(transition_vector);

	if (Util::EvalBdd(stopping_conditions_[i], transition_vector)) {
		return Util::EvalAdd(continuations_[i], transition_vector);
	} else {
		return i;
	}
}

}
