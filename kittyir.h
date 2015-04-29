#ifndef KITTY_EMITTER_H
#define KITTY_EMITTER_H 
#include "dlinkedlist.h"
#include "dlinkedstack.h"
#include "parserscanner/kittytree.h"

#define MAXLABELSIZE 20

typedef enum ARGUMENTTYPE {
	REGISTER, TEMP, VIRTUAL, SPILLED, STATICLINK, INT
} ARGUMENTTYPE;

typedef enum REGISTERS {
	r_eax, r_ebx, r_ecx, r_edx, r_ebp, r_esp, r_esi, r_edi 
} REGISTERS;

typedef struct IR_INSTRUCTION {
	int id;
	char *label;
	enum { globl, mainmet, string, movl, call, pushl, popl, addl, 
		subl, ret, xor, divl, imul, cmp, jne, jmp, je,
		JGE, JLE, jl, jg, notl, negl, intcode } op_code; 
							// add more instructions later on
	struct ARGUMENT *arg1;
	struct ARGUMENT *arg2;

} IR_INSTRUCTION;


typedef struct ARGUMENT {
	int tempid;
	ARGUMENTTYPE type;
	enum { address_arg, register_arg, label_arg, 
		constant_arg, tempreg_arg } kind;
	char *label;
	int intConst;
	REGISTERS reg;
	char *charConst;
	void *address; // for use in local vars
} ARGUMENT;


/*
 * stores all IR code in a linked_list
 */
linked_list *IR_build(BODY *program, SYMBOLTABLE *symboltable);
void IR_builder_function(FUNC *func);
void IR_builder_head (HEAD *header);
void IR_builder_body (BODY *body);
void IR_builder_type ( TYPE *type);
void IR_builder_par_decl_list ( PAR_DECL_LIST *pdecl);
void IR_builder_var_decl_list ( VAR_DECL_LIST *vdecl);
void IR_builder_var_type ( VAR_TYPE *vtype);
void IR_builder_decl_list ( DECL_LIST *dlst);
void IR_builder_declaration ( DECLARATION *decl);
void IR_builder_statement_list ( STATEMENT_LIST *slst);
void IR_builder_statement ( STATEMENT *st);
void IR_builder_act_list ( ACT_LIST *actlst);
void IR_builder_expression_list ( EXP_LIST *explst);
void callerSave();
void callerRestore();
void calleeSave();
void calleeRestore();
void calleeStart();
void calleeEnd();
int getNextLabel();
void localVariableAllocation();
int getNextFunction();
void moveStackpointer(int i);
void IR_print_arguments(ARGUMENT *arg);
void IR_printer(linked_list *ir_lines);
void buildForm(char *name, char *actual);

ARGUMENT *IR_builder_variable ( VAR *var);
ARGUMENT *IR_builder_expression ( EXPRES *exp);
ARGUMENT *IR_builder_term ( TERM *term);
ARGUMENT *IR_builder_opt_length ( OPT_LENGTH *oplen);
//void IR_pretty_printer_instruction ( IR_INSTRUCTION *instr );
//void IR_pretty_printer_arguments (ARGUMENT *arg);
//void IR_pretty_printer_temp (TEMP *tmp);

//void IR_shift_to_new_frame ();
//void IR_shift_to_old_frame ();

#endif