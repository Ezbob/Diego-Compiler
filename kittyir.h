#ifndef KITTY_EMITTER_H
#define KITTY_EMITTER_H 
#include "dlinkedlist.h"


typedef struct IR_INSTRUCTION {
	void *label; // label defined as address
	enum { movl, call, pushl, popl, addl, subl } op_code; // add more instruc-
														// -tions when we need
														// them
	struct ARGUMENT *arg1;
	struct ARGUMENT *arg2;

} IR_INSTRUCTION;

typedef struct ARGUMENT {
	enum { address_arg, register_arg, label_arg, constant_arg } kind;
	union {
		struct TEMP *reg;
		int intConst;
	} value;
} ARGUMENT;

typedef struct TEMP {
	int id;
	enum { register_temp, virtual_temp, spilled_temp } kind;
	union {
		enum { eax, ebx, ecx, edx, ebp, esi, edi, esp } registerName;
		void *address;
	} value;
} TEMP;

/*
 * stores all IR code in a linked_list
 */
linked_list *IR_build();
void IR_builder_function(FUNC *func);
void IR_builder_head (HEAD *header);
void IR_builder_body (BODY *body);
void IR_builder_tail (TAIL *tail);
void IR_builder_type ( TYPE *type);
void IR_builder_par_decl_list ( PAR_DECL_LIST *pdecl);
void IR_builder_var_decl_list ( VAR_DECL_LIST *vdecl);
void IR_builder_var_type ( VAR_TYPE *vtype);
void IR_builder_decl_list ( DECL_LIST *dlst);
void IR_builder_declaration ( DECLARATION *decl);
void IR_builder_statement_list ( STATEMENT_LIST *slst);
void IR_builder_statement ( STATEMENT *st);
void IR_builder_opt_length ( OPT_LENGTH *oplen);
void IR_builder_opt_else ( OPT_ELSE *opel);
void IR_builder_variable ( VAR *var);
void IR_builder_expression ( EXPRES *exp);
void IR_builder_term ( TERM *term);
void IR_builder_act_list ( ACT_LIST *actlst);
void IR_builder_expression_list ( EXP_LIST *explst);

#endif