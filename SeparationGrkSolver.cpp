#include "SeparationGrkSolver.h"

#include "CycleCover.h"
#include "SpaceConnectivity.h"
#include "WeakFGGame.h"
#include "WeakFGGameSolver.h"

namespace SGrk {

SeparationGrkSolver::SeparationGrkSolver(std::shared_ptr<CUDD::Cudd> mgr,
                                         std::shared_ptr<VarMgr> vars,
                                         SeparationGrkSpec spec)
	: mgr_(std::move(mgr))
	, vars_(std::move(vars))
	, spec_(std::move(spec)) {}

std::optional<SeparationGrkStrategy> SeparationGrkSolver::Run() const {
	CUDD::BDD initial_assumptions = spec_.InitialAssumptions();
	CUDD::BDD initial_guarantees = spec_.InitialGuarantees();
	CUDD::BDD initial_states = initial_assumptions & initial_guarantees;
	
	CUDD::BDD safety_assumptions = spec_.SafetyAssumptions();
	CUDD::BDD safety_guarantees = spec_.SafetyGuarantees();
	CUDD::BDD transition_relation = safety_assumptions & safety_guarantees;

	SpaceConnectivity connectivity(vars_, initial_assumptions, initial_guarantees,
	                               safety_assumptions, safety_guarantees);

	CycleCover cycle_cover(mgr_, vars_, spec_, connectivity);

	CUDD::BDD accepting_states = (connectivity.ValidStates() &
	                              cycle_cover.CoversAllCycles());

	WeakFGGame game(connectivity, initial_assumptions, initial_guarantees,
	                safety_assumptions, safety_guarantees, accepting_states);

	WeakFGGameSolver solver(mgr_, vars_, std::move(game));

	PermissiveStrategy weak_fg_strategy = solver.Run();

	// Forall x . Init_env(x) -> Exists y . Init_sys(y) & Win(x, y)
	bool is_realizable =
		vars_->Forall(vars_->UnprimedInputs(),
		              !initial_assumptions |
		              vars_->Exists(vars_->UnprimedOutputs(),
		                            initial_guarantees &
		                            weak_fg_strategy.WinningStates())).IsOne();

	if (!is_realizable) {
		return std::nullopt;
	} else {
		CUDD::BDD winning_states = weak_fg_strategy.WinningStates();
		CUDD::BDD cycle_covering_states = accepting_states;
		CUDD::BDD reaching_moves =
			!cycle_covering_states & weak_fg_strategy.WinningMoves();
		
		return SeparationGrkStrategy(mgr_, vars_, winning_states, reaching_moves,
		                             cycle_covering_states);
	}
}

}
