#include "SeparationGrkPlayer.h"

#include <algorithm>

#include "Util.h"

namespace SGrk {

SeparationGrkPlayer::SeparationGrkPlayer(
  std::shared_ptr<CUDD::Cudd> mgr,
  std::shared_ptr<VarMgr> vars,
  SeparationGrkSpec spec,
  SeparationGrkStrategy strategy)
	: mgr_(std::move(mgr))
	, vars_(std::move(vars))
	, spec_(std::move(spec))
	, strategy_(std::move(strategy)) {}

bool SeparationGrkPlayer::AskForInput(std::istream& in,
                                      const CUDD::BDD& constraint,
                                      std::vector<int>& transition_vector)
	  const {
	CUDD::BDD input_vars = vars_->UnprimedInputs();
	CUDD::BDD primed_input_vars = vars_->PrimedInputs();
	std::vector<unsigned int> input_indices = primed_input_vars.SupportIndices();
	std::size_t input_size = vars_->InputVarCount();
	
	while (true) {
		std::cout << "Please enter inputs (" << vars_->ToString(input_vars)
		          << "):" << std::endl;

		std::string input_string;
		in >> input_string;

		if (input_string == "quit") {
			return 1;
		}

		if (input_string.length() != input_size) {
			std::cout << "Need " << input_size << " inputs." << std::endl;
			continue;
		}

		if (std::any_of(input_string.begin(), input_string.end(),
		                [] (char bit) { return bit != '0' && bit != '1'; })) {
				std::cout << "Please only input 0s and 1s." << std::endl;
				continue;
		}

		for (int i = 0; i < input_size; ++i) {
			unsigned int index = input_indices[i];
			transition_vector[index] = input_string[i] - '0';
		}

		if (Util::EvalBdd(constraint, transition_vector) == 0) {
			std::cout << "Invalid input." << std::endl;
			continue;
		}

		return 0;
	}
}

CUDD::BDD
SeparationGrkPlayer::NewState(const std::vector<int>& transition_vector) const {
	CUDD::BDD primed_state = mgr_->bddOne();
		
	for (unsigned int index : vars_->PrimedVars().SupportIndices()) {
		if (transition_vector[index] == 1) {
			primed_state &= mgr_->bddVar(index);
		}
	}

	return vars_->PrimedToUnprimed(primed_state);
}

void SeparationGrkPlayer::Play(std::istream& in) const {
	std::vector<int> transition_vector = vars_->ZeroedTransitionVector();

	// Modifies transition_vector
	bool quit = AskForInput(in,
	                        vars_->UnprimedToPrimed(spec_.InitialAssumptions()),
	                        transition_vector);

	if (quit) {
		return;
	}

	// Modifies transition_vector
	strategy_.GetInitialStrategy().Update(transition_vector);

	CUDD::BDD accepting_states =
		strategy_.CycleCoveringStates();
	const MemorylessStrategy& reachability_strategy =
		strategy_.GetReachabilityStrategy();
	const CycleStrategy& cycle_strategy =
		strategy_.GetCycleStrategy();

	int cycle_index = cycle_strategy.InitialIndex(transition_vector);

	while (true) {
		CUDD::BDD current_state = NewState(transition_vector);
		
		std::cout << "Current state: " << vars_->ToString(current_state)
		          << std::endl;

		std::fill(transition_vector.begin(), transition_vector.end(), 0);

		for (unsigned int index : current_state.SupportIndices()) {
			transition_vector[index] = 1;
		}

		bool can_cover_cycles = Util::EvalBdd(accepting_states, transition_vector);
		
		bool quit = AskForInput(in, spec_.SafetyAssumptions(), transition_vector);

		if (quit) {
			return;
		}
		
		if (!can_cover_cycles) {
			std::cout << "Cannot cover cycles, moving to another component..."
			          << std::endl;
			
			reachability_strategy.Update(transition_vector);

			cycle_index = cycle_strategy.InitialIndex(transition_vector);
		} else {
			std::cout << "Covering cycles..." << std::endl;

			std::cout << "Cycle index: " << cycle_index << std::endl;
			
			cycle_index = cycle_strategy.Step(transition_vector, cycle_index);
		}
	}
}

}
