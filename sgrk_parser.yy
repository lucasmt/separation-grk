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
	XOR				"</>"
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
sgrk_formula: in_state AND out_state AND in_trans AND out_trans AND justice_implications
{
	driver.spec = SGrk::SeparationGrkSpec($1, $3, $5, $7, std::move($9));
};

prop: INPROP
{
	std::optional<SGrk::Var> maybe_var = driver.vars->FindVar($1);

	SGrk::Var var = maybe_var ?
			*maybe_var :
			driver.vars->NewVar(SGrk::VarType::INPUT, $1);

	$$ = std::move(var);
}
| OUTPROP
{
	std::optional<SGrk::Var> maybe_var = driver.vars->FindVar($1);

	SGrk::Var var = maybe_var ?
			*maybe_var :
			driver.vars->NewVar(SGrk::VarType::OUTPUT, $1);

	$$ = std::move(var);
}

in_state: state { $$ = $1; };

out_state: state { $$ = $1; };

state: FALSE { $$ = driver.mgr->bddZero(); }
| TRUE { $$ = driver.mgr->bddOne(); }
| prop { $$ = $1->operator()(); }
| NEXT prop {	$$ = $2->Prime(); }
| LEFT state RIGHT { $$ = $2; } | NOT state {	$$ = !$2; }
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

justice_implication: LEFT in_justices IFTHEN out_justices RIGHT
{
	$$ = SGrk::SeparationGrkImplication($2, $4);
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