#ifndef TEST_SET_H
#define TEST_SET_H

#include <memory>
#include <vector>

#include "cuddObj.hh"

#include "VarMgr.h"

namespace SGrk {

enum class TestResult { PASS,	FAIL };

class TestCase {
	std::vector<int> variable_assignment_;
	std::vector<int> expected_result_;

 public:

	TestCase(std::vector<int> variable_assignment,
	         std::vector<int> expected_result);
	
	const std::vector<int>& VariableAssignment() const;
	const std::vector<int>& ExpectedResult() const;
};

class TestSet {
	std::vector<TestCase> test_cases_;

	static int ReadBit(const std::string& line, int i);
	
 public:

	TestSet(std::vector<TestCase> test_cases);

	static TestSet ReadFromFile(const std::string& filename,
	                            const std::shared_ptr<VarMgr>& vars);

	std::vector<std::vector<TestResult>>
		Run(const std::vector<CUDD::BDD>& functions);
};

}

#endif // TEST_SET_H
