#ifndef KITTY_EMITTER_H
#define KITTY_EMITTER_H 
#include "dlinkedlist.h"
#include "parserscanner/kittytree.h"

#define MAX_LABEL_SIZE 20
#define WORD_SIZE 4
#define RUNTIME_ERROR_OUTBBOUNDS 2
#define RUNTIME_ERROR_DIVZERO 3
#define RUNTIME_ERROR_NEGSIZE 4
#define RUNTIME_ERROR_NULL 5
#define RUNTIME_ERROR_OUTMEM 6
#define MAX_HEAP_SIZE 4194304
#define NEW_LABEL ((char*) calloc(MAX_LABEL_SIZE + 1, sizeof(char)))
// standardization of the building of function labels for calls
#define GET_FUNCTION_LABEL(functionLabel, name, offset) functionLabel = \
	NEW_LABEL; sprintf(functionLabel,"f_%s.%d", name, offset)

#define GET_NEXT_LABEL_ID (current_label++)

typedef enum OP_CODES {
	directive, label, movl, call, pushl, popl, addl,
	subl, ret, xor, divl, imul, cmp, jne, jmp, je,
	JGE, JLE, jl, jg, notl, negl, intCode, space, leal,
	incl, decl
} OP_CODES;

typedef enum ARGUMENT_KIND {
	address_arg, register_arg, label_arg,
	constant_arg, tempReg_arg, indexing_arg, plain_constant_arg
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
void IR_build(BODY *program);
void IR_builder_function(FUNC *func);
void IR_builder_head(HEAD *head);
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
void function_epilog();
void function_prolog(int, SYMBOL_TABLE*);
IR_INSTRUCTION *local_variable_allocation(SYMBOL_TABLE *);
void init_heap();
void init_argument_constants();
void init_stack_instructions();
void init_registers();
void negative_array_size_check(int, ARGUMENT *arraySize);
void out_of_memory_runtime_check(int, ARGUMENT *);
void null_pointer_runtime_check(int, ARGUMENT *);
void out_of_bounds_runtime_check( int, ARGUMENT* variable, ARGUMENT *index );
void halt_for_error(char *, int, int);
void exit_assembler(int);
void add_to_stack_pointer(int i);
int is_already_defined(linked_list *definedList, char *labelName);
void build_data_section();
ARGUMENT *IR_builder_variable ( VAR *var);
void IR_builder_expression ( EXPRES *exp);
void IR_builder_term ( TERM *term);
void IR_builder_opt_length ( OPT_LENGTH *opt_length);

#endif