#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "cuddObj.hh"

#include "SeparationGrkPlayer.h"
#include "SeparationGrkSolver.h"
#include "SeparationGrkSpec.h"
#include "SeparationGrkStrategy.h"
#include "TestSet.h"
#include "VarMgr.h"

#include "Driver.h"

extern FILE* yyin;

bool ValidateArguments(int argc, char* argv[],
                       const std::vector<std::string>& arg_options,
                       std::unordered_map<std::string, std::string>& arg_map) {
	if (argc < 2)	return false;

	std::string arg_options_regex = arg_options[0];

	for (int i = 1; i < arg_options.size(); ++i) {
		arg_options_regex += "|" + arg_options[i];
	}

	for (int i = 2; i < argc; ++i) {
		std::string arg(argv[i]);
		std::regex regex("^--(" + arg_options_regex + ")=([a-zA-Z0-9_./-]*)$");
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

void RunTests(const std::vector<CUDD::BDD>& bdds, const std::string& test_file,
              const std::shared_ptr<SGrk::VarMgr>& vars) {
	try {
		SGrk::TestSet test_set =
			SGrk::TestSet::ReadFromFile(test_file, vars);

		std::vector<std::vector<SGrk::TestResult>> test_results =
			test_set.Run(bdds);

		for (int i = 0; i < test_results.size(); ++i) {
			std::cout << i << ":";

			for (int j = 0; j < test_results[i].size(); ++j) {
				if (test_results[i][j] == SGrk::TestResult::PASS) {
					std::cout << " PASS";
				} else {
					std::cout << " FAIL";
				}
			}

			std::cout << std::endl;
		}
	} catch (const std::runtime_error& e) {
		std::cout << "Error reading " << test_file << ": " << e.what() << std::endl;
	}
}

void PlayGame(const SGrk::SeparationGrkSpec& spec,
              const SGrk::SeparationGrkStrategy& strategy,
              const std::string& filename,
              const std::shared_ptr<CUDD::Cudd> mgr,
              const std::shared_ptr<SGrk::VarMgr> vars) {
	SGrk::SeparationGrkPlayer player(mgr, vars, spec, strategy);

	if (filename == "stdin") {
		player.Play(std::cin);
	} else {
		std::ifstream in(filename);
		player.Play(in);
	}
}

int main(int argc, char* argv[]) {
	std::shared_ptr<CUDD::Cudd> mgr = std::make_shared<CUDD::Cudd>();
	mgr->AutodynEnable();
	std::shared_ptr<SGrk::VarMgr> vars = std::make_shared<SGrk::VarMgr>(mgr);

	std::string test_option = "test";
	std::string dot_option = "dumpdot";
	std::string play_option = "play";
	std::vector<std::string> arg_options = { test_option,
	                                         dot_option,
	                                         play_option };
	std::unordered_map<std::string, std::string> arg_map;

	if (!ValidateArguments(argc, argv, arg_options, arg_map)) {
		std::cout << "Usage: "
		          << std::string(argv[0]) << " <spec-file> "
		          << "[--" << test_option << "=<test-set-file>] "
		          << "[--" << play_option << "=<input-play-file>] "
		          << "[--" << dot_option << "=<output-dot-file>] "
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

		if (!arg_map.empty()) {
			std::vector<CUDD::BDD> bdds = {
				strategy->WinningStates(),
				strategy->CycleCoveringStates(),
				strategy->ReachingMoves()
			};

			auto test_file = arg_map.find(test_option);
			auto play_file = arg_map.find(play_option);
			auto dot_file = arg_map.find(dot_option);

			if (test_file != arg_map.end()) {
				RunTests(bdds, test_file->second, vars);
			}
			
			if (dot_file != arg_map.end()) {
				vars->DumpDot(bdds, { "FG(CC)", "CC", "T" }, dot_file->second);
			}

			if (play_file != arg_map.end()) {
				PlayGame(spec, *strategy, play_file->second, mgr, vars);
			}
		}
	} else {
		std::cout << "Unrealizable" << std::endl;
	}

	return 0;
}
