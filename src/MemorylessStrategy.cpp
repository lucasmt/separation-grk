#include "MemorylessStrategy.h"

#include <cassert>
#include <utility>

#include "Util.h"

namespace SGrk {

MemorylessStrategy::MemorylessStrategy(
    std::shared_ptr<CUDD::Cudd> mgr,
    std::shared_ptr<VarMgr> vars,
    CUDD::BDD realizable_region,
    std::unordered_map<int, CUDD::BDD> strategy)
	: mgr_(std::move(mgr))
	, vars_(std::move(vars))
	, realizable_region_(std::move(realizable_region))
	, strategy_(std::move(strategy)) {}
	
MemorylessStrategy MemorylessStrategy::Determinize(
    std::shared_ptr<CUDD::Cudd> mgr,
    std::shared_ptr<VarMgr> vars,
    const CUDD::BDD& winning_moves) {
	std::vector<CUDD::BDD> parameterized_output_function;
  int* output_indices;
  CUDD::BDD output_cube = vars->PrimedOutputs();
  std::size_t output_count = output_cube.SupportSize();

  // Need to negate the BDD because b.SolveEqn(...) solves the equation b = 0
  CUDD::BDD consistency_condition =
	  (!winning_moves).SolveEqn(output_cube, parameterized_output_function,
	                            &output_indices, output_count);

  // Copy the index since it will be necessary in the last step
  std::vector<int> index_copy(output_count);

  for (std::size_t i = 0; i < output_count; ++i) {
    index_copy[i] = output_indices[i];
  }

  // Verify that the solution is correct, also frees output_index
  CUDD::BDD verified = (!winning_moves).VerifySol(parameterized_output_function,
                                                  output_indices);

  assert(consistency_condition == verified);

  std::unordered_map<int, CUDD::BDD> output_function;
  
  // Let y_i be the i-th output variable in the BDD ordering.
  // The parameterized output function for y_i is of the form
  // f_i(x_1, ..., x_m, p_i, ..., p_n) where x_1, ..., x_m are the state and
  // input variables and p_i, ..., p_n are parameters respectively taking the
  // place of y_i, ..., y_n. All f_i are such that no matter what we replace
  // parameters p_i, ..., p_n with, the result is a valid output function. We
  // replace the parameters with 1 so that all f_i are dependent only on the
  // input and state variables.
  for (int i = output_count - 1; i >= 0; --i) {
	  int output_index = index_copy[i];

	  output_function[output_index] = parameterized_output_function[i];

	  // TODO(Lucas): Replace inner loop with CUDD::BDD::VectorCompose
	  for (int j = output_count - 1; j >= i; --j) {
		  int parameter_index = index_copy[j];

		  // Can be anything, set to the constant 0 for simplicity
		  CUDD::BDD parameter_value = mgr->bddZero();

		  output_function[output_index] = output_function[output_index].Compose(
			    parameter_value, parameter_index);
	  }
  }

  // Consistency condition is the complement of the realizable region
  return MemorylessStrategy(std::move(mgr), std::move(vars),
                            !consistency_condition, std::move(output_function));
}

CUDD::BDD MemorylessStrategy::RealizableRegion() const {
	return realizable_region_;
}
	
void MemorylessStrategy::Update(std::vector<int>& transition_vector) const {
	for (const std::pair<int, CUDD::BDD>& index_and_function : strategy_) {
		int index = index_and_function.first;
		CUDD::BDD function = index_and_function.second;

		transition_vector[index] = Util::EvalBdd(function, transition_vector);
	}
}

}
