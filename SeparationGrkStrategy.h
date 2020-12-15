#ifndef SEPARATION_GRK_STRATEGY_H
#define SEPARATION_GRK_STRATEGY_H

#include <memory>

#include <cuddObj.hh>

#include "VarMgr.h"

namespace SGrk {

class SeparationGrkStrategy {
	std::shared_ptr<CUDD::Cudd> mgr_;
	std::shared_ptr<VarMgr> vars_;
	CUDD::BDD winning_states_;
	CUDD::BDD reaching_moves_;
	CUDD::BDD cycle_covering_states_;

 public:
	
	SeparationGrkStrategy(
	  std::shared_ptr<CUDD::Cudd> mgr,
	  std::shared_ptr<VarMgr> vars,
	  CUDD::BDD winning_states,
	  CUDD::BDD reaching_moves,
	  CUDD::BDD cycle_covering_states);

	CUDD::BDD WinningStates() const;
	
	CUDD::BDD ReachingMoves() const;
	
	CUDD::BDD CycleCoveringStates() const;
};
 
}

#endif // SEPARATION_GRK_STRATEGY_H
