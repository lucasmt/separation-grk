#ifndef WEAK_FG_GAME_SOLVER_H
#define WEAK_FG_GAME_SOLVER_H

#include <memory>
#include <vector>

#include <cuddObj.hh>

#include "PermissiveStrategy.h"
#include "VarMgr.h"
#include "WeakFGGame.h"

namespace SGrk {

class WeakFGGameSolver {
	std::shared_ptr<CUDD::Cudd> mgr_;
	std::shared_ptr<VarMgr> vars_;
	WeakFGGame game_;
	std::vector<CUDD::BDD> layers_;
	std::vector<CUDD::BDD> layers_below_;

	CUDD::BDD PeelLayer(const CUDD::BDD& states) const;
	PermissiveStrategy SolveReachability(const CUDD::BDD& goal_states,
	                                     const CUDD::BDD& state_space) const;
	PermissiveStrategy SolveSafety(const CUDD::BDD& safe_states,
	                               const CUDD::BDD& state_space) const;
	CUDD::BDD TransitionsInto(const CUDD::BDD& states) const;
	bool ContainsInitialStates(const CUDD::BDD& superset) const;
	
 public:

	WeakFGGameSolver(std::shared_ptr<CUDD::Cudd> mgr,
	                 std::shared_ptr<VarMgr> vars,
	                 WeakFGGame game);

	PermissiveStrategy Run() const;
};
	
}

#endif // WEAK_FG_GAME_SOLVER_H
