#ifndef MEMORYLESS_STRATEGY_H
#define MEMORYLESS_STRATEGY_H

#include <unordered_map>
#include <vector>

#include "cuddObj.hh"

#include "VarMgr.h"

namespace SGrk {

class MemorylessStrategy {
	std::shared_ptr<CUDD::Cudd> mgr_;
	std::shared_ptr<VarMgr> vars_;
	CUDD::BDD realizable_region_;
	std::unordered_map<int, CUDD::BDD> strategy_;

 public:

	MemorylessStrategy(std::shared_ptr<CUDD::Cudd> mgr,
	                   std::shared_ptr<VarMgr> vars,
	                   CUDD::BDD realizable_region,
	                   std::unordered_map<int, CUDD::BDD> strategy);
	
	static MemorylessStrategy Determinize(std::shared_ptr<CUDD::Cudd> mgr,
	                                      std::shared_ptr<VarMgr> vars,
	                                      const CUDD::BDD& winning_moves);

	CUDD::BDD RealizableRegion() const;
	
	void Update(std::vector<int>& transition_vector) const;
};

}

#endif // MEMORYLESS_STRATEGY_H
