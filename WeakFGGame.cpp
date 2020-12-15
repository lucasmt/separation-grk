#include "WeakFGGame.h"

namespace SGrk {

WeakFGGame::WeakFGGame(const SpaceConnectivity& connectivity,
                       CUDD::BDD antagonist_initial_states,
                       CUDD::BDD protagonist_initial_states,
                       CUDD::BDD antagonist_transition_relation,
                       CUDD::BDD protagonist_transition_relation,
                       CUDD::BDD accepting_states)
	: all_states_(connectivity.ValidStates())
	, path_relation_(connectivity.PathRelation())
	, initial_states_(antagonist_initial_states & protagonist_initial_states)
	, antagonist_transition_relation_(std::move(antagonist_transition_relation))
	, protagonist_transition_relation_(std::move(protagonist_transition_relation))
	, accepting_states_(std::move(accepting_states)) {}

CUDD::BDD WeakFGGame::AllStates() const {
	return all_states_;
}

CUDD::BDD WeakFGGame::InitialStates() const {
	return initial_states_;
}

CUDD::BDD WeakFGGame::AntagonistTransitionRelation() const {
	return antagonist_transition_relation_;
}

CUDD::BDD WeakFGGame::ProtagonistTransitionRelation() const {
	return protagonist_transition_relation_;
}

CUDD::BDD WeakFGGame::PathRelation() const {
	return path_relation_;
}

CUDD::BDD WeakFGGame::AcceptingStates() const {
	return accepting_states_;
}

}
