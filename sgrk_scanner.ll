%{
#include "Driver.h"
#include "sgrk_parser.hpp"

#undef yywrap
#define yywrap() 1
%}

%option noyywrap nounput noinput batch debug

id    [a-zA-Z_0-9]+
blank [ \t]

%{
  // Code run each time a pattern is matched.
  #define YY_USER_ACTION loc.columns(yyleng);
%}
%%
%{
  // A handy shortcut to the location held by the driver.
  yy::location& loc = driver.location;
  // Code run each time yylex is called.
  loc.step();
%}

{blank}+   loc.step();
[\n]+      loc.lines(yyleng); loc.step();

"("                 { return yy::parser::make_LEFT(loc); }
")"                 { return yy::parser::make_RIGHT(loc); }
\"in:{id}\"         { return yy::parser::make_INPROP(yytext, loc); }
\"out:{id}\"        { return yy::parser::make_OUTPROP(yytext, loc); }
0                   { return yy::parser::make_FALSE(loc); }
1                   { return yy::parser::make_TRUE(loc); }
"!"                 { return yy::parser::make_NOT(loc); }
"&"                 { return yy::parser::make_AND(loc); }
"|"                 { return yy::parser::make_OR(loc); }
"->"                { return yy::parser::make_IFTHEN(loc); }
"<->"               { return yy::parser::make_IFF(loc); }
"^"                 { return yy::parser::make_XOR(loc); }
X                   { return yy::parser::make_NEXT(loc); }
G                   { return yy::parser::make_ALWAYS(loc); }
GF                  { return yy::parser::make_INFINITE(loc); }
<<EOF>>             { return yy::parser::make_END(loc); }

%%
