# Separation GR(k)

## Installation

This project requires the 3.0.0 version of the CUDD BDD package. It additionally assumes that the CUDD C++ API is wrapped in a namespace called ``CUDD``. A version of the code with the namespace can be found here: https://github.com/KavrakiLab/cudd.git. Once CUDD has been compiled, replace the ``CUDD=../cudd-install`` line in the ``Makefile`` with the path to the installation directory, which should have an ``include`` folder with the file ``cuddObj.hh`` file and a ``lib`` folder with the file ``libcudd.a``.

The project also requires a working installation of ``flex`` and ``bison``, and a version of ``g++`` with support for C++17. To compile, simply run ``make`` from the top-level directory.

## Specification format

Examples of specification files can be found in the ``benchmarks`` subdirectory. The following example can be found in ``cleaning_robots_2.sgrk``:

```
(("in:room0" & !"in:clean0" & !"in:done") &
 G (("in:room0") &
   (X "in:room0") &
   ("in:room0" -> X "in:room0") &
   ((!"in:clean0" & X "in:clean0") -> ("in:room0" & X "in:room0")) &
   ("in:clean0" -> X "in:clean0") &
   ("in:done" -> X "in:done") &
   (X "in:done" -> (X "in:room0" <-> "in:room0")) &
   (X "in:done" -> (X "in:clean0" <-> "in:clean0")))) ->
(("out:room0" & !"out:clean0") &
 G (("out:room0") &
    (X "out:room0") &
    ("out:room0" -> X "out:room0") &
    ((!"out:clean0" & X "out:clean0") -> ("out:room0" & X "out:room0")) &
    ("out:clean0" -> X "out:clean0")) &
 ((GF "in:done" & GF !"in:clean0") -> (GF "out:clean0")) &
 ((GF "in:done" & GF "in:clean0") -> (GF !"out:clean0")))
```

Specification files must be in the format

```
(<initial-assumptions> &
 G <safety-assumptions>) ->
(<initial-guarantees> &
 G <safety-guarantees> &
 ((GF <justice-assumption> & ... & GF <justice-assumption>) -> (GF <justice-guarantee> & ... & GF <justice-guarantee>)) &
... &
 ((GF <justice-assumption> & ... & GF <justice-assumption>) -> (GF <justice-guarantee> & ... & GF <justice-guarantee>)))
```

where assumptions are boolean formulas over the input variables and guarantees are boolean formulas over the output variables. All variables must be in quotation marks. Input variables must be prefixed by ``in:`` and output variables by ``out:``, and their names can use any alphanumeric character or underscores, in any order. Although the tool currently does not check whether the assumptions only contain input variables and the guarantees only contain output variables, if this condition is violated the tool is not guaranteed to produce correct results.

Boolean formulas use the operators ``!`` (not), ``&`` (and), ``|`` (or), ``->`` (implies), ``<->`` (iff) and ``^`` (xor), and the constants ``0`` and ``1``. In the safety assumptions and guarantees variables can also be preceded by the temporal operator ``X`` (next).

Whitespace and newlines are used for readability, but ignored by the parser.

## Running the tool

To run the tool, use the command

```
./sgrk <specification-file> [--test=<test-set-file>] [--play=<input-play-file>] [--dumpdot=<output-dot-file>]
```

where ``<specification-file>`` is a file in the format above. The tool outputs either ``Realizable`` or ``Unrealizable``. If the specification is realizable, the optional arguments indicate what to do with the winning strategy. We describe the options below.

### Dump dot

If the optional argument ``--dumpdot=<output-dot-file>`` is provided, then it will save a ``.dot`` file with three BDDs:

* The first (labeled ``FG(CC)``) represents the set of winning states; that is, the states from where the system can force the game to remain forever in a strongly-connected component that covers cycles.
* The second (labeled ``CC``) represents the set of states where the system is able to cover cycles: as long as the environment does not move to a different strongly-connected component, the system has a strategy for guaranteeing that all implications are satisfied without leaving the current strongly-connected component.
* The third (labeled ``T``) represents a transition relation that will take the system from a winning state that does not cover cycles to a winning state that does cover cycles.

### Test

If the optional argument ``--test=<test-set-file>`` is provided, the tool reads the input ``<test-set-file>``. The following example test file for the ``cleaning_robots_2.sgrk`` benchmark can be found in ``benchmarks/cleaning_robots_2.test``:

```
in:room0 in:room1 in:clean0 in:clean1 in:done out:room0 out:room1 out:clean0 out:clean1
010011000010011010 -> 101
010011000010010100 -> 100
100011000111111111 -> 100
100001000111111111 -> 110
011111000111111111 -> 110
010111010111111111 -> 110
010000100111111111 -> 010
010010100111111111 -> 000
100000100111111111 -> 010
100010100111111111 -> 000
```

The first line of the test file must contain all of the input and output variables present in the benchmark, in any order. The following lines contain test cases.

On the left of the arrow should be a bitvector of size ``2n``, where ``n`` is the total number of variables. The first ``n`` bits represent an assignment to the variables in the order given in the first line, and the last ``n`` bits represent an assignment to the variables in the next step, in the same order.

On the right side of the arrow should be a bitvector of size 3, denoting the expected evaluation of the BDDs ``FG(CC)``, ``CC`` and ``T`` described above, in that order. Since ``FG(CC)`` and ``CC`` represent sets of states, they only consider the first ``n`` bits, representing the current state. Meanwhile, since ``T`` represents a transition relation, it considers all ``2n`` bits.

The tool outputs the test results in the following format:

```
0: PASS PASS PASS
1: PASS PASS PASS
2: PASS PASS PASS
3: PASS PASS PASS
4: PASS PASS PASS
5: PASS PASS PASS
6: PASS PASS PASS
7: PASS PASS PASS
8: PASS PASS PASS
9: PASS PASS PASS
```

Each line corresponds to one test case, and displays ``PASS`` or ``FAIL`` for each of the three bits, depending on whether the evaluation matched the one in the test file.

### Play

If the optional argument ``--play=<input-play-file>`` is provided, the tool can play the game with the user, using the winning strategy to choose outputs based on the inputs received.

If ``<input-play-file>`` is given as ``stdin``, the tool requests inputs via the terminal. Inputs are given as bitvectors with one bit per input variable, in the order requested by the tool. Based on these inputs, the tool chooses the output and updates the state. The interaction can be ended by typing ``quit``. If a file name is passed as ``<input-play-file>``, the file should contain one input bitvector per line and end with ``quit``. The tool will then run the game automatically using the inputs from the file.