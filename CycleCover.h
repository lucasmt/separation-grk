#ifndef CYCLE_COVER_H
#define CYCLE_COVER_H

#include <memory>
#include <vector>

#include "cuddObj.hh"

#include "PermissiveStrategy.h"
#include "SeparationGrkSpec.h"
#include "SpaceConnectivity.h"
#include "VarMgr.h"

namespace SGrk {

class CycleCover {
	std::shared_ptr<CUDD::Cudd> mgr_;
	std::shared_ptr<VarMgr> vars_;

	std::size_t implication_count_;
	std::vector<std::size_t> assumption_count_;
	std::vector<std::size_t> guarantee_count_;
	std::vector<std::vector<CUDD::BDD>> can_satisfy_assumption_;
	std::vector<std::vector<CUDD::BDD>> can_satisfy_guarantee_;
	std::vector<CUDD::BDD> can_satisfy_assumptions_;
	std::vector<CUDD::BDD> can_satisfy_guarantees_;
	std::vector<CUDD::BDD> covers_cycles_;
	CUDD::BDD covers_all_cycles_;
	std::vector<std::vector<std::size_t>> guarantee_index_;
	std::vector<PermissiveStrategy> cycle_strategy_;
	std::vector<CUDD::ADD> cycle_continuation_;

	CUDD::BDD HasCycle(const CUDD::BDD& connected, const CUDD::BDD& prop) const;
	void ComputeCounts(const SeparationGrkSpec& spec);
	void ComputeCycles(const SeparationGrkSpec& spec,
	                   const SpaceConnectivity& connectivity);
	void ComputeCoverage(const SeparationGrkSpec& spec,
	                     const SpaceConnectivity& connectivity);
	void ComputeIndices(const SeparationGrkSpec& spec);
	void ComputeStrategy(const SeparationGrkSpec& spec,
	                     const SpaceConnectivity& connectivity);
	void ComputeContinuation(const SeparationGrkSpec& spec);
	
 public:
	CycleCover(std::shared_ptr<CUDD::Cudd> mgr, std::shared_ptr<VarMgr> vars,
	           const SeparationGrkSpec& spec,
	           const SpaceConnectivity& connectivity);

	CUDD::BDD CoversAllCycles() const;

	const std::vector<CUDD::BDD>& AssumptionComponents() const;
	const std::vector<CUDD::BDD>& GuaranteeComponents() const;
	const std::vector<CUDD::BDD>& CycleCoverage() const;
	const std::vector<PermissiveStrategy>& CycleStrategy() const;
	const std::vector<CUDD::ADD>& CycleContinuation() const;
};

}

#endif // CYCLE_COVER_H
