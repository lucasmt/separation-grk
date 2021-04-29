#include "SpaceConnectivity.h"

namespace SGrk {

SpaceConnectivity::SpaceConnectivity(
    std::shared_ptr<VarMgr> vars,
    const CUDD::BDD& environment_initial_states,
    const CUDD::BDD& system_initial_states,
    const CUDD::BDD& environment_transition_relation,
    const CUDD::BDD& system_transition_relation)
	  : vars_(std::move(vars)) {
	CUDD::BDD initial_states =
		system_initial_states & environment_initial_states;
	CUDD::BDD transition_relation =
		system_transition_relation & environment_transition_relation;
	
	system_path_relation_ =
		TransitiveClosure(system_transition_relation);
	environment_path_relation_ =
		TransitiveClosure(environment_transition_relation);
	path_relation_ =
		TransitiveClosure(transition_relation);

	// Reachable(s') = Exists s . I(s) & Path(s, s')
	CUDD::BDD system_reachable_states =
		vars_->Exists(vars_->UnprimedOutputs(), (system_initial_states &
		                                        system_path_relation_));
	CUDD::BDD environment_reachable_states =
		vars_->Exists(vars_->UnprimedInputs(), (environment_initial_states &
		                                       environment_path_relation_));
	CUDD::BDD reachable_states =
		vars_->Exists(vars_->UnprimedVars(), initial_states & path_relation_);

	// Valid(s) = I(s) | Reachable(s)
	system_valid_states_ =
		system_initial_states |
		vars_->PrimedToUnprimed(system_reachable_states);
	environment_valid_states_ =
		environment_initial_states |
		vars_->PrimedToUnprimed(environment_reachable_states);
	valid_states_ =
		initial_states |
		vars_->PrimedToUnprimed(reachable_states);

	// Bipath(s, s') = Path(s, s') & Path(s', s)
	system_bipath_relation_ =
		system_path_relation_ &
		vars_->SwapPrimedAndUnprimed(system_path_relation_);
	environment_bipath_relation_ =
		environment_path_relation_ &
		vars_->SwapPrimedAndUnprimed(environment_path_relation_);
	bipath_relation_ =
		path_relation_ &
		vars_->SwapPrimedAndUnprimed(path_relation_);
}

CUDD::BDD
SpaceConnectivity::TransitiveClosure(const CUDD::BDD& relation) const {
	// Closure_0(s, s') = R(s, s')
	CUDD::BDD closure = relation;

	while (true) {
		// Transitive_i(s, t, s') = Closure_{i-1}(s, t) & Closure_{i-1}(t, s')
		CUDD::BDD transitive =
			vars_->PrimedToTemp(closure) &
			vars_->UnprimedToTemp(closure);

		// Closure_i(s, s') = Closure_{i-1}(s, s') |
		//                    Exists t . Transitive_i(s, t, s')
		CUDD::BDD new_closure = (closure |
		                         vars_->Exists(vars_->TempVars(), transitive));

		if (new_closure == closure) {
			return closure;
		}

		closure = new_closure;
	}
}

CUDD::BDD SpaceConnectivity::EnvironmentValidStates() const {
	return environment_valid_states_;
}

CUDD::BDD SpaceConnectivity::SystemValidStates() const {
	return system_valid_states_;
}

CUDD::BDD SpaceConnectivity::ValidStates() const {
	return valid_states_;
}

CUDD::BDD SpaceConnectivity::EnvironmentPathRelation() const {
	return environment_path_relation_;
}

CUDD::BDD SpaceConnectivity::SystemPathRelation() const {
	return system_path_relation_;
}

CUDD::BDD SpaceConnectivity::PathRelation() const {
	return path_relation_;
}

CUDD::BDD SpaceConnectivity::EnvironmentBipathRelation() const {
	return environment_bipath_relation_;
}

CUDD::BDD SpaceConnectivity::SystemBipathRelation() const {
	return system_bipath_relation_;
}

CUDD::BDD SpaceConnectivity::BipathRelation() const {
	return bipath_relation_;
}

}
