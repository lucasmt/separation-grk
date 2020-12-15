#include "PermissiveStrategy.h"

#include <memory>

namespace SGrk {

PermissiveStrategy::PermissiveStrategy(CUDD::BDD winning_states,
                                       CUDD::BDD winning_moves)
	: winning_states_(std::move(winning_states))
	, winning_moves_(std::move(winning_moves)) {}

CUDD::BDD PermissiveStrategy::WinningStates() const {
	return winning_states_;
}

CUDD::BDD PermissiveStrategy::WinningMoves() const {
	return winning_moves_;
}
}
