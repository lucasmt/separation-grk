#ifndef SEPARATION_GRK_STRATEGY_H
#define SEPARATION_GRK_STRATEGY_H

#include <memory>

#include <cuddObj.hh>

#include "CycleCover.h"
#include "CycleStrategy.h"
#include "MemorylessStrategy.h"
#include "PermissiveStrategy.h"
#include "VarMgr.h"

namespace SGrk {

class SeparationGrkStrategy {
	std::shared_ptr<CUDD::Cudd> mgr_;
	std::shared_ptr<VarMgr> vars_;
	MemorylessStrategy initial_strategy_;
	PermissiveStrategy weak_fg_strategy_;
	CycleCover cycle_cover_;
	CUDD::BDD reaching_moves_;
	MemorylessStrategy reachability_strategy_;

 public:
	
	SeparationGrkStrategy(
	  std::shared_ptr<CUDD::Cudd> mgr,
	  std::shared_ptr<VarMgr> vars,
	  MemorylessStrategy initial_strategy,
	  CycleCover cycle_cover,
	  PermissiveStrategy weak_fg_strategy);

	CUDD::BDD WinningStates() const;
	
	CUDD::BDD CycleCoveringStates() const;

	CUDD::BDD ReachingMoves() const;

	const MemorylessStrategy& GetInitialStrategy() const;

	const MemorylessStrategy& GetReachabilityStrategy() const;

	const CycleStrategy& GetCycleStrategy() const;
};
 
}

#endif // SEPARATION_GRK_STRATEGY_H
