#ifndef SEPARATION_GRK_PLAYER_H
#define SEPARATION_GRK_PLAYER_H

#include <iostream>
#include <memory>
#include <vector>

#include "cuddObj.hh"

#include "SeparationGrkSpec.h"
#include "SeparationGrkStrategy.h"
#include "VarMgr.h"

namespace SGrk {

class SeparationGrkPlayer {
	std::shared_ptr<CUDD::Cudd> mgr_;
	std::shared_ptr<VarMgr> vars_;
	SeparationGrkSpec spec_;
	SeparationGrkStrategy strategy_;
	
	bool AskForInput(std::istream& in,
	                 const CUDD::BDD& constraint,
	                 std::vector<int>& transition_vector) const;
	
	CUDD::BDD NewState(const std::vector<int>& transition_vector) const;

 public:
	
	SeparationGrkPlayer(std::shared_ptr<CUDD::Cudd> mgr,
	                    std::shared_ptr<VarMgr> vars,
	                    SeparationGrkSpec spec,
	                    SeparationGrkStrategy strategy);

	void Play(std::istream& in) const;
};

}

#endif // SEPARATION_GRK_PLAYER_H
