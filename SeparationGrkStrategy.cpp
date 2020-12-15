#include "SeparationGrkStrategy.h"

namespace SGrk {

SeparationGrkStrategy::SeparationGrkStrategy(
    std::shared_ptr<CUDD::Cudd> mgr,
    std::shared_ptr<VarMgr> vars,
    CUDD::BDD winning_states,
    CUDD::BDD reaching_moves,
    CUDD::BDD cycle_covering_states)
	  : mgr_(std::move(mgr)), vars_(std::move(vars))
	  , winning_states_(std::move(winning_states))
	  , reaching_moves_(std::move(reaching_moves))
	  , cycle_covering_states_(std::move(cycle_covering_states)) {}

CUDD::BDD SeparationGrkStrategy::WinningStates() const {
	return winning_states_;
}
	
CUDD::BDD SeparationGrkStrategy::CycleCoveringStates() const {
	return cycle_covering_states_;
}
	
CUDD::BDD SeparationGrkStrategy::ReachingMoves() const {
	return reaching_moves_;
}

}
