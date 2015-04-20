
%{
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include "kittytree.h"
	#include "memory.h"
	int yylex ( void );
	void yyerror( char * );
	extern struct BODY *_main_;
	extern int lineno;
%}


%union {
	int int_val;
	char *str_val; 
	struct FUNC *func;
	struct HEAD *head;
	struct TAIL *tail;
	struct PAR_DECL_LIST *par_decl_list;
	struct VAR_DECL_LIST *var_decl_list;
	struct VAR_TYPE *var_type;
	struct BODY *body;
	struct DECL_LIST *decl_list;
	struct DECLARATION *decl;
	struct STATEMENT_LIST *stm_list;
	struct STATEMENT *stm;
	struct OPT_LENGTH *opt_length;
	struct OPT_ELSE *opt_else;
	struct VAR *var;
	struct TYPE *type;
	struct EXPRES *exp;
	struct TERM *term;
	struct ACT_LIST *act_list;
	struct EXP_LIST *exp_list;
}

%token <str_val> ID
%token <int_val> NUM
%token RETURN_TOK 
%token ALLOCATE_TOK 
%token INT_TOK 
%token BOOL_TOK 
%token IF_TOK 
%token THEN_TOK 
%token ELSE_TOK 
%token WHILE_TOK 
%token DO_TOK 
%token WRITE_TOK 
%token VAR_TOK
%token NULL_TOK 
%token BOOL_TRUE_TOK 
%token BOOL_FALSE_TOK 
%token BOOL_OR_TOK 
%token BOOL_EQUAL_TOK 
%token BOOL_NOT_EQUAL_TOK 
%token BOOL_LESS_EQ_TOK 
%token BOOL_GREATER_EQ_TOK 
%token BOOL_AND_TOK  
%token FUNCTION_TOK 
%token END_FUNCTION_TOK 
%token TYPE_TOK 
%token OF_LENGTH_TOK 
%token ARRAY_OF_TOK 
%token RECORD_OF_TOK 

%type <func> func
%type <head> head
%type <tail> tail
%type <type> type
%type <par_decl_list> par_decl_list
%type <var_decl_list> var_decl_list
%type <var_type> var_type
%type <body> body
%type <decl_list> decl_list
%type <decl> decl
%type <stm_list> stm_list
%type <stm> stm
%type <opt_length> opt_length
%type <opt_else> opt_else
%type <var> var
%type <exp> exp
%type <term> term
%type <act_list> act_list
%type <exp_list> exp_list

%left BOOL_AND_TOK BOOL_OR_TOK
%left BOOL_NOT_EQUAL_TOK BOOL_EQUAL_TOK
%left BOOL_GREATER_EQ_TOK BOOL_LESS_EQ_TOK '<' '>'
%left '+' '-'
%left '*' '/'

%nonassoc "no_else" // no binds weaker than else case
%nonassoc ELSE_TOK

%start program

%%

program	: body { _main_ = $1;}
		;

func 	: head body tail { if(strcmp($1->headH.id,$3->id) == 0)
			{$$ = make_FUNC_structure($1,$2,$3);}else{ 
				lineno = $3->lineno;
				yyerror("Function identification mismatch"); 
				} 
			}
		;

head	: FUNCTION_TOK ID '(' par_decl_list ')' ':' type 
			{ $$ = make_HEAD_header($2,$4,$7); } 
		;

tail 	: END_FUNCTION_TOK ID { $$ = make_TAIL_functail($2); }
		;

type 	: ID 	{ $$ = make_TYPE_id($1); }
		| INT_TOK 	{ $$ = make_TYPE_int(); }
		| BOOL_TOK 	{ $$ = make_TYPE_bool(); }
		| ARRAY_OF_TOK type { $$ = make_TYPE_arrayof($2); }
		| RECORD_OF_TOK '{' var_decl_list '}' { $$ = make_TYPE_recordof($3);}
		;

par_decl_list 	: var_decl_list { $$ = make_PAR_DECL_LIST_vardecllist($1);}
				| /*empty*/ { $$ = make_PAR_DECL_LIST_empty();}
				;

var_decl_list	: var_decl_list ',' var_type 
					{ $$ = make_VAR_DECL_LIST_compound($1,$3);}
				| var_type { $$ = make_VAR_DECL_LIST_vartype($1);}
				;

var_type		: ID ':' type { $$ = make_VAR_TYPE_id($1,$3);}
				;

body			: decl_list stm_list {  $$ = make_BODY_funcbody($1,$2);}
				;

decl_list 		: decl_list decl { $$ = make_DECL_LIST_compound($1,$2);}
				| /* empty */ { $$ = make_DECL_LIST_empty();}
				;

decl 			: TYPE_TOK ID '=' type ';' 
					{ $$ = make_DECLARATION_typeid($2,$4);}
				| func {$$ = make_DECLARATION_func($1);}
				| VAR_TOK var_decl_list ';' {$$ = make_DECLARATION_var($2);}
				;

act_list 	: exp_list { $$ = make_ACT_LIST_explist($1); }
			| /* empty */ { $$ = make_ACT_LIST_epsilon(); }
			;

var 	: ID { $$ = make_VAR_id($1); }
		| var '[' exp ']' { $$ = make_VAR_indexing($1,$3); }
		| var '.' ID { $$ = make_VAR_dot($1,$3); }
		;

term	: var { $$ = make_TERM_var($1); }
		| ID '(' act_list ')' { $$ = make_TERM_actlist($1,$3); }
		| '(' exp ')' { $$ = make_TERM_parenteseexp($2); }
		| '!' term { $$ = make_TERM_bangterm($2); }
		| '|' exp '|' {$$ = make_TERM_pipeexp($2); }
		| NUM { $$ = make_TERM_num($1); }
		| NULL_TOK { $$ = make_TERM_null(); }
		| BOOL_TRUE_TOK { $$ = make_TERM_true(); }
		| BOOL_FALSE_TOK { $$ = make_TERM_false(); }
		;

exp_list 	: exp {$$ = make_EXP_LIST_expression($1); }
			| exp_list ',' exp  { $$ = make_EXP_LIST_commalist($1,$3); }
			;

exp		: exp '+' exp {$$ = make_EXPRESS_plus($1,$3);}
		| exp '-' exp {$$ = make_EXPRESS_minus($1,$3);}
		| exp '*' exp {$$ = make_EXPRESS_times($1,$3);}
		| exp '/' exp {$$ = make_EXPRESS_divide($1,$3);}
		| exp '<' exp {$$ = make_EXPRESS_boolless($1,$3); }
		| exp '>' exp {$$ = make_EXPRESS_boolgreater($1,$3);}
		| exp BOOL_AND_TOK exp {$$ = make_EXPRESS_booland($1,$3);}
		| exp BOOL_OR_TOK exp {$$ = make_EXPRESS_boolor($1,$3);}
		| exp BOOL_LESS_EQ_TOK exp {$$ = make_EXPRESS_boolleq($1,$3);}
		| exp BOOL_GREATER_EQ_TOK exp {$$ = make_EXPRESS_boolgeq($1,$3);}
		| exp BOOL_NOT_EQUAL_TOK exp {$$ = make_EXPRESS_boolneq($1,$3);}
		| exp BOOL_EQUAL_TOK exp {$$ = make_EXPRESS_booleq($1,$3);}
		| term {$$ = make_EXPRESS_term($1);}
		;

opt_else	: ELSE_TOK stm {$$ = make_OPT_ELSE_elsestatement($2);}
			| /* empty */ %prec "no_else" {$$ = make_OPT_ELSE_empty();} 
			;

opt_length	: OF_LENGTH_TOK exp {$$ = make_OPT_LENGTH_oflength($2);}
			| /* empty */ {$$ = make_OPT_LENGTH_empty();}
			;

stm 	: RETURN_TOK exp ';' { $$ = make_STATEMENT_return($2); }
	| WRITE_TOK exp ';' {$$ = make_STATEMENT_write($2);}
	| ALLOCATE_TOK var opt_length ';' {$$ = make_STATEMENT_allocate($2,$3);}
	| var '=' exp ';' {$$ = make_STATEMENT_assignment($1,$3);}
	| IF_TOK exp THEN_TOK stm opt_else 
			{$$ = make_STATEMENT_ifbranch($2,$4,$5);}
	| WHILE_TOK exp DO_TOK stm {$$ = make_STATEMENT_while($2,$4);}
	| '{' stm_list '}' {$$ = make_STATEMENT_compound($2);}
	;

stm_list	: stm { $$ = make_STATEMENT_LIST_statement($1); }
	   		| stm_list stm { $$ = make_STATEMENT_LIST_compound($1,$2); }
	   		;

%%

/*
 * Taking care of errors in parsing
 */
void yyerror(char *msg){
    fprintf(stderr,"Error at line %i: %s \n", lineno ,msg); 
}
