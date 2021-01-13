#ifndef UTIL_H
#define UTIL_H

#include "cuddObj.hh"

namespace SGrk {

namespace Util {

int EvalBdd(const CUDD::BDD& bdd, std::vector<int>& transition_vector);
int EvalAdd(const CUDD::ADD& add, std::vector<int>& transition_vector);
 
}

}

#endif // UTIL_H
