#include "SeparationGrkStrategy.h"

namespace SGrk {

SeparationGrkStrategy::SeparationGrkStrategy(
    std::shared_ptr<CUDD::Cudd> mgr,
    std::shared_ptr<VarMgr> vars,
    MemorylessStrategy initial_strategy,
    CycleCover cycle_cover,
    PermissiveStrategy weak_fg_strategy)
	  : mgr_(std::move(mgr)), vars_(std::move(vars))
	  , initial_strategy_(std::move(initial_strategy))
	  , cycle_cover_(std::move(cycle_cover))
	  , weak_fg_strategy_(std::move(weak_fg_strategy))
	  , reaching_moves_(weak_fg_strategy_.WinningMoves() &
	                    !cycle_cover_.CoveredRegion())
	  , reachability_strategy_(
	        MemorylessStrategy::Determinize(mgr_, vars_,
	                                        weak_fg_strategy_.WinningMoves()))
{}

CUDD::BDD SeparationGrkStrategy::WinningStates() const {
	return weak_fg_strategy_.WinningStates();
}

CUDD::BDD SeparationGrkStrategy::CycleCoveringStates() const {
	return cycle_cover_.CoveredRegion();
}
	
CUDD::BDD SeparationGrkStrategy::ReachingMoves() const {
	return reaching_moves_;
}

const MemorylessStrategy& SeparationGrkStrategy::GetInitialStrategy() const {
	return initial_strategy_;
}

const MemorylessStrategy& SeparationGrkStrategy::GetReachabilityStrategy()
	  const {
	return reachability_strategy_;
}

const CycleStrategy& SeparationGrkStrategy::GetCycleStrategy() const {
	return cycle_cover_.Strategy();
}

}
