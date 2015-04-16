#include "kittyir.h"
#include "irInstructions.h"
#include <stdio.h>
#include <stdlib.h>
#define NEW_TEMPORARY_ID (current_temporary++)

static int current_temporary = 0;
static linked_list *ir_lines; // linked list of ir_lines
extern BODY *_main_;

linked_list *IR_build() {
	ir_lines = initialize_list();
	// make ".string "%d\n" "
	IR_INSTRUCTION *print_format = make_instruction_string();
	print_format->arg1 = make_argument_label("\"%d\\n\"");
	IR_LINE *print_format_line = make_line_instruction(print_format,"format");
	append_element(ir_lines,print_format_line);
	append_element(ir_lines,make_line_empty());

	// make ".globl main" directive
	IR_INSTRUCTION *globl_directive = make_instruction_globl();
	globl_directive->arg1 = make_argument_label("_main");
	IR_LINE *globl_line = make_line_instruction(globl_directive,NULL);
	append_element(ir_lines, globl_line);
    // make "main:" label line
    IR_LINE *main_label = make_line_label("_main");
    append_element(ir_lines, main_label);

    // make new stack frame
    IR_shift_to_new_frame();

    append_element(ir_lines,make_line_empty());

    // recursive run-through the AST
	IR_builder_body(_main_);

	append_element(ir_lines,make_line_empty());

	// shift back to old frame
	IR_shift_to_old_frame();

	// make "ret"
	IR_LINE *ret_line = make_line_instruction(make_instruction_ret(),NULL);
	append_element(ir_lines,ret_line);
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
	IR_INSTRUCTION *new_instruction; // note: using arrays here
	IR_LINE *new_line; // note: using arrays here
	switch(st->kind) {
		case print_S_K:
			// the printf call
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

void IR_shift_to_new_frame (){
	IR_LINE **new_lines;
	int i;
	new_lines = (IR_LINE **) malloc(sizeof(IR_LINE) * 2);
	new_lines[0] = make_line_instruction(make_instruction_pushl(),NULL);
	new_lines[1] = make_line_instruction(make_instruction_movl(),NULL);

	new_lines[0]->instruction->arg1 = make_argument_register();
	new_lines[0]->instruction->arg1->value.reg = 
										make_temp_register(NEW_TEMPORARY_ID);
	new_lines[0]->instruction->arg1->value.reg->value.registerName = ebp;

	new_lines[1]->instruction->arg1 = make_argument_register();
	new_lines[1]->instruction->arg2 = make_argument_register();
	new_lines[1]->instruction->arg1->value.reg = 
										make_temp_register(NEW_TEMPORARY_ID);
	new_lines[1]->instruction->arg2->value.reg = 
										make_temp_register(NEW_TEMPORARY_ID);
	new_lines[1]->instruction->arg1->value.reg->value.registerName = esp;
	new_lines[1]->instruction->arg2->value.reg->value.registerName = ebp;

	for(i = 0; i < 2; i++){
		append_element(ir_lines, new_lines[i]);
	}
}

void IR_shift_to_old_frame (){
	IR_LINE **new_lines;
	int i;
	new_lines = (IR_LINE **) malloc(sizeof(IR_LINE) * 2);
	new_lines[0] = make_line_instruction(make_instruction_popl(),NULL);
	new_lines[1] = make_line_instruction(make_instruction_movl(),NULL);

	new_lines[0]->instruction->arg1 = make_argument_register();
	new_lines[0]->instruction->arg1->value.reg = 
										make_temp_register(NEW_TEMPORARY_ID);
	new_lines[0]->instruction->arg1->value.reg->value.registerName = ebp;

	new_lines[1]->instruction->arg1 = make_argument_register();
	new_lines[1]->instruction->arg2 = make_argument_register();
	new_lines[1]->instruction->arg1->value.reg = 
										make_temp_register(NEW_TEMPORARY_ID);
	new_lines[1]->instruction->arg2->value.reg = 
										make_temp_register(NEW_TEMPORARY_ID);
	new_lines[1]->instruction->arg1->value.reg->value.registerName = ebp;
	new_lines[1]->instruction->arg2->value.reg->value.registerName = esp;

	for(i = 1; i >= 0; i--){
		append_element(ir_lines, new_lines[i]);
	}	
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
					printf("%s: ", line_to_print->label);
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
			printf(".globl %s\n", instr->arg1->value.label);
			break;
		case string:
			printf(".string %s\n", instr->arg1->value.label);
			break;
		case movl:
			printf("\t movl ");
			IR_pretty_printer_arguments(instr->arg1);
			IR_pretty_printer_arguments(instr->arg2);
			printf("\n");
			break;
		case call:
			printf("\t call ");
			IR_pretty_printer_arguments(instr->arg1);
			printf("\n");
			break;
		case pushl:
			printf("\t pushl ");
			IR_pretty_printer_arguments(instr->arg1);
			printf("\n");
			break;
		case popl:
			printf("\t popl ");
			IR_pretty_printer_arguments(instr->arg1);
			printf("\n");
			break;
		case addl:
			printf("\t addl ");
			IR_pretty_printer_arguments(instr->arg1);
			IR_pretty_printer_arguments(instr->arg2);
			printf("\n");
			break;
		case subl:
			printf("\t subl ");
			IR_pretty_printer_arguments(instr->arg1);
			IR_pretty_printer_arguments(instr->arg2);
			printf("\n");
			break;
		case ret:
			printf("\t ret \n");
			break;
	}
}

void IR_pretty_printer_arguments (ARGUMENT *arg) {
	switch(arg->kind){
		case address_arg:
			// TODO
		break;
		case register_arg:
			IR_pretty_printer_temp(arg->value.reg);
		break;
		case label_arg:
			printf("%s ", arg->value.label);
		break;
		case constant_arg:
			printf("%i ", arg->value.intConst);
		break;
	}
}

void IR_pretty_printer_temp (TEMP *tmp) {
	switch(tmp->kind){
		case register_temp:
			switch(tmp->value.registerName){
				case eax:
				printf("%%eax ");
				break;
				case ebx:
				printf("%%ebx ");
				break;
				case ecx:
				printf("%%ecx ");
				break;
				case edx:
				printf("%%edx ");
				break;
				case ebp:
				printf("%%ebp ");
				break;
				case esi:
				printf("%%esi ");
				break;
				case edi:
				printf("%%edi ");
				break;
				case esp:
				printf("%%esp ");
				break;
			}
		break;
		case virtual_temp:
		// TODO
		break;
		case spilled_temp:
		// TODO
		break;
	}
}
