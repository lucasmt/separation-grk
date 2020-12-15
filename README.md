# Separation GR(k)

## Installation

This project requires the 3.0.0 version of the CUDD BDD package. It additionally assumes that the CUDD C++ API is wrapped in a namespace called ``CUDD``. A version of the code with the namespace can be found here: https://github.com/KavrakiLab/cudd.git. Once CUDD has been compiled, replace the ``CUDD=../cudd-install`` line in the ``Makefile`` with the path to the installation directory, which should have an ``include`` folder with the file ``cuddObj.hh`` file and a ``lib`` folder with the file ``libcudd.a``.

The project also requires a working installation of ``flex`` and ``bison``, and a version of ``g++`` with support for C++17. To compile, simply run ``make`` from the top-level directory.

## Specification format

An example of specification file can be found in the ``benchmarks`` subdirectory, under the name ``cleaning_robots.sgrk``:

```
(in:room0 & !in:room1 & !in:clean0 & !in:clean1) &
(out:room0 & !out:room1 & !out:clean0 & !out:clean1) &
G((in:room0 </> in:room1) & (X in:room0 </> X in:room1) &
  (in:room1 -> X in:room1) &
  ((!in:clean0 & X in:clean0) -> (in:room0 & X in:room0)) &
  ((!in:clean1 & X in:clean1) -> (in:room1 & X in:room1)) &
  (in:clean0 -> X in:clean0) &
  (in:clean1 -> X in:clean1)) &
G((out:room0 </> out:room1) & (X out:room0 </> X out:room1) &
  (out:room1 -> X out:room1) &
  ((!out:clean0 & X out:clean0) -> (out:room0 & X out:room0)) &
  ((!out:clean1 & X out:clean1) -> (out:room1 & X out:room1)) &
  (out:clean0 -> X out:clean0) &
  (out:clean1 -> X out:clean1)) &
(GF in:room1 & GF !in:clean0 -> GF out:clean0) &
(GF in:room1 & GF !in:clean1 -> GF out:clean1)
```

Specification files must be in the format

```
<initial-assumptions> &
<initial-guarantees> &
G <safety-assumptions> &
G <safety-guarantees> &
(GF <justice-assumption> & ... & GF <justice-assumption> -> GF <justice-guarantee> & ... & GF <justice-guarantee>) &
... &
(GF <justice-assumption> & ... & GF <justice-assumption> -> GF <justice-guarantee> & ... & GF <justice-guarantee>)
```

where assumptions are boolean formulas over the input variables and guarantees are boolean formulas over the output variables. Input variables must be prefixed by ``in:`` and output variables by ``out:``, and their names can use any alphanumeric character or underscores, in any order. Although the tool currently does not check whether the assumptions only contain input variables and the guarantees only contain output variables, if this condition is violated the tool is not guaranteed to produce correct results.

Boolean formulas use the operators ``!`` (not), ``&`` (and), ``|`` (or), ``->`` (implies), ``<->`` (iff) and ``</>`` (xor), and the constants ``0`` and ``1``. In the safety assumptions and guarantees variables can also be preceded by the temporal operator ``X`` (next).

Whitespace and newlines are used for readability, but ignored by the parser.

## Running the tool

To run the tool, use the command

```
./sgrk <specification-file> [<dot-file>]
```

where ``<specification-file>`` is a file in the format above. The tool outputs either ``Realizable`` or ``Unrealizable``. If the optional argument ``<dot-file>`` is provided, then when the specification is realizable it will save a ``.dot`` file with three BDDs:

* The first (labeled ``FG(CC)``) represents the set of winning states; that is, the states from where the system can force the game to remain forever in a strongly-connected component that covers cycles.
* The second (labeled ``CC``) represents the set of states where the system is able to cover cycles: as long as the environment does not move to a different strongly-connected component, the system can guarantee that all implications are satisfied by visiting every state in the current strongly-connected component infinitely often.
* The third (labeled ``T``) represents a transition relation that will take the system from a winning state that does not cover cycles to a winning state that does cover cycles.
