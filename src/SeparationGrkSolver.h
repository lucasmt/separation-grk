#ifndef SEPARATION_GRK_SOLVER_H
#define SEPARATION_GRK_SOLVER_H

#include <memory>
#include <optional>
#include <vector>

#include <cuddObj.hh>

#include "SeparationGrkSpec.h"
#include "SeparationGrkStrategy.h"
#include "VarMgr.h"

namespace SGrk {

class SeparationGrkSolver {
	std::shared_ptr<CUDD::Cudd> mgr_;
	std::shared_ptr<VarMgr> vars_;
	SeparationGrkSpec spec_;

 public:
	
	SeparationGrkSolver(std::shared_ptr<CUDD::Cudd> mgr,
	                    std::shared_ptr<VarMgr> vars,
	                    SeparationGrkSpec spec);

	std::optional<SeparationGrkStrategy> Run() const;
};
 
}

#endif // SEPARATION_GRK_SOLVER_H
