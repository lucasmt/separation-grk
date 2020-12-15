#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "cuddObj.hh"

#include "SeparationGrkSolver.h"
#include "SeparationGrkSpec.h"
#include "SeparationGrkStrategy.h"
#include "VarMgr.h"

#include "Driver.h"

extern FILE* yyin;

int main(int argc, char* argv[]) {
	std::shared_ptr<CUDD::Cudd> mgr = std::make_shared<CUDD::Cudd>();
	mgr->AutodynEnable();
	std::shared_ptr<SGrk::VarMgr> vars = std::make_shared<SGrk::VarMgr>(mgr);

	if (argc < 2) {
		std::cout << "Usage: "
		          << std::string(argv[0]) << " <input-file> [<output-dot-file>]"
		          << std::endl;

		return 0;
	}
	
	Driver driver(mgr, vars);
	driver.Parse(std::string(argv[1]));
	
	const SGrk::SeparationGrkSpec& spec = driver.spec;

	SGrk::SeparationGrkSolver solver(mgr, vars, spec);

	std::optional<SGrk::SeparationGrkStrategy> strategy = solver.Run();
	
	if (strategy) {
		std::cout << "Realizable" << std::endl;

		if (argc > 2) {
			std::vector<CUDD::BDD> bdds = {
				strategy->WinningStates(),
				strategy->CycleCoveringStates(),
				strategy->ReachingMoves()
			};

			vars->DumpDot(bdds, { "FG(CC)", "CC", "T" }, std::string(argv[2]));
		}
	} else {
		std::cout << "Unrealizable" << std::endl;
	}

	return 0;
}
