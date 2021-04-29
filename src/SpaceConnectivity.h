#ifndef SPACE_CONNECTIVITY_H
#define SPACE_CONNECTIVITY_H

#include <memory>

#include "cuddObj.hh"

#include "VarMgr.h"

namespace SGrk {

class SpaceConnectivity {
	std::shared_ptr<VarMgr> vars_;
	CUDD::BDD environment_valid_states_;
	CUDD::BDD system_valid_states_;
	CUDD::BDD valid_states_;
	CUDD::BDD environment_path_relation_;
	CUDD::BDD system_path_relation_;
	CUDD::BDD path_relation_;
	CUDD::BDD environment_bipath_relation_;
	CUDD::BDD system_bipath_relation_;
	CUDD::BDD bipath_relation_;
	
 public:
	SpaceConnectivity(std::shared_ptr<VarMgr> vars,
	                  const CUDD::BDD& environment_initial_states,
	                  const CUDD::BDD& system_initial_states,
	                  const CUDD::BDD& environment_transition_relation,
	                  const CUDD::BDD& system_transition_relation);

	CUDD::BDD TransitiveClosure(const CUDD::BDD& relation) const;

	CUDD::BDD EnvironmentValidStates() const;
	CUDD::BDD SystemValidStates() const;
	CUDD::BDD ValidStates() const;
	
	CUDD::BDD EnvironmentPathRelation() const;
	CUDD::BDD SystemPathRelation() const;
	CUDD::BDD PathRelation() const;

	CUDD::BDD EnvironmentBipathRelation() const;
	CUDD::BDD SystemBipathRelation() const;
	CUDD::BDD BipathRelation() const;
};

}
 
#endif // SPACE_CONNECTIVITY_H
