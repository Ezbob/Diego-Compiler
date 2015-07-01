#ifndef KITTY_EMITTER_H
#define KITTY_EMITTER_H 
#include "dlinkedlist.h"
#include "parserscanner/kittytree.h"

#define MAX_LABEL_SIZE 20
#define WORD_SIZE 4
#define NEW_LABEL ((char*) calloc(MAX_LABEL_SIZE + 1, sizeof(char*)))

typedef enum REGISTERS {
	r_eax, r_ebx, r_ecx, r_edx, r_ebp, r_esp, r_esi, r_edi 
} REGISTERS;

typedef enum OP_CODES {
	directive, label, movl, call, pushl, popl, addl,
	subl, ret, xor, divl, imul, cmp, jne, jmp, je,
	JGE, JLE, jl, jg, notl, negl, intCode, space, leal,
	incl, decl
} OP_CODES;

typedef enum ARGUMENT_KIND {
	address_arg, register_arg, label_arg,
	constant_arg, tempReg_arg, indexing_arg, staticLink_arg
} ARGUMENT_KIND;

typedef struct IR_INSTRUCTION {
	int id;
	char *label;
	OP_CODES op_code;
	struct ARGUMENT *arg1;
	struct ARGUMENT *arg2;
} IR_INSTRUCTION;

typedef struct ARGUMENT {
	int temp_id;
	ARGUMENT_KIND kind;
	char *label;
	int intConst;
	REGISTERS reg;
	struct ARGUMENT *displace;
	struct ARGUMENT *base;
	struct ARGUMENT *index;
	char *charConst;
} ARGUMENT;

typedef struct SECTION {
	char *sectionName;
	int temps;
	struct SYMBOL_TABLE *symboltable;
	struct SECTION *nextSection;
	struct SECTION *prevSection;
	struct IR_INSTRUCTION *first;
	struct IR_INSTRUCTION *last;
	struct LIVENESS *root;
	struct LIVENESS *tail;
} SECTION;

/*
 * stores all IR code in a linked_list
 */
linked_list *IR_build(BODY *program);
void IR_builder_function(FUNC *func);
void IR_builder_head (HEAD *header);
void IR_builder_body (BODY *body);
void IR_builder_var_decl_list ( VAR_DECL_LIST *vdecl);
void IR_builder_var_type ( VAR_TYPE *vtype);
void IR_builder_decl_list ( DECL_LIST *dlst);
void IR_builder_declaration ( DECLARATION *decl);
void IR_builder_statement_list ( STATEMENT_LIST *slst);
void IR_builder_statement ( STATEMENT *st);
void IR_builder_act_list ( ACT_LIST *actList);
void IR_builder_expression_list ( EXP_LIST *expList);
void caller_save();
void caller_restore();
void callee_save();
void callee_restore();
void callee_start();
void callee_end();
IR_INSTRUCTION *local_variable_allocation(SYMBOL_TABLE *);
void build_exception(char *intCode);
void init_static_link();
void init_heap();
void add_Static_Link(int id);
void add_to_stack_pointer(int i);
ARGUMENT *get_register(int n);
void basic_assign(linked_list *ir_lines);
void assign_instruction_number(linked_list *ir_lines);
void repair_memory(linked_list *ir_code);
void build_data_section();

ARGUMENT *IR_builder_variable ( VAR *var);
void IR_builder_expression ( EXPRES *exp);
ARGUMENT *IR_builder_term ( TERM *term);
ARGUMENT *IR_builder_opt_length ( OPT_LENGTH *opt_length);

#endif