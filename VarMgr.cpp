#include "VarMgr.h"

#include <algorithm>
#include <cstring>
#include <sstream>

namespace SGrk {

Var::Var(CUDD::BDD unprimed, CUDD::BDD primed, CUDD::BDD temp)
	: unprimed_(std::move(unprimed))
	, primed_(std::move(primed))
	, temp_(std::move(temp)) {}

CUDD::BDD Var::operator()() const {
	return unprimed_;
}

CUDD::BDD Var::Prime() const {
	return primed_;
}

CUDD::BDD Var::Temp() const {
	return temp_;
}

VarMgr::VarMgr(std::shared_ptr<CUDD::Cudd> mgr)
	: mgr_(std::move(mgr))
	, unprimed_input_vars_(mgr_->bddOne())
	, unprimed_output_vars_(mgr_->bddOne())
	, unprimed_vars_(mgr_->bddOne())
	, primed_input_vars_(mgr_->bddOne())
	, primed_output_vars_(mgr_->bddOne())
	, primed_vars_(mgr_->bddOne())
	, temp_input_vars_(mgr_->bddOne())
	, temp_output_vars_(mgr_->bddOne())
	, temp_vars_(mgr_->bddOne()) {}

Var VarMgr::NewVar(VarType var_type, const std::string& name) {
	CUDD::BDD unprimed = mgr_->bddVar();
	CUDD::BDD primed = mgr_->bddVar();
	CUDD::BDD temp = mgr_->bddVar();
	Var var(unprimed, primed, temp);

	std::vector<std::string>& names =
		(var_type == VarType::INPUT) ? input_names_ : output_names_;
	CUDD::BDD& xput_unprimed_vars =
		(var_type == VarType::INPUT) ? unprimed_input_vars_ : unprimed_output_vars_;
	CUDD::BDD& xput_primed_vars =
		(var_type == VarType::INPUT) ? primed_input_vars_ : primed_output_vars_;
	CUDD::BDD& xput_temp_vars =
		(var_type == VarType::INPUT) ? temp_input_vars_ : temp_output_vars_;

	names.push_back(name);
	name_to_var_.emplace(name, var);
	xput_unprimed_vars &= unprimed;
	unprimed_vars_ &= unprimed;
	xput_primed_vars &= primed;
	primed_vars_ &= primed;
	xput_temp_vars &= temp;
	temp_vars_ &= temp;

	unprimed_to_primed_.push_back(primed);
	unprimed_to_primed_.push_back(primed);
	unprimed_to_primed_.push_back(temp);

	primed_to_unprimed_.push_back(unprimed);
	primed_to_unprimed_.push_back(unprimed);
	primed_to_unprimed_.push_back(temp);

	swap_primed_and_unprimed_.push_back(primed);
	swap_primed_and_unprimed_.push_back(unprimed);
	swap_primed_and_unprimed_.push_back(temp);

	primed_to_temp_.push_back(unprimed);
	primed_to_temp_.push_back(temp);
	primed_to_temp_.push_back(temp);

	unprimed_to_temp_.push_back(temp);
	unprimed_to_temp_.push_back(primed);
	unprimed_to_temp_.push_back(temp);

	var_labels_.push_back(name);
	var_labels_.push_back(name + "'");
	var_labels_.push_back(name + "*");
	
	return var;
}

std::optional<Var> VarMgr::FindVar(const std::string& name) const {
	std::unordered_map<std::string, Var>::const_iterator it =
		name_to_var_.find(name);

	if (it == name_to_var_.end()) {
		return std::nullopt;
	} else {
		return it->second;
	}
}

std::size_t VarMgr::InputVarCount() const {
	return input_names_.size();
}

std::size_t VarMgr::OutputVarCount() const {
	return output_names_.size();
}

CUDD::BDD VarMgr::UnprimedVar(const std::string& name) const {
	return name_to_var_.at(name)();
}

CUDD::BDD VarMgr::PrimedVar(const std::string& name) const {
	return name_to_var_.at(name).Prime();
}

CUDD::BDD VarMgr::TempVar(const std::string& name) const {
	return name_to_var_.at(name).Temp();
}

CUDD::BDD VarMgr::UnprimedInputs() const {
	return unprimed_input_vars_;
}

CUDD::BDD VarMgr::UnprimedOutputs() const {
	return unprimed_output_vars_;
}

CUDD::BDD VarMgr::UnprimedVars() const {
	return unprimed_vars_;
}

CUDD::BDD VarMgr::PrimedInputs() const {
	return primed_input_vars_;
}

CUDD::BDD VarMgr::PrimedOutputs() const {
	return primed_output_vars_;
}

CUDD::BDD VarMgr::PrimedVars() const {
	return primed_vars_;
}

CUDD::BDD VarMgr::TempVars() const {
	return temp_vars_;
}

CUDD::BDD VarMgr::UnprimedToPrimed(const CUDD::BDD& bdd) const {
	return bdd.VectorCompose(unprimed_to_primed_);
}

CUDD::BDD VarMgr::PrimedToUnprimed(const CUDD::BDD& bdd) const {
	return bdd.VectorCompose(primed_to_unprimed_);
}

CUDD::BDD VarMgr::SwapPrimedAndUnprimed(const CUDD::BDD& bdd) const {
	return bdd.VectorCompose(swap_primed_and_unprimed_);
}

CUDD::BDD VarMgr::PrimedToTemp(const CUDD::BDD& bdd) const {
	return bdd.VectorCompose(primed_to_temp_);
}

CUDD::BDD VarMgr::UnprimedToTemp(const CUDD::BDD& bdd) const {
	return bdd.VectorCompose(unprimed_to_temp_);
}

CUDD::BDD VarMgr::Forall(const CUDD::BDD& vars, const CUDD::BDD& bdd) const {
	return bdd.UnivAbstract(vars);
}

CUDD::BDD VarMgr::Exists(const CUDD::BDD& vars, const CUDD::BDD& bdd) const {
	return bdd.ExistAbstract(vars);
}

std::vector<int> VarMgr::ZeroedTransitionVector() const {
	std::size_t cudd_var_count = 3 * InputVarCount() + 3 * OutputVarCount();
	
	return std::vector<int>(cudd_var_count, 0);
}
	
std::string VarMgr::ToString(const CUDD::BDD& bdd) const {
	std::stringstream ss;
	ss << bdd;
	std::string s = ss.str();

	while (true) {
		std::size_t pos = s.find("x");

		if (pos == std::string::npos) {
			break;
		}

		s.replace(pos, 1, "$");
	}

	// Sort ids from highest to lowest so that ids with more digits get replaced
	// before ids with fewer digits.
	std::vector<unsigned int> ids = bdd.SupportIndices();
	std::sort(ids.begin(), ids.end(), std::greater<unsigned int>());
	
	for (unsigned int id : ids) {
		std::string var_string = "$" + std::to_string(id);

		while (true) {
			std::size_t pos = s.find(var_string);

			if (pos == std::string::npos) {
				break;
			}

			s.replace(pos, var_string.size(), var_labels_[id]);
		}
	}

	return s;
}

void VarMgr::DumpDot(const std::vector<CUDD::BDD>& bdds,
                     const std::vector<std::string>& labels,
                     const std::string& filename) const {
	std::vector<CUDD::ADD> adds;

	for (const CUDD::BDD& bdd : bdds) {
		adds.push_back(bdd.Add());
	}

	return DumpDot(adds, labels, filename);
}
	
void VarMgr::DumpDot(const std::vector<CUDD::ADD>& bdds,
                     const std::vector<std::string>& labels,
                     const std::string& filename) const { 
  std::size_t var_count = var_labels_.size();
  std::vector<char*> inames(var_count);

  for (std::size_t i = 0; i < var_count; ++i) {
    std::string label = var_labels_[i];

    // Return value of label.c_str() is temporary, so need to make a copy
    inames[i] = new char[label.length() + 1]; // need space for '\0' terminator
    std::strcpy(inames[i], label.c_str());
  }

  std::size_t bdd_count = bdds.size();
  std::vector<char*> onames(bdd_count);

  for (std::size_t i = 0; i < bdd_count; ++i) {
    std::string label = labels[i];

    // Return value of label.c_str() is temporary, so need to make a copy
    onames[i] = new char[label.length() + 1]; // need space for '\0' terminator
    std::strcpy(onames[i], label.c_str());
  }

  FILE* fp = fopen(filename.c_str(), "w");

  mgr_->DumpDot(bdds, inames.data(), onames.data(), fp);

  fclose(fp);

  for (std::size_t i = 0; i < var_count; ++i) {
    delete[] inames[i];
  }

  for (std::size_t i = 0; i < bdd_count; ++i) {
    delete[] onames[i];
  }
}	
}
