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
	SEP				";"
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
%nterm <CUDD::BDD> formula
%nterm <CUDD::BDD> formulas
%nterm <CUDD::BDD> in_formulas
%nterm <CUDD::BDD> out_formulas
%nterm <CUDD::BDD> and_formulas
%nterm <CUDD::BDD> or_formulas
%nterm <std::vector<SGrk::SeparationGrkImplication>> justice_implications
%nterm <SGrk::SeparationGrkImplication> justice_implication
%nterm <std::vector<CUDD::BDD>> justices
%nterm <std::vector<CUDD::BDD>> in_justices
%nterm <std::vector<CUDD::BDD>> out_justices
%nterm <CUDD::BDD> justice
%nterm <SGrk::SeparationGrkSpec> sgrk_spec

%printer { yyoutput << "PLACEHOLDER"; } <*>;

%%

%start sgrk_spec;
sgrk_spec: in_formulas SEP out_formulas SEP in_formulas SEP out_formulas SEP justice_implications
{
	driver.spec = SGrk::SeparationGrkSpec($1, $3, $5, $7, std::move($9));
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

in_formulas: formulas { $$ = $1; };

out_formulas: formulas { $$ = $1; };

formulas: formula { $$ = $1; }
| formula AND formulas { $$ = $1 & $3; };

formula: FALSE { $$ = driver.mgr->bddZero(); }
| TRUE { $$ = driver.mgr->bddOne(); }
| prop { $$ = $1->operator()(); }
| NEXT prop {	$$ = $2->Prime(); }
| LEFT formula RIGHT { $$ = $2; }
| NOT formula {	$$ = !$2; }
| LEFT formula and_formulas RIGHT {	$$ = $2 & $3; }
| LEFT formula or_formulas RIGHT { $$ = $2 | $3; }
| LEFT formula IFTHEN formula RIGHT {	$$ = !$2 | $4; }
| LEFT formula IFF formula RIGHT { $$ = $2.Xnor($4); }
| LEFT formula XOR formula RIGHT { $$ = $2.Xnor(!$4); };

and_formulas: AND formula { $$ = $2; }
| AND formula and_formulas { $$ = $2 & $3; };

or_formulas: OR formula { $$ = $2; }
| OR formula or_formulas {$$ = $2 | $3; };

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

justice: INFINITE formula { $$ = $2; };

%%

void yy::parser::error(const location_type& l, const std::string& m) {
	std::cerr << l << ": " << m << std::endl;
}
