#ifndef SEPARATION_GRK_SPEC_H
#define SEPARATION_GRK_SPEC_H

#include <memory>
#include <vector>

#include <cuddObj.hh>

namespace SGrk {

class SeparationGrkImplication {
	std::vector<CUDD::BDD> assumptions_;
	std::vector<CUDD::BDD> guarantees_;

 public:

	SeparationGrkImplication() {}
	
	SeparationGrkImplication(std::vector<CUDD::BDD> assumptions,
	                         std::vector<CUDD::BDD> guarantees)
	: assumptions_(std::move(assumptions))
	, guarantees_(std::move(guarantees)) {}

	const std::vector<CUDD::BDD>& Assumptions() const { return assumptions_; }
	const std::vector<CUDD::BDD>& Guarantees() const { return guarantees_; }
};

class SeparationGrkSpec {
	CUDD::BDD initial_assumptions_;
	CUDD::BDD initial_guarantees_;
	CUDD::BDD safety_assumptions_;
	CUDD::BDD safety_guarantees_;
	std::vector<SeparationGrkImplication> justice_implications_;

 public:

	SeparationGrkSpec() {}
	
	SeparationGrkSpec(
	    CUDD::BDD initial_assumptions,
	    CUDD::BDD initial_guarantees,
	    CUDD::BDD safety_assumptions,
	    CUDD::BDD safety_guarantees,
	    std::vector<SeparationGrkImplication> justice_implications)
	: initial_assumptions_(std::move(initial_assumptions))
	, initial_guarantees_(std::move(initial_guarantees))
	, safety_assumptions_(std::move(safety_assumptions))
	, safety_guarantees_(std::move(safety_guarantees))
	, justice_implications_(std::move(justice_implications)) {}

	CUDD::BDD InitialAssumptions() const { return initial_assumptions_; }
	CUDD::BDD InitialGuarantees() const { return initial_guarantees_; }
	CUDD::BDD SafetyAssumptions() const { return safety_assumptions_; }
	CUDD::BDD SafetyGuarantees() const { return safety_guarantees_; }
	const std::vector<SeparationGrkImplication> JusticeImplications() const {
		return justice_implications_;
	}
};
 
}

#endif // SEPARATION_GRK_SPEC_H
