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

	CUDD::BDD accepting_states = cycle_cover.CoveredRegion();

	WeakFGGame game(connectivity, initial_assumptions, initial_guarantees,
	                safety_assumptions, safety_guarantees, accepting_states);

	WeakFGGameSolver solver(mgr_, vars_, std::move(game));

	PermissiveStrategy weak_fg_strategy = solver.Run();

	CUDD::BDD initial_moves =
		vars_->UnprimedToPrimed(initial_guarantees &
		                        weak_fg_strategy.WinningStates());
	MemorylessStrategy initial_strategy =
		MemorylessStrategy::Determinize(mgr_, vars_, initial_moves);

	// Check if every input that satisfies the initial assumptions is in the
	// realizable region
	bool is_realizable = (!vars_->UnprimedToPrimed(initial_assumptions) |
	                      initial_strategy.RealizableRegion()).IsOne();

	if (!is_realizable) {
		return std::nullopt;
	} else {
		return SeparationGrkStrategy(mgr_, vars_,
		                             std::move(initial_strategy),
		                             std::move(cycle_cover),
		                             std::move(weak_fg_strategy));
	}
}

}
