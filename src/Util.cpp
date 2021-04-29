#include "Util.h"

namespace SGrk {

namespace Util {

int EvalBdd(const CUDD::BDD& bdd,
            // Cannot be const because of bdd.Eval
            std::vector<int>& transition_vector) {
	return bdd.Eval(transition_vector.data()).IsOne();
}

int EvalAdd(const CUDD::ADD& add,
            // Cannot be const because of bdd.Eval
            std::vector<int>& transition_vector) {
	return Cudd_V(add.Eval(transition_vector.data()).getNode());
}

}

}
