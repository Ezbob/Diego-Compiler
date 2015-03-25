#ifndef __KITTYPRINTER_H
#define __KITTYPRINTER_H
#define INDENTATION_SIZE 4
#define INDENT(n) for(int i = 0; i < (n * INDENTATION_SIZE ); i++){ \
							printf(" "); } \

#include "../parserscanner/kittytree.h"

void iprintf(const char* format, ...);
void printer_function ( FUNC *function);
void printer_head ( HEAD *header);
void printer_body ( BODY *body);
void printer_tail ( TAIL *tail);
void printer_type ( TYPE *type);
void printer_par_decl_list ( PAR_DECL_LIST *pdecl);
void printer_var_decl_list ( VAR_DECL_LIST *vdecl);
void printer_var_type ( VAR_TYPE *vtype);
void printer_decl_list ( DECL_LIST *dlst);
void printer_declaration ( DECLARATION *decl);
void printer_statement_list ( STATEMENT_LIST *slst);
void printer_statement ( STATEMENT *st);
void printer_opt_length ( OPT_LENGTH *oplen);
void printer_opt_else ( OPT_ELSE *opel);
void printer_variable ( VAR *var);
void printer_expression ( EXPRES *exp);
void printer_term ( TERM *term);
void printer_act_list ( ACT_LIST *actlst);
void printer_expression_list ( EXP_LIST *explst);

#endif /* END OF __KITTYPRINTER_H */
