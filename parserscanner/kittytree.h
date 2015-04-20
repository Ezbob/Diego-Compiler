#ifndef __KITTYTREE_H
#define __KITTYTREE_H

#define NEW_SHORT_STR ((char*) Malloc(sizeof(char*)*17))
#include "../symbol/symbol.h"

typedef struct EXPRES {
	int lineno;
	SYMBOLTABLE *symboltable;
	SYMBOLTYPE *symboltype;
	enum {term_E_K, 
				plus_E_K, 
				minus_E_K, 
				times_E_K, 
				divide_E_K, 
				booleq_E_K,
				boolneq_E_K,
				boolgreater_E_K, 
				boolless_E_K,
				boolgeq_E_K,
				boolleq_E_K,
				booland_E_K,
				boolor_E_K 
	} kind;
	union {
		struct { struct EXPRES *left; struct EXPRES *right; } sides;
		struct TERM *term;
	} value;
} EXPRES;

typedef struct TERM {
	int lineno;
	SYMBOLTABLE *symboltable;
	SYMBOLTYPE *symboltype;
	enum {var_T_K, actList_T_K, expresParent_T_K, expresPipes_T_K, 
		termBang_T_K, num_T_K, boolTrue_T_K, boolFalse_T_K, null_T_K } kind;
	union {
		char *nullval;
		int intconst;
		struct { char *id; struct ACT_LIST *actlist; } actlistT;
		struct VAR *var;
		struct EXPRES *exp;
		struct TERM *term; 
	} value;
} TERM;

typedef struct ACT_LIST {
	int lineno;
	SYMBOLTABLE *symboltable;
	enum { explist_AL_K, empty_AL_K } kind;
	union {
		struct EXP_LIST *exp_list;
	} value;
} ACT_LIST;

typedef struct EXP_LIST {
	int lineno;
	SYMBOLTABLE *symboltable;
	enum {exp_EL_K,commalist_EL_K} kind;
	union {
		struct EXPRES *exp;
		struct { struct EXP_LIST *explist; struct EXPRES *exp;} commaEL;
	} value;
} EXP_LIST;

typedef struct FUNC {
	int lineno;
	SYMBOLTABLE *symboltable;
	SYMBOLTYPE *symboltype;
	struct {struct HEAD *head; struct BODY *body; 
		struct TAIL *tail;} functionF;
} FUNC;

typedef struct HEAD {
	int lineno;
	int arguments;
	SYMBOLTABLE *symboltable;
	SYMBOLTYPE *symboltype;
	struct { char *id; 
		     struct PAR_DECL_LIST *pdeclist;
			 struct TYPE *returntype; 
	} headH;
} HEAD;

typedef struct TAIL {
	int lineno;
	SYMBOLTABLE *symboltable;
	char *id;
} TAIL;

typedef struct TYPE {
	int lineno;
	SYMBOLTYPE *symboltype;
	SYMBOLTABLE *symboltable;
	enum {id_TY_K,int_TY_K,bool_TY_K,arrayof_TY_K,recordof_TY_K} kind;
	union {
		char *idconst;
		char *typeconst;
		struct TYPE *type;
		struct VAR_DECL_LIST *var_decl_list;
	} value;
} TYPE;

typedef struct PAR_DECL_LIST {
	int lineno;
	SYMBOLTABLE *symboltable;
	enum { empty_PDL_K, varType_PDL_K } kind;
	union {
		struct VAR_DECL_LIST *var_decl_list;
	} value;
} PAR_DECL_LIST;

typedef struct VAR_DECL_LIST {
	int lineno;
	SYMBOLTABLE *symboltable;
	enum { comma_VDL_K , var_VDL_typeK } kind;
	union {
		struct { struct VAR_DECL_LIST *var_decl_list;
			struct VAR_TYPE *var_type;} commaVDL;
		struct VAR_TYPE *var_type;
	} value;
} VAR_DECL_LIST;

typedef struct VAR_TYPE {
	int lineno;
	SYMBOLTABLE *symboltable;
	SYMBOL *symbol;
	char* id;
	struct TYPE *type;
} VAR_TYPE;

typedef struct BODY {
	int lineno;
	SYMBOLTABLE *symboltable;
	struct DECL_LIST *decl_list;
	struct STATEMENT_LIST *statement_list;
} BODY;

typedef struct DECL_LIST {
	int lineno;
	SYMBOLTABLE *symboltable;
	enum { compound_DL_K, empty_DL_K } kind;
	union {
		struct { struct DECL_LIST *decl_list; 
			struct DECLARATION *declaration; } compoundDL;
	} value;
} DECL_LIST;

typedef struct DECLARATION
{
	int lineno;
	SYMBOLTABLE *symboltable;
	enum { typeassign_D_K, func_D_K, var_D_K } kind;
	union {
		struct { char *id; struct TYPE *type; } typedeclID;
		struct FUNC *function;
		struct VAR_DECL_LIST *var_decl_list;
	} value;
} DECLARATION;

typedef struct STATEMENT_LIST
{
	int lineno;
	SYMBOLTABLE *symboltable;
	enum { statement_SL_K, compound_SL_K } kind;
	union {
		struct {struct STATEMENT *statement;
					  struct STATEMENT_LIST *statement_list; } compoundSL;
		struct STATEMENT *statement;
	} value;
} STATEMENT_LIST;

typedef struct STATEMENT
{
	int lineno;
	SYMBOLTABLE *symboltable;
	int foundReturn;
	struct STATEMENT *next;

	enum { return_S_K, print_S_K, allocate_S_K, assign_S_K, ifbranch_S_K, 
		while_S_K, statement_list_S_K } kind;
	union { 
		struct EXPRES *exp;
		struct STATEMENT_LIST *statement_list;
		struct {struct EXPRES *exp; FUNC *function; } returnS;
		struct {struct VAR *variable; struct EXPRES *exp; } assignS;
		struct {struct VAR *variable; 
						struct OPT_LENGTH *opt_length; } allocateS;
		struct {struct EXPRES *exp; 
					  struct STATEMENT *statement;
					  struct OPT_ELSE *opt_else; } ifbranchS;
		struct {struct EXPRES *exp;
						struct STATEMENT *statement; } whileS;
	} value;
} STATEMENT;

typedef struct OPT_LENGTH
{
	int lineno;
	SYMBOLTABLE *symboltable;
	enum { lengthof_OL_K, empty_OL_K } kind;
	union { 
		struct EXPRES *exp;
	} value;
} OPT_LENGTH;

typedef struct OPT_ELSE
{
	int lineno;
	SYMBOLTABLE *symboltable;
	enum { else_OE_K, empty_OE_K } kind;
	union { 
		struct STATEMENT *statement;
	} value;
} OPT_ELSE;

typedef struct VAR
{
	int lineno;
	SYMBOLTABLE *symboltable;
	SYMBOLTYPE *symboltype;
	enum { id_V_K, indexing_V_K, dot_V_K } kind;
	union { 
		char *id;
		struct {struct VAR *variable; struct EXPRES *exp;} indexingV;
		struct {char *id; struct VAR *variable; } dotV;
	} value;
} VAR;

/* functions */

FUNC *make_FUNC_structure(HEAD*,BODY*,TAIL*);
HEAD *make_HEAD_header(char *,PAR_DECL_LIST*,TYPE*);
TAIL *make_TAIL_functail(char*);

TYPE *make_TYPE_id(char*);
TYPE *make_TYPE_int();
TYPE *make_TYPE_bool();
TYPE *make_TYPE_arrayof(TYPE *);
TYPE *make_TYPE_recordof(VAR_DECL_LIST *);

PAR_DECL_LIST *make_PAR_DECL_LIST_vardecllist(VAR_DECL_LIST *);
PAR_DECL_LIST *make_PAR_DECL_LIST_empty();

VAR_DECL_LIST *make_VAR_DECL_LIST_compound(VAR_DECL_LIST *, VAR_TYPE *);
VAR_DECL_LIST *make_VAR_DECL_LIST_vartype(VAR_TYPE *);

VAR_TYPE *make_VAR_TYPE_id(char *, TYPE *);

BODY *make_BODY_funcbody(DECL_LIST *, STATEMENT_LIST *);

DECL_LIST *make_DECL_LIST_compound(DECL_LIST *, DECLARATION *);
DECL_LIST *make_DECL_LIST_empty();

DECLARATION *make_DECLARATION_typeid(char *,TYPE *);
DECLARATION *make_DECLARATION_func(FUNC*);
DECLARATION *make_DECLARATION_var(VAR_DECL_LIST *);

STATEMENT_LIST *make_STATEMENT_LIST_statement(STATEMENT *);
STATEMENT_LIST *make_STATEMENT_LIST_compound(STATEMENT_LIST *, STATEMENT *);

STATEMENT *make_STATEMENT_return(EXPRES *);
STATEMENT *make_STATEMENT_write(EXPRES *);
STATEMENT *make_STATEMENT_allocate(VAR *, OPT_LENGTH *);
STATEMENT *make_STATEMENT_assignment(VAR *,EXPRES *);
STATEMENT *make_STATEMENT_ifbranch(EXPRES *, STATEMENT *,OPT_ELSE *);
STATEMENT *make_STATEMENT_while(EXPRES *, STATEMENT *);
STATEMENT *make_STATEMENT_compound(STATEMENT_LIST *);

OPT_LENGTH *make_OPT_LENGTH_oflength(EXPRES *);
OPT_LENGTH *make_OPT_LENGTH_empty();

OPT_ELSE *make_OPT_ELSE_elsestatement(STATEMENT *);
OPT_ELSE *make_OPT_ELSE_empty();

VAR *make_VAR_id(char *);
VAR *make_VAR_indexing(VAR *,EXPRES *);
VAR *make_VAR_dot(VAR *, char*);

/*Expression constructor */
EXPRES *make_EXPRESS_term(TERM *);
EXPRES *make_EXPRESS_plus(EXPRES *,EXPRES *);
EXPRES *make_EXPRESS_minus(EXPRES *,EXPRES *);
EXPRES *make_EXPRESS_times(EXPRES *,EXPRES *);
EXPRES *make_EXPRESS_divide(EXPRES *,EXPRES *);
EXPRES *make_EXPRESS_booleq(EXPRES *,EXPRES *);
EXPRES *make_EXPRESS_boolneq(EXPRES *,EXPRES *);
EXPRES *make_EXPRESS_boolgreater(EXPRES *,EXPRES *);
EXPRES *make_EXPRESS_boolless(EXPRES *,EXPRES *);
EXPRES *make_EXPRESS_boolgeq(EXPRES *,EXPRES *);
EXPRES *make_EXPRESS_boolleq(EXPRES *,EXPRES *);
EXPRES *make_EXPRESS_booland(EXPRES *,EXPRES *);
EXPRES *make_EXPRESS_boolor(EXPRES *,EXPRES *);

/*TERM constructor */

TERM *make_TERM_var(VAR *);
TERM *make_TERM_actlist(char *,ACT_LIST *);
TERM *make_TERM_bangterm(TERM *);
TERM *make_TERM_parenteseexp(EXPRES *);
TERM *make_TERM_pipeexp(EXPRES *);
TERM *make_TERM_null();
TERM *make_TERM_true();
TERM *make_TERM_false();
TERM *make_TERM_num(int);

ACT_LIST *make_ACT_LIST_explist(EXP_LIST *);
ACT_LIST *make_ACT_LIST_epsilon();

EXP_LIST *make_EXP_LIST_expression(EXPRES *);
EXP_LIST *make_EXP_LIST_commalist(EXP_LIST *,EXPRES *);


#endif /* END OF __KITTYTREE_H */
