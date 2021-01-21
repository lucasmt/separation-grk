%skeleton "lalr1.cc"
%require "3.0.4"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
#include <vector>
	
#include "cuddObj.hh"
	
#include "SeparationGrkSpec.h"
#include "VarMgr.h"

class Driver;
}

%param { Driver& driver }

%locations

%define parse.trace
%define parse.error verbose

%code {
#include "Driver.h"
}

%define api.token.prefix {TOK_}
%token
	LEFT			"("
	RIGHT			")"
	FALSE			"0"
	TRUE			"1"
	NOT				"!"
	AND				"&"
	OR				"|"
	IFTHEN		"->"
	IFF				"<->"
	XOR				"^"
	NEXT			"X"
	ALWAYS		"G"
	INFINITE	"GF"
;

%token <std::string> INPROP "input proposition"
%token <std::string> OUTPROP "output proposition"
%token END 0 "end of file"

%nterm <std::optional<SGrk::Var>> prop
%nterm <CUDD::BDD> state
%nterm <CUDD::BDD> in_state
%nterm <CUDD::BDD> out_state
%nterm <CUDD::BDD> and_states
%nterm <CUDD::BDD> or_states
%nterm <CUDD::BDD> trans
%nterm <CUDD::BDD> in_trans
%nterm <CUDD::BDD> out_trans
%nterm <std::vector<SGrk::SeparationGrkImplication>> justice_implications
%nterm <SGrk::SeparationGrkImplication> justice_implication
%nterm <std::vector<CUDD::BDD>> justices
%nterm <std::vector<CUDD::BDD>> in_justices
%nterm <std::vector<CUDD::BDD>> out_justices
%nterm <CUDD::BDD> justice
%nterm <SGrk::SeparationGrkSpec> sgrk_formula

%printer { yyoutput << "PLACEHOLDER"; } <*>;

%%

%start sgrk_formula;
sgrk_formula: LEFT in_state AND in_trans RIGHT IFTHEN LEFT out_state AND out_trans AND justice_implications RIGHT
{
	driver.spec = SGrk::SeparationGrkSpec($2, $8, $4, $10, std::move($12));
};

prop: INPROP
{
	std::string var_name = $1.substr(1, $1.size() - 2);

	std::optional<SGrk::Var> maybe_var = driver.vars->FindVar(var_name);

	SGrk::Var var = maybe_var ?
			*maybe_var :
			driver.vars->NewVar(SGrk::VarType::INPUT, var_name);

	$$ = std::move(var);
}
| OUTPROP
{
	std::string var_name = $1.substr(1, $1.size() - 2);

	std::optional<SGrk::Var> maybe_var = driver.vars->FindVar(var_name);

	SGrk::Var var = maybe_var ?
			*maybe_var :
			driver.vars->NewVar(SGrk::VarType::OUTPUT, var_name);

	$$ = std::move(var);
}

in_state: state { $$ = $1; };

out_state: state { $$ = $1; };

state: FALSE { $$ = driver.mgr->bddZero(); }
| TRUE { $$ = driver.mgr->bddOne(); }
| prop { $$ = $1->operator()(); }
| NEXT prop {	$$ = $2->Prime(); }
| LEFT state RIGHT { $$ = $2; }
| NOT state {	$$ = !$2; }
| LEFT state and_states RIGHT {	$$ = $2 & $3; }
| LEFT state or_states RIGHT { $$ = $2 | $3; }
| LEFT state IFTHEN state RIGHT {	$$ = !$2 | $4; }
| LEFT state IFF state RIGHT { $$ = $2.Xnor($4); }
| LEFT state XOR state RIGHT { $$ = $2.Xnor(!$4); };

and_states: AND state { $$ = $2; }
| AND state and_states { $$ = $2 & $3; };

or_states: OR state { $$ = $2; }
| OR state or_states {$$ = $2 | $3; };

in_trans: trans { $$ = $1; };

out_trans: trans { $$ = $1; };

trans: ALWAYS state { $$ = $2; };

justice_implications: justice_implication
{
	$$ = std::vector<SGrk::SeparationGrkImplication>({ $1 });
}
| justice_implication AND justice_implications
{
	std::vector<SGrk::SeparationGrkImplication> implications = std::move($3);
	implications.push_back($1);
	
	$$ = std::move(implications);
};

justice_implication: LEFT LEFT in_justices RIGHT IFTHEN LEFT out_justices RIGHT RIGHT
{
	$$ = SGrk::SeparationGrkImplication($3, $7);
};

in_justices: justices { $$ = std::move($1); };

out_justices: justices { $$ = std::move($1); };

justices: justice { $$ = std::vector<CUDD::BDD>({ $1 }); }
| justice AND justices
{
	std::vector<CUDD::BDD> justices = std::move($3);
	justices.push_back($1);

	$$ = std::move(justices);
};

justice: INFINITE state { $$ = $2; };

%%

void yy::parser::error(const location_type& l, const std::string& m) {
	std::cerr << l << ": " << m << std::endl;
}
