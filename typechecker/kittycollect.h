#ifndef __KITTY_TYPE_H
#define __KITTY_TYPE_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../symbol/symbol.h"
#include "../parserscanner/kittytree.h"

#define DEBUG_TYPE(symboltype) printf("BOOL>>> %i\n",symboltype->type \
			 == SYMBOL_BOOL); printf("INT>>> %i\n",symboltype->type \
				== SYMBOL_INT); \
			printf("ID>>> %i\n", symboltype->type == SYMBOL_ID); \
			printf("ARRAY>>> %i\n", symboltype->type == SYMBOL_ARRAY); \
			printf("RECORD>>> %i\n", symboltype->type == SYMBOL_RECORD); \
			printf("NULL>>> %i\n", symboltype->type == SYMBOL_NULL); \
			printf("UNKNOWN>>> %i\n", symboltype->type == SYMBOL_UNKNOWN); \
			printf("FUNCTION>>> %i\n", symboltype->type == SYMBOL_FUNCTION);

void collect(BODY *body);
void collect_function ( FUNC *function, SYMBOLTABLE *st);
void collect_head ( HEAD *header, SYMBOLTABLE *scope, SYMBOLTABLE *st);
void collect_body ( BODY *body, SYMBOLTABLE *st);
SYMBOLTYPE *collect_type ( TYPE *type, SYMBOLTABLE *st);
int collect_par_decl_list ( PAR_DECL_LIST *pdecl, SYMBOLTABLE *st);
int collect_var_decl_list ( VAR_DECL_LIST *vdecl, SYMBOLTABLE *st);
void collect_var_type ( VAR_TYPE *vtype, SYMBOLTABLE *st, int offset);
void collect_decl_list ( DECL_LIST *dlst, SYMBOLTABLE *st);
void collect_declaration ( DECLARATION *decl, SYMBOLTABLE *st);
void collect_statement_list ( STATEMENT_LIST *slst, SYMBOLTABLE *st);
void collect_statement ( STATEMENT *stm, SYMBOLTABLE *st);
void collect_opt_length ( OPT_LENGTH *oplen, SYMBOLTABLE *st);
void collect_opt_else ( OPT_ELSE *opel, SYMBOLTABLE *st);
void collect_variable ( VAR *var, SYMBOLTABLE *st);
void collect_expression ( EXPRES *exp, SYMBOLTABLE *st);
void collect_term ( TERM *term, SYMBOLTABLE *st);
void collect_act_list ( ACT_LIST *actlst, SYMBOLTABLE *st);
void collect_expression_list ( EXP_LIST *explst, SYMBOLTABLE *st);
SYMBOLTYPE *make_SYMBOLTYPE(TYPES_SUPPORTED);


#endif
