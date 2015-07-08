#ifndef __KITTYTREE_H
#define __KITTYTREE_H

#include "../symbol/symbol.h"

typedef struct EXPRES {
	int lineno;
	SYMBOL_TABLE *symboltable;
	SYMBOL_TYPE *symboltype;
	enum {
		EXPRES_TERM, 
		EXPRES_PLUS, 
		EXPRES_MINUS, 
		EXPRES_TIMES, 
		EXPRES_DIVIDE, 
		EXPRES_EQ,
		EXPRES_NEQ,
		EXPRES_GREATER, 
		EXPRES_LESS,
		EXPRES_GEQ,
		EXPRES_LEQ,
		EXPRES_AND,
		EXPRES_OR 
	} kind;
	union {
		struct { 
			struct EXPRES *left; 
			struct EXPRES *right; 
		} sides; //Unique name due to all have sides except terms
		struct TERM *term;
	} value;
} EXPRES;

typedef struct TERM {
	int lineno;
	SYMBOL_TABLE *symboltable;
	SYMBOL_TYPE *symboltype;
	enum {
		TERM_VAR,
		TERM_ACT_LIST,
		TERM_PARENTESES,
		TERM_UMINUS,
		TERM_ABS,
		TERM_NOT,
		TERM_NUM,
		TERM_TRUE,
		TERM_FALSE,
		TERM_NULL
	} kind;
	union {
		int intconst;
		struct { 
			char *id; 
			struct ACT_LIST *actlist; 
		} term_act_list;
		struct VAR *var;
		struct EXPRES *exp;
		struct TERM *term; 
	} value;
} TERM;

typedef struct ACT_LIST {
	int lineno;
	SYMBOL_TABLE *symboltable;
	enum { 
		ACT_LIST_EXPLIST, 
		ACT_LIST_EMPTY 
	} kind;
	struct EXP_LIST *exp_list;
} ACT_LIST;

typedef struct EXP_LIST {
	int lineno;
	SYMBOL_TABLE *symboltable;
	enum {
		EXP_LIST_EXP,
		EXP_LIST_LIST
	} kind;
	struct EXPRES *exp;
	struct EXP_LIST *explist;
} EXP_LIST;

typedef struct FUNC {
	int lineno;
	SYMBOL_TABLE *symboltable;
	SYMBOL_TYPE *symboltype;
	struct HEAD *head; 
	struct BODY *body; 
	struct TAIL *tail;
} FUNC;

typedef struct HEAD {
	int lineno;
	int arguments;
	SYMBOL_TABLE *symboltable;
	SYMBOL_TYPE *symboltype;
	char *id; 
	struct PAR_DECL_LIST *pdlist;
	struct TYPE *returntype; 
} HEAD;

typedef struct TAIL {
	int lineno;
	SYMBOL_TABLE *symboltable;
	char *id;
} TAIL;

typedef struct TYPE {
	int lineno;
	SYMBOL_TYPE *symboltype;
	SYMBOL_TABLE *symboltable;
	enum {
		TYPE_ID,
		TYPE_INT,
		TYPE_BOOL,
		TYPE_ARRAY,
		TYPE_RECORD
	} kind;
	union {
		char *id;
		struct TYPE *type;
		struct VAR_DECL_LIST *var_decl_list;
	} value;
} TYPE;

typedef struct PAR_DECL_LIST {
	int lineno;
	SYMBOL_TABLE *symboltable;
	enum { 
		PAR_DECL_LIST_EMPTY,
		PAR_DECL_LIST_LIST
	} kind;

	struct VAR_DECL_LIST *var_decl_list;

} PAR_DECL_LIST;

typedef struct VAR_DECL_LIST {
	int lineno;
	SYMBOL_TABLE *symboltable;
	enum { 
		VAR_DECL_LIST_LIST,
		VAR_DECL_LIST_TYPE
	} kind;
	struct VAR_DECL_LIST *var_decl_list;
	struct VAR_TYPE *var_type;

} VAR_DECL_LIST;

typedef struct VAR_TYPE {
	int lineno;
	SYMBOL_TABLE *symboltable;
	SYMBOL *symbol;
	char *id;
	struct TYPE *type;
} VAR_TYPE;

typedef struct BODY {
	int lineno;
	SYMBOL_TABLE *symboltable;
	struct DECL_LIST *decl_list;
	struct STATEMENT_LIST *statement_list;
} BODY;

typedef struct DECL_LIST {
	int lineno;
	SYMBOL_TABLE *symboltable;
	enum { 
		DECL_LIST_LIST,
		DECL_LIST_EMPTY
	} kind;
	struct DECL_LIST *decl_list; 
	struct DECLARATION *declaration;
} DECL_LIST;

typedef struct DECLARATION {
	int lineno;
	SYMBOL_TABLE *symboltable;
	enum { 
		DECLARATION_ID,
		DECLARATION_FUNC,
		DECLARATION_VAR
	} kind;
	union {
		struct { 
			char *id; 
			struct TYPE *type; 
		} declaration_id;

		struct FUNC *function;
		struct VAR_DECL_LIST *var_decl_list;
	} value;
} DECLARATION;

typedef struct STATEMENT_LIST {
	int lineno;
	SYMBOL_TABLE *symboltable;
	enum { 
		STATEMENT_LIST_LIST,
		STATEMENT_LIST_STATEMENT
	} kind;

	struct STATEMENT *statement;
	struct STATEMENT_LIST *statement_list; 

} STATEMENT_LIST;

typedef struct STATEMENT {
	int lineno;
	SYMBOL_TABLE *symboltable;
	int foundReturn;
	struct STATEMENT *next; //??
	enum { 
		STATEMENT_RETURN,
		STATEMENT_WRITE,
		STATEMENT_ALLOCATE,
		STATEMENT_ASSIGN,
		STATEMENT_IFBRANCH,
		STATEMENT_WHILE,
		STATEMENT_LISTS
	} kind;
	union { 
		struct EXPRES *exp;
		struct STATEMENT_LIST *statement_list;
		struct {
			struct EXPRES *exp; 
			FUNC *function; 
		} statement_return;

		struct {
			struct VAR *var; 
			struct EXPRES *exp; 
		} statement_assign;

		struct {
			struct VAR *var; 
			struct OPT_LENGTH *opt_length; 
		} statement_allocate;

		struct {
			struct EXPRES *exp; 
			struct STATEMENT *statement;
			struct OPT_ELSE *opt_else; 
		} statement_ifbranch;

		struct {
			struct EXPRES *exp;
			struct STATEMENT *statement; 
		} statement_while;
	} value;
} STATEMENT;

typedef struct OPT_LENGTH {
	int lineno;
	SYMBOL_TABLE *symboltable;

	enum { 
		OPT_LENGTH_EXPRES,
		OPT_LENGTH_EMPTY
	} kind;
	
	struct EXPRES *exp;

} OPT_LENGTH;

typedef struct OPT_ELSE {
	int lineno;
	SYMBOL_TABLE *symboltable;
	enum { 
		OPT_ELSE_STATEMENT,
		OPT_ELSE_EMPTY
	} kind;

	struct STATEMENT *statement;

} OPT_ELSE;

typedef struct VAR {
	int lineno;
	char *id;
	SYMBOL_TABLE *symboltable;
	SYMBOL_TYPE *symboltype;
	enum { 
		VAR_ID,
		VAR_ARRAY,
		VAR_RECORD
	} kind;
	union { 
		struct {
			struct VAR *var; 
			struct EXPRES *exp;
		} var_array;
		struct {
			char *id; 
			struct VAR *var; 
		} var_record;
	} value;
} VAR;

/* Functions constructors */
FUNC *make_FUNC(HEAD*,BODY*,TAIL*);
HEAD *make_HEAD(char *,PAR_DECL_LIST*,TYPE*);
BODY *make_BODY(DECL_LIST *, STATEMENT_LIST *);
TAIL *make_TAIL(char*);

/* Type constructors */
TYPE *make_TYPE_ID(char*);
TYPE *make_TYPE_INT();
TYPE *make_TYPE_BOOL();
TYPE *make_TYPE_ARRAY(TYPE *);
TYPE *make_TYPE_RECORD(VAR_DECL_LIST *);

/* Parameter declaration list constructors */
PAR_DECL_LIST *make_PAR_DECL_LIST_LIST(VAR_DECL_LIST *);
PAR_DECL_LIST *make_PAR_DECL_LIST_EMPTY();

/* Variable declaration list constructors */
VAR_DECL_LIST *make_VAR_DECL_LIST_LIST(VAR_DECL_LIST *, VAR_TYPE *);
VAR_DECL_LIST *make_VAR_DECL_LIST_TYPE(VAR_TYPE *);

/* Variable type constructor */
VAR_TYPE *make_VAR_TYPE(char *, TYPE *);

/* Declaration list constructors */
DECL_LIST *make_DECL_LIST_LIST(DECL_LIST *, DECLARATION *);
DECL_LIST *make_DECL_LIST_EMPTY();

/* Decaration constructors */
DECLARATION *make_DECLARATION_ID(char *,TYPE *);
DECLARATION *make_DECLARATION_FUNC(FUNC*);
DECLARATION *make_DECLARATION_VAR(VAR_DECL_LIST *);

/* Statement list constructors */
STATEMENT_LIST *make_STATEMENT_LIST_STATEMENT(STATEMENT *);
STATEMENT_LIST *make_STATEMENT_LIST_LIST(STATEMENT_LIST *, STATEMENT *);

/* Statement constructors */
STATEMENT *make_STATEMENT_RETURN(EXPRES *);
STATEMENT *make_STATEMENT_WRITE(EXPRES *);
STATEMENT *make_STATEMENT_ALLOCATE(VAR *, OPT_LENGTH *);
STATEMENT *make_STATEMENT_ASSIGN(VAR *,EXPRES *);
STATEMENT *make_STATEMENT_IFBRANCH(EXPRES *, STATEMENT *,OPT_ELSE *);
STATEMENT *make_STATEMENT_WHILE(EXPRES *, STATEMENT *);
STATEMENT *make_STATEMENT_LIST(STATEMENT_LIST *);

/* Optional length allocation constructors */
OPT_LENGTH *make_OPT_LENGTH_EXPRES(EXPRES *);
OPT_LENGTH *make_OPT_LENGTH_EMPTY();

/* Optional else constructors */ 
OPT_ELSE *make_OPT_ELSE_STATEMENT(STATEMENT *);
OPT_ELSE *make_OPT_ELSE_EMPTY();

/* Variable constructors */
VAR *make_VAR_ID(char *);
VAR *make_VAR_ARRAY(VAR *,EXPRES *);
VAR *make_VAR_RECORD(VAR *, char*);

/* Expression constructor */
EXPRES *make_EXPRES_TERM(TERM *);
EXPRES *make_EXPRES_PLUS(EXPRES *,EXPRES *);
EXPRES *make_EXPRES_MINUS(EXPRES *,EXPRES *);
EXPRES *make_EXPRES_TIMES(EXPRES *,EXPRES *);
EXPRES *make_EXPRES_DIVIDE(EXPRES *,EXPRES *);
EXPRES *make_EXPRES_EQ(EXPRES *,EXPRES *);
EXPRES *make_EXPRES_NEQ(EXPRES *,EXPRES *);
EXPRES *make_EXPRES_GREATER(EXPRES *,EXPRES *);
EXPRES *make_EXPRES_LESS(EXPRES *,EXPRES *);
EXPRES *make_EXPRES_GEQ(EXPRES *,EXPRES *);
EXPRES *make_EXPRES_LEQ(EXPRES *,EXPRES *);
EXPRES *make_EXPRES_AND(EXPRES *,EXPRES *);
EXPRES *make_EXPRES_OR(EXPRES *,EXPRES *);

/*TERM constructor */
TERM *make_TERM_VAR(VAR *);
TERM *make_TERM_UMINUS(TERM *);
TERM *make_TERM_ACT_LIST(char *,ACT_LIST *);
TERM *make_TERM_NOT(TERM *);
TERM *make_TERM_PARENTESES(EXPRES *);
TERM *make_TERM_ABS(EXPRES *);
TERM *make_TERM_NULL();
TERM *make_TERM_TRUE();
TERM *make_TERM_FALSE();
TERM *make_TERM_NUM(int);

/* Constructors for function calls */
ACT_LIST *make_ACT_LIST_EXPLIST(EXP_LIST *);
ACT_LIST *make_ACT_LIST_EMPTY();

/* Expression list constructors */
EXP_LIST *make_EXP_LIST_EXP(EXPRES *);
EXP_LIST *make_EXP_LIST_LIST(EXP_LIST *,EXPRES *);


#endif /* END OF __KITTYTREE_H */
