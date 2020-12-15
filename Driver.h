#ifndef DRIVER_HH
#define DRIVER_HH
#include <string>

#include "cuddObj.hh"

#include "VarMgr.h"

#include "sgrk_parser.hpp"

// Tell Flex the lexer's prototype ...
#define YY_DECL  yy::parser::symbol_type yylex (Driver& driver)
// ... and declare it for the parser's sake.
YY_DECL;

// Conducting the whole scanning and parsing of Calc++.
class Driver {
public:
	Driver(std::shared_ptr<CUDD::Cudd> m, std::shared_ptr<SGrk::VarMgr> v);

  std::shared_ptr<CUDD::Cudd> mgr;
  std::shared_ptr<SGrk::VarMgr> vars;

  SGrk::SeparationGrkSpec spec;

  // Run the parser on file F.  Return 0 on success.
  int Parse(const std::string& f);
  // The name of the file being parsed.
  std::string filename;
  // Whether to generate parser debug traces.
  bool trace_parsing;

  // Handling the scanner.
  void BeginScan();
  void EndScan();
  // Whether to generate scanner debug traces.
  bool trace_scanning;
  // The token's location used by the scanner.
  yy::location location;
};
#endif // ! DRIVER_HH
