/* Identifies DIEGO tokens */
%{
	#include <stdio.h>
	#include "y.tab.h"
	#define ERR_LABEL_SIZE 42
	extern int lineno;
	int commentcalls = 0;
%}

%option noyywrap nounput noinput
%option never-interactive
%x MULCOMMENT

DIGITS [0-9]+
ID	["_"a-zA-Z]["_"A-Za-z0-9]*
SINGLECOMMENT "#"[^\n]* 
WHITESPACE [\ \t]*

%%
\n 		{ lineno++; }
{WHITESPACE} 	/*ignore*/;

"(*"		{ commentcalls++; BEGIN(MULCOMMENT); }

<MULCOMMENT>\n	{ lineno++; }

<MULCOMMENT>. 	/*ignore*/;

<MULCOMMENT><<EOF>>		{ 
				yy_fatal_error("Error: multi line comment not closed \n"); }

<MULCOMMENT>"(*"	{ commentcalls++; }

<MULCOMMENT>"*)"	{ commentcalls--; if (commentcalls == 0) 
						{ BEGIN(INITIAL); } else { 
							BEGIN(MULCOMMENT); } }

{SINGLECOMMENT}	/*ignore */;

"return"	{ return RETURN_TOK; }
"allocate"	{ return ALLOCATE_TOK; }
"int"	{ return INT_TOK; }
"bool"	{ return BOOL_TOK; }
"if"	{ return IF_TOK; }
"then"	{ return THEN_TOK; }
"else"	{ return ELSE_TOK; }
"while"	{ return WHILE_TOK; }
"break" { return BREAK_TOK; }
"continue" { return CONTINUE_TOK; }
"do"	{ return DO_TOK; }
"write"	{ return WRITE_TOK; }
"null"	{ return NULL_TOK; }
"func"	{ return FUNCTION_TOK; }
"end"	{ return END_FUNCTION_TOK; }
"type"	{ return TYPE_TOK; }
"var"   { return VAR_TOK; }
"false" { return BOOL_FALSE_TOK; }
"true" { return BOOL_TRUE_TOK; }
"of length" { return OF_LENGTH_TOK; }
"array of"	{ return ARRAY_OF_TOK; }
"record of"	{ return RECORD_OF_TOK; }
"for" { return FOR_TOK; }
"for each" { return FOREACH_TOK; }
"in" { return IN_TOK; }

";"	{ return ';'; }
":"	{ return ':'; }
"," { return ','; }
"."	{ return '.'; }
"!" { return '!'; }
")"	{ return ')'; }
"("	{ return '('; }
"{"	{ return '{'; }
"}" { return '}'; }
"[" { return '['; }
"]" { return ']'; }
"|" { return '|'; }

"*"	{ return '*'; } 
"+"	{ return '+'; }
"-"	{ return '-'; }
"/"	{ return '/'; }
"%" { return '%'; }
"="	{ return '='; } 

"+="	{ return ADD_ASSIGN_TOK; }
"-="	{ return SUB_ASSIGN_TOK; }
"*=" 	{ return MUL_ASSIGN_TOK; }
"/="	{ return DIV_ASSIGN_TOK; }
"%="	{ return MOD_ASSIGN_TOK; }

"&&"	{ return BOOL_AND_TOK; } 
"=="	{ return BOOL_EQUAL_TOK; }
"!="	{ return BOOL_NOT_EQUAL_TOK; }
"<="	{ return BOOL_LESS_EQ_TOK; }
">="  { return BOOL_GREATER_EQ_TOK; }
"<"	{ return '<'; }
">"	{ return '>'; }

{DIGITS}	{ yylval.int_val = atoi(yytext); return NUM; }
{ID}		{ yylval.str_val = malloc(strlen(yytext)+1 * sizeof(char) );
				sprintf(yylval.str_val,"%s",yytext); return ID; }

. 			{ char *errorMsg = malloc(ERR_LABEL_SIZE * sizeof(char) );
		sprintf(errorMsg,"Error: invalid symbols found at line %i ",lineno);
			yy_fatal_error(errorMsg);}
%%
