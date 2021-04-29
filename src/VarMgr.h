#ifndef VAR_STORE_H
#define VAR_STORE_H

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "cuddObj.hh"

namespace SGrk {

enum class VarType {
	INPUT = 0,
	OUTPUT = 1
};

class Var {
	CUDD::BDD unprimed_;
	CUDD::BDD primed_;
	CUDD::BDD temp_;

 public:
	
	Var(CUDD::BDD unprimed, CUDD::BDD primed, CUDD::BDD temp);

	CUDD::BDD operator()() const;
	CUDD::BDD Prime() const;
	CUDD::BDD Temp() const;
};
 
class VarMgr {
	std::shared_ptr<CUDD::Cudd> mgr_;
	
	std::vector<std::string> input_names_;
	std::vector<std::string> output_names_;

	std::unordered_map<std::string, Var> name_to_var_;

	CUDD::BDD unprimed_input_vars_;
	CUDD::BDD unprimed_output_vars_;
	CUDD::BDD unprimed_vars_;
	CUDD::BDD primed_input_vars_;
	CUDD::BDD primed_output_vars_;
	CUDD::BDD primed_vars_;
	CUDD::BDD temp_input_vars_;
	CUDD::BDD temp_output_vars_;
	CUDD::BDD temp_vars_;
	
	std::vector<CUDD::BDD> unprimed_to_primed_;
	std::vector<CUDD::BDD> primed_to_unprimed_;
	std::vector<CUDD::BDD> swap_primed_and_unprimed_;
	std::vector<CUDD::BDD> primed_to_temp_;
	std::vector<CUDD::BDD> unprimed_to_temp_;
	std::vector<std::string> var_labels_;

	CUDD::BDD reflexive_;

 public:
	
	VarMgr(std::shared_ptr<CUDD::Cudd> mgr);

	Var NewVar(VarType var_type, const std::string& name);

	std::optional<Var> FindVar(const std::string& name) const;

	std::size_t InputVarCount() const;
	std::size_t OutputVarCount() const;
	
	CUDD::BDD UnprimedVar(const std::string& name) const;
	CUDD::BDD PrimedVar(const std::string& name) const;
	CUDD::BDD TempVar(const std::string& name) const;

	CUDD::BDD UnprimedInputs() const;
	CUDD::BDD UnprimedOutputs() const;
	CUDD::BDD UnprimedVars() const;
	CUDD::BDD PrimedInputs() const;
	CUDD::BDD PrimedOutputs() const;
	CUDD::BDD PrimedVars() const;
	CUDD::BDD TempVars() const;
	
	CUDD::BDD UnprimedToPrimed(const CUDD::BDD& bdd) const;
	CUDD::BDD PrimedToUnprimed(const CUDD::BDD& bdd) const;
	CUDD::BDD SwapPrimedAndUnprimed(const CUDD::BDD& bdd) const;
	CUDD::BDD PrimedToTemp(const CUDD::BDD& bdd) const;
	CUDD::BDD UnprimedToTemp(const CUDD::BDD& bdd) const;

	CUDD::BDD Forall(const CUDD::BDD& vars, const CUDD::BDD& bdd) const;
	CUDD::BDD Exists(const CUDD::BDD& vars, const CUDD::BDD& bdd) const;

	std::vector<int> ZeroedTransitionVector() const;

	std::string ToString(const CUDD::BDD& bdd) const;

	CUDD::BDD Reflexive() const;

	void DumpDot(const std::vector<CUDD::BDD>& bdds,
	             const std::vector<std::string>& labels,
	             const std::string& filename) const;
	void DumpDot(const std::vector<CUDD::ADD>& bdds,
	             const std::vector<std::string>& labels,
	             const std::string& filename) const;
};
	
}

#endif // VAR_STORE_H
