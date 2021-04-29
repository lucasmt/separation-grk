#include "Driver.h"

#include <cstring>

extern FILE* yyin;
extern int yy_flex_debug;

Driver::Driver(std::shared_ptr<CUDD::Cudd> m, std::shared_ptr<SGrk::VarMgr> v)
  : trace_parsing(false), trace_scanning(false)
  , mgr(std::move(m)), vars(std::move(v)) {}

int Driver::Parse(const std::string &f) {
	filename = f;
  location.initialize(&filename);
  BeginScan();
  yy::parser parser(*this);
  parser.set_debug_level(trace_parsing);
  int result = parser.parse();
  EndScan();
  
  return result;
}

void Driver::BeginScan() {
  yy_flex_debug = trace_scanning;
  
  if (filename.empty() || filename == "-") {
    yyin = stdin;
  } else if (!(yyin = fopen(filename.c_str(), "r"))) {
	  std::cerr << "Cannot open " << filename << ": " << strerror(errno)
	            << std::endl;
	  exit(EXIT_FAILURE);
  }
}

void Driver::EndScan() {
  fclose(yyin);
}

