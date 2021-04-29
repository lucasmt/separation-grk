#include "WeakFGGameSolver.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "cuddObj.hh"

namespace SGrk {

WeakFGGameSolver::WeakFGGameSolver(std::shared_ptr<CUDD::Cudd> mgr,
                                   std::shared_ptr<VarMgr> vars,
                                   WeakFGGame game)
	: mgr_(std::move(mgr))
	, vars_(std::move(vars))
	, game_(std::move(game)) {
	CUDD::BDD remaining_states = game_.AllStates();

	while (!(remaining_states.IsZero())) {
		layers_below_.push_back(remaining_states);

		CUDD::BDD top_layer = PeelLayer(remaining_states);
		layers_.push_back(top_layer);

		remaining_states &= !top_layer;
	}

	std::reverse(layers_.begin(), layers_.end());
	std::reverse(layers_below_.begin(), layers_below_.end());
}

CUDD::BDD WeakFGGameSolver::PeelLayer(const CUDD::BDD& states) const {
	// TODO(Lucas): What happens if a state is a slide?
	
	// Path(s, s') = States(s) & States(s') & P(s, s')
	CUDD::BDD path = (states & vars_->UnprimedToPrimed(states) &
	                  game_.PathRelation());

	// TopLayer(s) = States(s) & ForAll s' . (Path(s', s) -> Path(s, s'))
	CUDD::BDD top_layer = (states &
	                       vars_->Forall(vars_->PrimedVars(),
	                                     (!vars_->SwapPrimedAndUnprimed(path) |
	                                      path)));

	return top_layer;
}

PermissiveStrategy WeakFGGameSolver::Run() const {
	CUDD::BDD good_states = mgr_->bddZero();
	CUDD::BDD bad_states = mgr_->bddZero();
	CUDD::BDD winning_moves = mgr_->bddZero();
	CUDD::BDD accepting_states = game_.AcceptingStates();
	
	for (int i = 0; i < layers_.size(); ++i) {
		PermissiveStrategy reachability_strategy =
			SolveReachability(good_states, layers_below_[i]);
		PermissiveStrategy safety_strategy =
			SolveSafety(!bad_states, layers_below_[i]);

		winning_moves |= layers_[i] &
			((!accepting_states & reachability_strategy.WinningMoves()) |
			 (accepting_states & safety_strategy.WinningMoves()));

		CUDD::BDD reach_good_states = reachability_strategy.WinningStates();
		CUDD::BDD avoid_bad_states = safety_strategy.WinningStates();

		good_states |= layers_[i] & ((!accepting_states & reach_good_states) |
		                             (accepting_states & avoid_bad_states));
		bad_states |= layers_[i] & !good_states;
	}

	return PermissiveStrategy(good_states, winning_moves);
}

PermissiveStrategy
WeakFGGameSolver::SolveReachability(const CUDD::BDD& goal_states,
                                    const CUDD::BDD& state_space)
	const {
	CUDD::BDD winning_states = state_space & goal_states;
	CUDD::BDD winning_moves = winning_states;

  while (true) {
	  CUDD::BDD new_winning_moves = (winning_moves |
	                                 (state_space &
	                                  !winning_states &
	                                  game_.AntagonistTransitionRelation() &
	                                  TransitionsInto(winning_states)));
	  CUDD::BDD new_winning_states =
		  state_space &
		  vars_->Forall(vars_->PrimedInputs(),
		                (!game_.AntagonistTransitionRelation() |
		                 vars_->Exists(vars_->PrimedOutputs(), new_winning_moves)));

    if (new_winning_states == winning_states) {
	    return PermissiveStrategy(new_winning_states, new_winning_moves);
    }

    winning_states = new_winning_states;
    winning_moves = new_winning_moves;
  }
}

PermissiveStrategy WeakFGGameSolver::SolveSafety(const CUDD::BDD& safe_states,
                                                 const CUDD::BDD& state_space)
	const {
	// WinS_0(s) = Valid(s) & Safe(s)
	CUDD::BDD winning_states = state_space & safe_states;
	// WinM_0(s, s') = WinS_0(s)
	CUDD::BDD winning_moves = winning_states;

	while (true) {
		// WinM_{i+1}(s, s') = Valid(s) & WinS_i(s) & T^P(s, s') & WinS_i(s')
		CUDD::BDD new_winning_moves = (state_space & winning_states &
		                               game_.AntagonistTransitionRelation() &
		                               TransitionsInto(winning_states));

		// WinS_{i+1}(x, y) = Valid(x, y) & Forall x' . T^A(x, y, x') ->
		//                                      Exists y' . WinM_{i+1}(x, y, x', y')
		CUDD::BDD new_winning_states =
			state_space &
			vars_->Forall(vars_->PrimedInputs(),
			              (!game_.AntagonistTransitionRelation() |
			               vars_->Exists(vars_->PrimedOutputs(), new_winning_moves)));
		
		if (new_winning_states == winning_states) {
			return PermissiveStrategy(new_winning_states, new_winning_moves);
		}

		winning_states = new_winning_states;
		winning_moves = new_winning_moves;
	}
}	

CUDD::BDD WeakFGGameSolver::TransitionsInto(const CUDD::BDD& states) const {
	// T^P(x, y, x', y') & S(x', y')
	return (game_.ProtagonistTransitionRelation() &
	        vars_->UnprimedToPrimed(states));
}

bool WeakFGGameSolver::ContainsInitialStates(const CUDD::BDD& superset) const {
	return (game_.InitialStates() & !superset).IsZero();
}

}
