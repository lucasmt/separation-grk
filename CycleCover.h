#ifndef CYCLE_COVER_H
#define CYCLE_COVER_H

#include <memory>
#include <vector>

#include "cuddObj.hh"

#include "CycleStrategy.h"
#include "MemorylessStrategy.h"
#include "SeparationGrkSpec.h"
#include "SpaceConnectivity.h"
#include "VarMgr.h"

namespace SGrk {

class CycleCover {
	std::shared_ptr<CUDD::Cudd> mgr_;
	std::shared_ptr<VarMgr> vars_;
	CUDD::BDD covered_region_;
	CycleStrategy cycle_strategy_;

	MemorylessStrategy ComputeReachabilityStrategy(
	  const CUDD::BDD& transition_relation,
	  const CUDD::BDD& bipath_relation,
	  const CUDD::BDD& goal) const;

	PathStrategy ComputePathStrategy(
    const CUDD::BDD& transition_relation,
    const CUDD::BDD& bipath_relation,
    const std::vector<CUDD::BDD>& goals) const;

	CycleStrategy ComputeCycleStrategy(
    const SeparationGrkSpec& spec,
    const SpaceConnectivity& connectivity) const;

	CUDD::BDD HasCycle(const CUDD::BDD& connected, const CUDD::BDD& prop) const;

	CUDD::BDD ComputeCoveredRegion(const SeparationGrkSpec& spec,
	                               const SpaceConnectivity& connectivity) const;
	
 public:

	CycleCover(std::shared_ptr<CUDD::Cudd> mgr, std::shared_ptr<VarMgr> vars,
	           const SeparationGrkSpec& spec,
	           const SpaceConnectivity& connectivity);

	CUDD::BDD CoveredRegion() const;
	const CycleStrategy& Strategy() const;
};

}

#endif // CYCLE_COVER_H
