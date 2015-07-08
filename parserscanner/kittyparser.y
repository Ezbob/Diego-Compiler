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

%nonassoc '|'
%left BOOL_AND_TOK
%left BOOL_NOT_EQUAL_TOK BOOL_EQUAL_TOK
%left BOOL_GREATER_EQ_TOK BOOL_LESS_EQ_TOK '<' '>'
%left '+' '-'
%left '*' '/'
%left "unary_minus"

%nonassoc "no_else" // no binds weaker than else case
%nonassoc ELSE_TOK

%start program

%%

program			: body { _main_ = $1; }
				;

func 			: head body tail { if(strcmp($1->id,$3->id) == 0)
					{$$ = make_FUNC($1,$2,$3);}else{ 
						lineno = $3->lineno;
						yyerror("Function identification mismatch"); 
					} 
				}
				;

head			: FUNCTION_TOK ID '(' par_decl_list ')' ':' type 
					{ $$ = make_HEAD($2,$4,$7); } 
				;

tail 			: END_FUNCTION_TOK ID { $$ = make_TAIL($2); }
				;

type 			: ID 	{ $$ = make_TYPE_ID($1); }
				| INT_TOK 	{ $$ = make_TYPE_INT(); }
				| BOOL_TOK 	{ $$ = make_TYPE_BOOL(); }
				| ARRAY_OF_TOK type { $$ = make_TYPE_ARRAY($2); }
				| RECORD_OF_TOK '{' var_decl_list '}' { 
							$$ = make_TYPE_RECORD($3);}
				;

par_decl_list 	: var_decl_list { $$ = make_PAR_DECL_LIST_LIST($1);}
				| /*empty*/ { $$ = make_PAR_DECL_LIST_EMPTY();}
				;

var_decl_list	: var_decl_list ',' var_type 
					{ $$ = make_VAR_DECL_LIST_LIST($1,$3);}
				| var_type { $$ = make_VAR_DECL_LIST_TYPE($1);}
				;

var_type		: ID ':' type { $$ = make_VAR_TYPE($1,$3);}
				;

body			: decl_list stm_list {  $$ = make_BODY($1,$2);}
				;

decl_list 		: decl_list decl { $$ = make_DECL_LIST_LIST($1,$2);}
				| /* empty */ { $$ = make_DECL_LIST_EMPTY();}
				;

decl 			: TYPE_TOK ID '=' type ';' 

					{ $$ = make_DECLARATION_ID($2,$4);}
				| func {$$ = make_DECLARATION_FUNC($1);}
				| VAR_TOK var_decl_list ';' {$$ = make_DECLARATION_VAR($2);}
				;

act_list 		: exp_list { $$ = make_ACT_LIST_EXPLIST($1); }
				| /* empty */ { $$ = make_ACT_LIST_EMPTY(); }
				;

var 			: ID { $$ = make_VAR_ID($1); }
				| var '[' exp ']' { $$ = make_VAR_ARRAY($1,$3); }
				| var '.' ID { $$ = make_VAR_RECORD($1,$3); }
				;

term			: var { $$ = make_TERM_VAR($1); }
				| ID '(' act_list ')' { $$ = make_TERM_ACT_LIST($1,$3); }
				| '(' exp ')' { $$ = make_TERM_PARENTESES($2); }
				| '!' term { $$ = make_TERM_NOT($2); }
				| '-' term %prec "unary_minus" {$$ = make_TERM_UMINUS($2); }
				| '|' exp '|' {$$ = make_TERM_ABS($2); }
				| NUM { $$ = make_TERM_NUM($1); }
				| NULL_TOK { $$ = make_TERM_NULL(); }
				| BOOL_TRUE_TOK { $$ = make_TERM_TRUE(); }
				| BOOL_FALSE_TOK { $$ = make_TERM_FALSE(); }
				;

exp_list 		: exp {$$ = make_EXP_LIST_EXP($1); }
				| exp_list ',' exp  { $$ = make_EXP_LIST_LIST($1,$3); }
				;

exp				: exp '+' exp {$$ = make_EXPRES_PLUS($1,$3);}
				| exp '-' exp {$$ = make_EXPRES_MINUS($1,$3);}
				| exp '*' exp {$$ = make_EXPRES_TIMES($1,$3);}
				| exp '/' exp {$$ = make_EXPRES_DIVIDE($1,$3);}
				| exp '<' exp {$$ = make_EXPRES_LESS($1,$3); }
				| exp '>' exp {$$ = make_EXPRES_GREATER($1,$3);}
				| exp BOOL_AND_TOK exp {$$ = make_EXPRES_AND($1,$3);}
				| exp '|''|' exp {$$ = make_EXPRES_OR($1,$4);}
				| exp BOOL_LESS_EQ_TOK exp {$$ = make_EXPRES_LEQ($1,$3);}
				| exp BOOL_GREATER_EQ_TOK exp {$$ = make_EXPRES_GEQ($1,$3);}
				| exp BOOL_NOT_EQUAL_TOK exp {$$ = make_EXPRES_NEQ($1,$3);}
				| exp BOOL_EQUAL_TOK exp {$$ = make_EXPRES_EQ($1,$3);}
				| term {$$ = make_EXPRES_TERM($1);}
				;

opt_else		: ELSE_TOK stm {$$ = make_OPT_ELSE_STATEMENT($2);}
				| /* empty */ %prec "no_else" {$$ = make_OPT_ELSE_EMPTY();} 
				;

opt_length		: OF_LENGTH_TOK exp {$$ = make_OPT_LENGTH_EXPRES($2);}
				| /* empty */ {$$ = make_OPT_LENGTH_EMPTY();}
				;

stm 			: RETURN_TOK exp ';' { $$ = make_STATEMENT_RETURN($2); }
				| WRITE_TOK exp ';' {$$ = make_STATEMENT_WRITE($2);}
				| ALLOCATE_TOK var opt_length ';' 
						{$$ = make_STATEMENT_ALLOCATE($2,$3);}
				| var '=' exp ';' {$$ = make_STATEMENT_ASSIGN($1,$3);}
				| IF_TOK exp THEN_TOK stm opt_else 
						{$$ = make_STATEMENT_IFBRANCH($2,$4,$5);}
				| WHILE_TOK exp DO_TOK stm {$$ = make_STATEMENT_WHILE($2,$4);}
				| '{' stm_list '}' {$$ = make_STATEMENT_LIST($2);}
				;

stm_list		: stm { $$ = make_STATEMENT_LIST_STATEMENT($1); }
	   			| stm_list stm { $$ = make_STATEMENT_LIST_LIST($1,$2); }
	   			;

%%

/*
 * Taking care of errors in parsing
 */
void yyerror(char *msg){
    fprintf(stderr,"Error at line %i: %s \n", lineno ,msg); 
}
