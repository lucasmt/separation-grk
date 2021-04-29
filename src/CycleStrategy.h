#ifndef CYCLE_STRATEGY_H
#define CYCLE_STRATEGY_H

#include <memory>
#include <vector>

#include "cuddObj.hh"

#include "MemorylessStrategy.h"
#include "VarMgr.h"

namespace SGrk {

class PathStrategy {
	CUDD::BDD realizable_region_;
	std::vector<MemorylessStrategy> strategy_parts_;
	// Remember to use primed variables for the following:
	std::vector<CUDD::BDD> stopping_conditions_;

 public:

	PathStrategy(CUDD::BDD realizable_region,
	             std::vector<MemorylessStrategy> strategy_parts,
	             std::vector<CUDD::BDD> stopping_conditions);

	CUDD::BDD RealizableRegion() const;
	const std::vector<MemorylessStrategy>& StrategyParts() const;
	const std::vector<CUDD::BDD>& StoppingConditions() const;
};
	
class CycleStrategy {
	std::shared_ptr<CUDD::Cudd> mgr_;
	std::shared_ptr<VarMgr> vars_;
	std::vector<MemorylessStrategy> strategy_parts_;
	std::vector<CUDD::BDD> stopping_conditions_;
	std::vector<CUDD::ADD> continuations_;
	std::vector<CUDD::BDD> reset_;
	CUDD::BDD idle_region_;

 public:

	CycleStrategy(std::shared_ptr<CUDD::Cudd> mgr, std::shared_ptr<VarMgr> vars,
	              MemorylessStrategy idle_strategy);

	void Merge(const PathStrategy& other);

	int InitialIndex(std::vector<int>& transition_vector) const;
	
	int Step(std::vector<int>& transition_vector, int i) const;
};

}

#endif // CYCLE_STRATEGY_H
