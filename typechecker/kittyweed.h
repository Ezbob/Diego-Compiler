#ifndef __KITTYWEED_H
#define __KITTYWEED_H
#define INDENTATION_SIZE 4 //this might be used?
#define WEEDER_FAILURE 1 //error code when exiting
#include "../parserscanner/kittytree.h" // need types
#include "stack.h"

/*
 Special function we should call when 
 reporting an error in the weeding phase.
 Shall stop the program. if second argument is 
 less than zero, then no line number will be 
 included in the error message.
 */
void weed_error_report(const char*,int);

/*
 Different functions for recursively going 
 through the tree. We need so many because we got 
 a lot of different types
 */
BODY *begin_weed(BODY *body);
FUNC *weed_function(FUNC *);
BODY *weed_body(BODY *);
DECL_LIST *weed_decl_list(DECL_LIST *);
DECLARATION *weed_declaration(DECLARATION *);
STATEMENT_LIST *weed_statement_list(STATEMENT_LIST *);
STATEMENT *weed_statement(STATEMENT *);
OPT_ELSE *weed_opt_else(OPT_ELSE *);
EXPRES *weed_expression( EXPRES *exp );
HEAD *weed_head ( HEAD *header);
TYPE *weed_type ( TYPE *type);
PAR_DECL_LIST *weed_par_decl_list ( PAR_DECL_LIST *pdecl);
VAR_DECL_LIST *weed_var_decl_list ( VAR_DECL_LIST *vdecl);
VAR_TYPE *weed_var_type ( VAR_TYPE *vtype);
OPT_LENGTH *weed_opt_length ( OPT_LENGTH *oplen);
TERM *weed_term ( TERM *term);
VAR *weed_variable ( VAR *var);
ACT_LIST *weed_act_list ( ACT_LIST *actlst);
EXP_LIST *weed_expression_list ( EXP_LIST *explst);

#endif /* END OF __KITTYWEED_H */





