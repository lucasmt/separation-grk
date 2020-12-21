#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "cuddObj.hh"

#include "SeparationGrkSolver.h"
#include "SeparationGrkSpec.h"
#include "SeparationGrkStrategy.h"
#include "VarMgr.h"

#include "Driver.h"

extern FILE* yyin;

bool ValidateArguments(int argc, char* argv[],
                       const std::vector<std::string>& arg_options,
                       std::unordered_map<std::string, std::string>& arg_map) {
	if (argc < 2)	return false;

	std::string arg_options_regex = arg_options[0];

	for (int i = 1; i < arg_options.size(); ++i) {
		arg_options_regex += "+" + arg_options[i];
	}

	for (int i = 2; i < argc; ++i) {
		std::string arg(argv[i]);
		std::regex regex("^--(" + arg_options_regex + ")=([a-zA-Z0-9_.]*)$");
		std::smatch match;

		std::regex_match(arg, match, regex);

		if (match.empty()) {
			std::cout << "Invalid option: " << arg << std::endl;
			return false;
		}
		
		arg_map[match[1]] = match[2];
	}

	return true;
}

int main(int argc, char* argv[]) {
	std::shared_ptr<CUDD::Cudd> mgr = std::make_shared<CUDD::Cudd>();
	mgr->AutodynEnable();
	std::shared_ptr<SGrk::VarMgr> vars = std::make_shared<SGrk::VarMgr>(mgr);

	std::string dot_option = "dotfile";
	std::vector<std::string> arg_options = { dot_option };
	std::unordered_map<std::string, std::string> arg_map;

	if (!ValidateArguments(argc, argv, arg_options, arg_map)) {
		std::cout << "Usage: "
		          << std::string(argv[0]) << " <spec-file> "
		          << "[--" << dot_option << "=<output-dot-file>]"
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

		auto dot_file = arg_map.find(dot_option);
		
		if (dot_file != arg_map.end()) {
			std::vector<CUDD::BDD> bdds = {
				strategy->WinningStates(),
				strategy->CycleCoveringStates(),
				strategy->ReachingMoves()
			};

			vars->DumpDot(bdds, { "FG(CC)", "CC", "T" }, dot_file->second);
		}
	} else {
		std::cout << "Unrealizable" << std::endl;
	}

	return 0;
}
