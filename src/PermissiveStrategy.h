#ifndef PERMISSIVE_STRATEGY_H
#define PERMISSIVE_STRATEGY_H

#include <cuddObj.hh>

namespace SGrk {

class PermissiveStrategy {
	CUDD::BDD winning_states_;
	CUDD::BDD winning_moves_;

 public:
	
	PermissiveStrategy(CUDD::BDD winning_states_, CUDD::BDD winning_moves_);
	CUDD::BDD WinningStates() const;
	CUDD::BDD WinningMoves() const;
};
	
}

#endif // PERMISSIVE_STRATEGY_H
