#ifndef _KITTY_CHECK_H
#define _KITTY_CHECK_H
#include "../parserscanner/kittytree.h"
#include "../symbol/symbol.h"

#define NUMBER_OF_TYPES_SUPPORTED 8
#define CHECK_ERROR_SIGNAL 1

void begin_check(BODY *main);

void check_expression ( EXPRES *);
void check_term ( TERM *);

void check_function(FUNC *);
void check_head (HEAD *);
void check_body (BODY *);
void check_type ( TYPE *);
void check_par_decl_list ( PAR_DECL_LIST *);
void check_var_decl_list ( VAR_DECL_LIST *);
void check_decl_list ( DECL_LIST *);
void check_declaration ( DECLARATION *);
void check_statement_list ( STATEMENT_LIST *);
void check_statement ( STATEMENT *);
void check_opt_length ( OPT_LENGTH *);
void check_opt_else ( OPT_ELSE *);
int check_variable ( VAR *);

int check_act_list ( ACT_LIST *);
int check_expression_list ( EXP_LIST *);
void check_var_type (VAR_TYPE *);
SYMBOL_TYPE *get_base_array_type (SYMBOL_TYPE *);
int get_array_dim (SYMBOL_TYPE *);
int compare_record_as_sets (SYMBOL_TYPE *, SYMBOL_TYPE *);
void function_parameter_compare(EXP_LIST *, VAR_DECL_LIST *);

#endif
