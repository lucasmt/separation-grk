#include "TestSet.h"

#include <exception>
#include <fstream>

namespace SGrk {

TestCase::TestCase(std::vector<int> variable_assignment,
                   std::vector<int> expected_result)
	: variable_assignment_(std::move(variable_assignment))
	, expected_result_(std::move(expected_result)) {}
	
const std::vector<int>& TestCase::VariableAssignment() const {
	return variable_assignment_;
}

const std::vector<int>& TestCase::ExpectedResult() const {
	return expected_result_;
}

TestSet::TestSet(std::vector<TestCase> test_cases)
	: test_cases_(std::move(test_cases)) {}

TestSet TestSet::ReadFromFile(const std::string& filename,
                              const std::shared_ptr<VarMgr>& vars) {
	std::ifstream in(filename);

	std::string line;
	std::getline(in, line);

	if (in.eof()) {
		throw std::runtime_error("File is empty: " + filename);
	}

	std::vector<Var> variables;

	while (!line.empty()) {
		std::size_t pos = line.find(" ");

		if (pos == std::string::npos) {
			pos = line.length();
		}

		std::string variable_name = line.substr(0, pos);

		std::optional<Var> variable = vars->FindVar("\"" + variable_name + "\"");

		if (!variable) {
			throw std::runtime_error("Invalid variable name: " + variable_name);
		}
		
		variables.push_back(*variable);

		line.erase(0, pos + 1);
	}

	std::vector<TestCase> test_cases;
	
	while (true) {
		std::getline(in, line);

		if (in.eof()) break;

		std::size_t pos = line.find(" -> ");

		if (pos == std::string::npos) {
			throw std::runtime_error("Expected line in the format \"XXXXXX -> XXX\"");
		}

		std::string assignment_string = line.substr(0, pos);

		if (assignment_string.size() != 2 * variables.size()) {
			throw std::runtime_error("Expected assignment of size " +
			                         std::to_string(2 * variables.size()));
		}

		std::vector<int> variable_assignment = vars->ZeroedTransitionVector();
		
		for (int i = 0; i < variables.size(); ++i) {
			int unprimed_index = variables[i]().NodeReadIndex();
			int primed_index = variables[i].Prime().NodeReadIndex();
			
			variable_assignment[unprimed_index] = ReadBit(line, i);
			variable_assignment[primed_index] = ReadBit(line, variables.size() + i);
		}

		line.erase(0, pos + 4);
		
		std::vector<int> expected_result;

		for (int i = 0; i < line.size(); ++i) {
			expected_result.push_back(ReadBit(line, i));
		}

		test_cases.emplace_back(std::move(variable_assignment),
		                        std::move(expected_result));
	}

	return TestSet(std::move(test_cases));
}

int TestSet::ReadBit(const std::string& line, int i) {
	if (line[i] == '0') {
		return 0;
	} else if (line[i] == '1') {
		return 1;
	} else {
		throw std::runtime_error("Test cases should be composed of only 1s and 0s");
	}
}

std::vector<std::vector<TestResult>>
TestSet::Run(const std::vector<CUDD::BDD>& functions) {
	std::vector<std::vector<TestResult>> test_results;

	for (const TestCase& test_case : test_cases_) {
		const std::vector<int>& expected_result = test_case.ExpectedResult();
		std::vector<TestResult> case_results;

		for (int i = 0; i < functions.size(); ++i) {
			// Making a copy because Eval requires its argument to be non-const
			std::vector<int> assignment = test_case.VariableAssignment();
			
			bool evaluation =
				functions[i].Eval(assignment.data()).IsOne();

			if (evaluation == expected_result[i]) {
				case_results.push_back(TestResult::PASS);
			} else {
				case_results.push_back(TestResult::FAIL);
			}
		}

		test_results.push_back(std::move(case_results));
	}

	return test_results;
}

}
