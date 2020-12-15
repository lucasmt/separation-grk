#ifndef WEAK_FG_GAME_H
#define WEAK_FG_GAME_H

#include <memory>
#include <vector>

#include "cuddObj.hh"

#include "SeparationGrkSpec.h"
#include "SpaceConnectivity.h"
#include "VarMgr.h"

namespace SGrk {

class WeakFGGame {
	CUDD::BDD all_states_;
	CUDD::BDD initial_states_;
	CUDD::BDD antagonist_transition_relation_;
	CUDD::BDD protagonist_transition_relation_;
	CUDD::BDD path_relation_;
	CUDD::BDD accepting_states_;
	
 public:

	WeakFGGame(const SpaceConnectivity& connectivity,
	           CUDD::BDD antagonist_initial_states,
	           CUDD::BDD protagonist_initial_states,
	           CUDD::BDD antagonist_transition_relation,
	           CUDD::BDD protagonist_transition_reation,
	           CUDD::BDD accepting_states);

	CUDD::BDD AllStates() const;

	CUDD::BDD InitialStates() const;

	CUDD::BDD AntagonistTransitionRelation() const;

	CUDD::BDD ProtagonistTransitionRelation() const;

	CUDD::BDD PathRelation() const;

	CUDD::BDD AcceptingStates() const;
};

}

#endif // WEAK_FG_GAME_H
