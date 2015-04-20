#include "kittyir.h"
#include "irInstructions.h"
#include <stdio.h>
#include <stdlib.h>
#define NEW_TEMPORARY_ID (current_temporary++)

static int current_temporary = 0;
static int current_label = 0;
static int current_instruction = 0;
static linked_list *ir_lines; // linked list of ir_lines
extern BODY *_main_;

ARGUMENT *eax, *ebx, *ecx, *edx, *edi, *esi, *ebp, *esp;

void initRegisters(){
	eax = make_argument_register(r_eax, "eax");
	ebx = make_argument_register(r_ebx, "ebx");
	ecx = make_argument_register(r_ecx, "ecx");
	edx = make_argument_register(r_edx, "edx");
	edi = make_argument_register(r_edi, "edi");
	esi = make_argument_register(r_esi, "esi");
	ebp = make_argument_register(r_ebp, "ebp");
	esp = make_argument_register(r_esp, "esp");
}
linked_list *IR_build() {
	ir_lines = initialize_list();
	initRegisters();

	// make ".string "%d\n" "
	buildForm("formNUM:", ".string \"%d\\n\" ");

	// make ".globl main" directive
	ARGUMENT *global_label = make_argument_label(".globl main");
	IR_INSTRUCTION *globl_directive = make_instruction_globl(global_label, NULL);
	append_element(ir_lines, globl_directive);

    // make "main:" label line
	ARGUMENT *main_label = make_argument_label("main:");
	IR_INSTRUCTION *globl_main = make_instruction_globl(main_label, NULL);
	append_element(ir_lines, globl_main);

	IR_builder_body(_main_);

	IR_printer(ir_lines);

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
	ARGUMENT *returnvalue;
	ARGUMENT *arg1;
	ARGUMENT *arg2;

	switch(st->kind){

		case return_S_K:	
			returnvalue = IR_builder_expression(st->value.exp);
			IR_INSTRUCTION *movl = make_instruction_movl(returnvalue, eax);
			append_element(ir_lines, movl);
			IR_INSTRUCTION *ret = make_instruction_ret();
			calleeRestore();
			append_element(ir_lines, ret);
			break;

		case print_S_K:
			switch(st->value.exp->symboltype->type){

				case SYMBOL_INT:
					calleeSave();
					//Push arguments for print then form for print
					arg1 = IR_builder_expression(st->value.exp);
					callerSave();

					IR_INSTRUCTION *params = make_instruction_pushl(arg1, NULL);
					append_element(ir_lines, params);
					ARGUMENT *arg3 = make_argument_label("$formNUM");
					IR_INSTRUCTION *pushform = make_instruction_pushl(arg3, NULL);
					append_element(ir_lines, pushform);

					arg2 = make_argument_label("printf");
					IR_INSTRUCTION *call = make_instruction_call(arg1, arg2);
					append_element(ir_lines, call);

					moveStackpointer(2);

					callerRestore();
					calleeRestore();
					break;

				case SYMBOL_BOOL:
					break;

				default:
					printf("%s\n", "DEFAULT CASE EXP");
					break;
			}

		case ifbranch_S_K:

			break;
		default:
			break;
	}
} 

ARGUMENT *IR_builder_opt_length ( OPT_LENGTH *oplen) {
	return IR_builder_expression(oplen->value.exp);
}

void IR_builder_opt_else ( OPT_ELSE *opel) {

}

ARGUMENT *IR_builder_variable ( VAR *var) {

	ARGUMENT *arg = NEW(ARGUMENT);
	return arg;

}

ARGUMENT *IR_builder_expression ( EXPRES *exp) {
	ARGUMENT *argLeft;
	ARGUMENT *argRight;
	IR_INSTRUCTION *instr;
	IR_INSTRUCTION *edxsave;
	IR_INSTRUCTION *edxrestore;

	if(exp->kind != term_E_K){
		argLeft = IR_builder_expression(exp->value.sides.left);
		argRight = IR_builder_expression(exp->value.sides.right);
	}

	switch(exp->kind){
		case term_E_K:
			return IR_builder_term(exp->value.term);
			break;

		case plus_E_K:
			instr = make_instruction_addl(argLeft, argRight);
			append_element(ir_lines, instr);
			return argRight; 
			break;

		case minus_E_K:
			instr = make_instruction_subl(argLeft, argRight);
			append_element(ir_lines, instr);
			return argRight; 
			break;

		case times_E_K:
			instr = make_instruction_mul(argLeft, argRight);
			append_element(ir_lines, instr);
			return argRight;
			break;

		case divide_E_K:
			edxsave = make_instruction_pushl(edx, NULL); //Saving edx register
			append_element(ir_lines, edxsave);

			instr = make_instruction_xor(edx, edx); //Clear edx for DIV
			append_element(ir_lines, instr);

			IR_INSTRUCTION *instr1 = make_instruction_movl(argLeft, eax);
			append_element(ir_lines, instr1);

			IR_INSTRUCTION*instr2 = make_instruction_div(argRight, NULL);
			append_element(ir_lines, instr2);

			IR_INSTRUCTION *instr3 = make_instruction_movl(eax, argLeft);
			append_element(ir_lines, instr3);

			edxrestore = make_instruction_popl(edx, NULL);
			append_element(ir_lines, edxrestore); //Restoring edx register

			return argLeft;
			break;

		case booleq_E_K:


			break;
		case boolneq_E_K:
			break;
		case boolgreater_E_K:
			break;
		case boolless_E_K:
			break;
		case boolleq_E_K:
			break;
		case boolgeq_E_K:
			break;
		case booland_E_K:
			break;
		case boolor_E_K:
			break;
	}

	return argLeft;

}

ARGUMENT *IR_builder_term ( TERM *term) {

	ARGUMENT *arg1;
	ARGUMENT *arg2;
	IR_INSTRUCTION *instr;

	switch(term->kind){
		case num_T_K:
			arg1 = make_argument_constant(term->value.intconst);
			arg2 = make_argument_tempregister(current_temporary++);
			instr = make_instruction_movl(arg1, arg2);
			append_element(ir_lines, instr);
			return arg2; //Return arg2 to keep track of temps
			break;

		case boolTrue_T_K:
			arg1 = make_argument_constant(1);
			arg2 = make_argument_tempregister(current_temporary++);
			instr = make_instruction_movl(arg1, arg2);
			append_element(ir_lines, instr);
			return arg2; //Return arg2 to keep track of temps
			break;
			break;

		case boolFalse_T_K:
			arg1 = make_argument_constant(0);
			arg2 = make_argument_tempregister(current_temporary++);
			instr = make_instruction_movl(arg1, arg2);
			append_element(ir_lines, instr);
			return arg2; //Return arg2 to keep track of temps
			break;

		case null_T_K:
			arg1 = make_argument_constant(0);
			arg2 = make_argument_tempregister(current_temporary++);
			instr = make_instruction_movl(arg1, arg2);
			append_element(ir_lines, instr);
			return arg2; //Return arg2 to keep track of temps
			break;

		case expresPipes_T_K:
			break;

		case expresParent_T_K:
			arg1 = IR_builder_expression(term->value.exp);
			return arg1;
			break;

		case termBang_T_K:
			break;

		case var_T_K:
			arg1 = IR_builder_variable(term->value.var);
			arg2 = make_argument_tempregister(current_temporary++);
			instr = make_instruction_movl(arg1, arg2);
			append_element(ir_lines, instr);
			return arg2; //Return arg2 to keep track of temps			
			break;

		case actList_T_K:
			break;

		default:
			break;
	}
}

void IR_builder_act_list ( ACT_LIST *actlst) {

}

void IR_builder_expression_list ( EXP_LIST *explst) {

}


void callerSave(){

	IR_INSTRUCTION *instr1 = make_instruction_pushl(ecx, NULL);
	IR_INSTRUCTION *instr2 = make_instruction_pushl(edx, NULL);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);


}

void callerRestore(){

	IR_INSTRUCTION *instr1 = make_instruction_popl(edx, NULL);
	IR_INSTRUCTION *instr2 = make_instruction_popl(ecx, NULL);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);

}

void calleeSave(){

	IR_INSTRUCTION *instr1 = make_instruction_pushl(ebx, NULL);
	IR_INSTRUCTION *instr2 = make_instruction_pushl(esi, NULL);
	IR_INSTRUCTION *instr3 = make_instruction_pushl(edi, NULL);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);
	append_element(ir_lines, instr3);
}

void calleeRestore(){

	IR_INSTRUCTION *instr1 = make_instruction_popl(ebx, NULL);
	IR_INSTRUCTION *instr2 = make_instruction_popl(esi, NULL);
	IR_INSTRUCTION *instr3 = make_instruction_popl(edi, NULL);
	append_element(ir_lines, instr3);
	append_element(ir_lines, instr2);
	append_element(ir_lines, instr1);

}

void moveStackpointer(int i){

	ARGUMENT *arg = make_argument_constant(i*4);
	IR_INSTRUCTION *instr = make_instruction_addl(arg, esp);
	append_element(ir_lines, instr); 

}

void buildForm(char *name, char *actual){

	ARGUMENT *arg1 = make_argument_label(name);
	IR_INSTRUCTION *instr1 = make_instruction_globl(arg1, NULL);
	append_element(ir_lines, instr1);

	ARGUMENT *arg2 = make_argument_label(actual);
	IR_INSTRUCTION *instr2 = make_instruction_string(arg2);
	append_element(ir_lines, instr2);

}

void IR_printer(linked_list *ir_lines){

	linked_list *temp;
	temp = ir_lines->next;
	IR_INSTRUCTION *instr_to_print;

	while(temp != ir_lines){

		instr_to_print = (IR_INSTRUCTION *) temp->data;

		switch(instr_to_print->op_code){

			case popl:
				printf("\t%s", "popl ");
				IR_print_arguments(instr_to_print->arg1);
				printf("\n");
				break;

			case pushl:
				printf("\t%s", "pushl ");
				IR_print_arguments(instr_to_print->arg1);
				printf("\n");
				break;

			case ret:
				printf("\t%s\t\n", "ret");
				break;

			case movl:
				printf("\t%s", "movl ");
				IR_print_arguments(instr_to_print->arg1); 
				printf(", ");
				IR_print_arguments(instr_to_print->arg2);
				printf("\n");
				break;

			case addl:
				printf("\t%s", "addl ");
				IR_print_arguments(instr_to_print->arg1); 
				printf(", ");
				IR_print_arguments(instr_to_print->arg2);
				printf("\n");
				break;

			case subl:
				printf("\t%s", "subl ");
				IR_print_arguments(instr_to_print->arg1); 
				printf(", ");
				IR_print_arguments(instr_to_print->arg2);
				printf("\n");
				break;

			case xor:
				printf("\t%s", "XOR ");
				IR_print_arguments(instr_to_print->arg1);
				printf(", ");
				IR_print_arguments(instr_to_print->arg1);
				printf("\n");
				break;

			case divl:
				printf("\t%s", "DIV ");
				IR_print_arguments(instr_to_print->arg1);
				printf("\n");
				break;

			case call:
				printf("\t%s", "call ");
				IR_print_arguments(instr_to_print->arg2);
				printf("\n");
				break;

			case globl:
				IR_print_arguments(instr_to_print->arg1);
				printf("\n");
				break;

			case string:
				printf("\t");
				IR_print_arguments(instr_to_print->arg1);
				printf("\n\n");
				break;
			default:
				break;
		}

		temp = temp->next;
	}
}


void IR_print_arguments(ARGUMENT *arg){

	switch(arg->kind){

		case constant_arg:
			printf("$%d", arg->intConst);
			break;

		case register_arg:
			printf("%s", "%");
			printf("%s", arg->charConst);
			break;

		case tempreg_arg:
			printf("%s", "%");
			printf("%s%i", "temp", arg->tempid);
			break;

		case label_arg:
			printf("%s", arg->label);
			break;

		default:
			break;
	}

}
/*
	
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
*/