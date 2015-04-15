#include "kittyir.h"
#include "irInstructions.h"
#include <stdio.h>
#include <stdlib.h>

static linked_list *ir_lines; // linked list of ir_lines
extern BODY *_main_;

linked_list *IR_build() {
	ir_lines = initialize_list();
	// make ".globl main" directive
	IR_INSTRUCTION *globl_directive = make_instruction_globl();
	globl_directive->arg1 = make_argument_label("main");
	IR_LINE *globl_line = make_line_instruction(globl_directive,
		".globl");
	append_element(ir_lines, globl_line);
    // make "main:" label line
    IR_LINE *main_label = make_line_label("main");
    append_element(ir_lines, main_label);


	IR_builder_body(_main_);
	return ir_lines;
}

void IR_builder_function(FUNC *func) {

}

void IR_builder_head (HEAD *header) {

}

void IR_builder_body (BODY *body) {
	IR_builder_statement_list(body->statement_list);
}

void IR_builder_tail (TAIL *tail) {

}

void IR_builder_type ( TYPE *type) {

}

void IR_builder_par_decl_list ( PAR_DECL_LIST *pdecl) {

}

void IR_builder_var_decl_list ( VAR_DECL_LIST *vdecl) {

}

void IR_builder_var_type ( VAR_TYPE *vtype) {

}

void IR_builder_decl_list ( DECL_LIST *dlst) {

}

void IR_builder_declaration ( DECLARATION *decl) {

}

void IR_builder_statement_list ( STATEMENT_LIST *slst) {
	switch(slst->kind) {
		case compound_SL_K:
			IR_builder_statement(slst->value.compoundSL.statement);
			IR_builder_statement_list(slst->value.compoundSL.statement_list);
			break;
		case statement_SL_K:
			IR_builder_statement(slst->value.statement);
			break;
		default:
			break;
	}
}

void IR_builder_statement ( STATEMENT *st) {
	IR_INSTRUCTION *new_instruction;
	IR_LINE *new_line;
	switch(st->kind) {
		case print_S_K:
			new_instruction = make_instruction_call();
			new_instruction->arg1 = make_argument_label("printf");
			new_line = make_line_instruction(new_instruction,NULL);

			append_element(ir_lines,new_line);
			break;
		default:
			break;
	}
} 

void IR_builder_opt_length ( OPT_LENGTH *oplen) {

}

void IR_builder_opt_else ( OPT_ELSE *opel) {

}

void IR_builder_variable ( VAR *var) {

}

void IR_builder_expression ( EXPRES *exp) {

}

void IR_builder_term ( TERM *term) {

}

void IR_builder_act_list ( ACT_LIST *actlst) {

}

void IR_builder_expression_list ( EXP_LIST *explst) {

}

void IR_pretty_printer ( linked_list *line_list ) {
	linked_list *temp;
	IR_LINE *line_to_print;

	temp = line_list->next;
	while(temp != line_list){
		line_to_print = (IR_LINE *) temp->data;

		switch(line_to_print->kind){
			case empty_line:
				printf("\n");
				break;
			case label_line:
				printf("%s:\n", line_to_print->label );
				break;
			case instruction_line:
				if(line_to_print->label != NULL){
					printf("%s ", line_to_print->label);
				}
				IR_pretty_printer_instruction(line_to_print->instruction);
				break;
		}

		temp = temp->next;
	}
}

void IR_pretty_printer_instruction ( IR_INSTRUCTION *instr ) {
	switch(instr->op_code){
		case globl:
			printf("%s\n", instr->arg1->value.label);
			break;
		case string:
			printf("%s\n", instr->arg1->value.label);
			break;
		case movl:
			break;
		case call:
			printf("\t call %s\n", instr->arg1->value.label);
			break;
		case pushl:
			break;
		case popl:
			break;
		case addl:
			break;
		case subl:
			break;
		case ret:
			break;
	}
}