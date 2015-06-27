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
void collect_function ( FUNC *function, SYMBOL_TABLE *st);
void collect_head ( HEAD *header, SYMBOL_TABLE *scope, SYMBOL_TABLE *st);
void collect_body ( BODY *body, SYMBOL_TABLE *st);
SYMBOL_TYPE *collect_type ( TYPE *type, SYMBOL_TABLE *st);
int collect_par_decl_list ( PAR_DECL_LIST *pdecl, SYMBOL_TABLE *st);
int collect_var_decl_list ( VAR_DECL_LIST *vdecl, SYMBOL_TABLE *st,
							SYMBOL_KIND);
void collect_var_type ( VAR_TYPE *vtype, SYMBOL_TABLE *st, int offset,
						SYMBOL_KIND);
void collect_decl_list ( DECL_LIST *dlst, SYMBOL_TABLE *st);
void collect_declaration ( DECLARATION *decl, SYMBOL_TABLE *st);
void collect_statement_list ( STATEMENT_LIST *slst, SYMBOL_TABLE *st);
void collect_statement ( STATEMENT *stm, SYMBOL_TABLE *st);
void collect_opt_length ( OPT_LENGTH *oplen, SYMBOL_TABLE *st);
void collect_opt_else ( OPT_ELSE *opel, SYMBOL_TABLE *st);
void collect_variable ( VAR *var, SYMBOL_TABLE *st);
void collect_expression ( EXPRES *exp, SYMBOL_TABLE *st);
void collect_term ( TERM *term, SYMBOL_TABLE *st);
void collect_act_list ( ACT_LIST *actlst, SYMBOL_TABLE *st);
void collect_expression_list ( EXP_LIST *explst, SYMBOL_TABLE *st);
SYMBOL_TYPE *make_SYMBOL_TYPE(TYPES_SUPPORTED);


#endif
