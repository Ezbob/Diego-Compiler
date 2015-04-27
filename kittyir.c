#include "kittyir.h"
#include "irInstructions.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int current_temporary = 0;
static int current_label = 0;
static int function_label = 0;

static linked_list *ir_lines; // plug IR code in here
static linked_stack *stackframe; // generate a stackframe

extern BODY *_main_;
extern SYMBOLTABLE *globalTable;

ARGUMENT *eax, *ebx, *ecx, *edx, *edi, *esi, *ebp, *esp;

char truelabelstring[MAXLABELSIZE];
char falselabelstring[MAXLABELSIZE];
char endlabelstring[MAXLABELSIZE];
char booltruelabel[MAXLABELSIZE];
char boolendlabel[MAXLABELSIZE];
char elselabel[MAXLABELSIZE];
char endiflabel[MAXLABELSIZE];
char orOKlabel[MAXLABELSIZE];
char orEndlabel[MAXLABELSIZE];
char andFalselabel[MAXLABELSIZE];
char andEndlabel[MAXLABELSIZE];
char functionlabel[MAXLABELSIZE];

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

int getNextLabel(){

	return current_label++;
}

int getNextFunction(){

	return function_label++;
}

linked_list *IR_build(BODY *program, SYMBOLTABLE *symboltable) {
	fprintf(stderr, "%s\n", "Initializing intermediate code generation");
	ir_lines = initialize_list();
	initRegisters();

	globalTable = symboltable;
	_main_ = program;

	// make ".string "%d\n" "
	buildForm("formNUM:", ".string \"%d\\n\" ");
	buildForm("formTRUE:", ".string \"TRUE\\n\" ");
	buildForm("formFALSE:", ".string \"FALSE\\n\" ");

	IR_builder_body(_main_);

	IR_INSTRUCTION *ret = make_instruction_ret();
	append_element(ir_lines, ret);
	IR_printer(ir_lines);
	return ir_lines;
}

//TODO
void IR_builder_function(FUNC *func) {

	SYMBOL *symbol = getSymbol(globalTable, func->functionF.head->headH.id);
	sprintf(functionlabel, "func%d", getNextFunction());
	strcpy(symbol->uniquename, functionlabel);

	sprintf(functionlabel, "%s:", functionlabel);

	ARGUMENT *func_label = make_argument_label(functionlabel);
	IR_INSTRUCTION *func_main = make_instruction_globl(func_label, NULL);
	append_element(ir_lines, func_main);

	IR_builder_head(func->functionF.head);
	IR_builder_body(func->functionF.body);

}


//TODO sæt ordenligt offset OBS reverse vars
void IR_builder_head (HEAD *header) {

	SYMBOL *symbol;
	SYMBOL *args = getSymbol(header->symboltable, header->headH.id);

	int count = 0;
	int offset = -1; //offset neg for stack

	VAR_DECL_LIST *vars = header->headH.pdeclist->value.var_decl_list;
	
	while(count < args->noArguments){

		if(globalTable != NULL || vars->value.var_type != NULL){
		
			if (vars->kind == comma_VDL_K) {
				symbol = getSymbol(header->headH.pdeclist->symboltable, vars->value.commaVDL.var_type->id);
			} else {
				symbol = getSymbol(header->headH.pdeclist->symboltable, vars->value.var_type->id);
			}

			if(symbol == NULL){
				fprintf(stderr, "%s\n", "Variable not found in symboltable");
				exit(1);
			}

			symbol->offset = offset;
			offset = offset - 1;
			vars = vars->value.commaVDL.var_decl_list;
			count++;
		} else {
			break;
		}
	}
}

//TODO
void IR_builder_body (BODY *body) {

	IR_builder_decl_list(body->decl_list);

	// make ".globl main" directive
	ARGUMENT *global_label = make_argument_label(".globl main");
	IR_INSTRUCTION *globl_directive = make_instruction_globl(global_label, NULL);
	append_element(ir_lines, globl_directive);

    // make "main:" label line
	ARGUMENT *main_label = make_argument_label("main:");
	IR_INSTRUCTION *globl_main = make_instruction_globl(main_label, NULL);
	append_element(ir_lines, globl_main);

	IR_builder_statement_list(body->statement_list);

}


void IR_builder_par_decl_list ( PAR_DECL_LIST __attribute__((__unused__)) *pdecl) {

	return;

}

void IR_builder_var_decl_list ( VAR_DECL_LIST __attribute__((__unused__)) *vdecl) {

	return;

}


void IR_builder_decl_list ( DECL_LIST *dlst) {

	if(dlst->kind != empty_DL_K){
		IR_builder_declaration(dlst->value.compoundDL.declaration);
	}

}

void IR_builder_declaration ( DECLARATION *decl) {

	switch(decl->kind){

		case typeassign_D_K:
			break;

		case func_D_K:
			IR_builder_function(decl->value.function);
			break;

		case var_D_K:
			break; 
	}

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

//Missing allocate, assign, print(records and arrays)
void IR_builder_statement ( STATEMENT *st) {
	int tmp = 0; 

	ARGUMENT *returnvalue;
	ARGUMENT *arg1;
	ARGUMENT *arg2;
	ARGUMENT *truearg;
	ARGUMENT *falsearg;
	ARGUMENT *endarg;
	ARGUMENT *compare;

	IR_INSTRUCTION *params;
	IR_INSTRUCTION *call;

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

					//move stackpointer and basepointer
					calleeStart();
					calleeSave();
					//Push arguments for print then form for print
					arg1 = IR_builder_expression(st->value.exp);

					callerSave();

					params = make_instruction_pushl(arg1, NULL);
					append_element(ir_lines, params);
					ARGUMENT *arg3 = make_argument_label("$formNUM");
					IR_INSTRUCTION *pushform = make_instruction_pushl(arg3, NULL);
					append_element(ir_lines, pushform);

					arg2 = make_argument_label("printf");
					call = make_instruction_call(arg1, arg2);
					append_element(ir_lines, call);

					moveStackpointer(2);

					callerRestore();
					calleeRestore();

					//move stackpointer and basepointer
					calleeEnd();

					break;

				case SYMBOL_BOOL:

					//move stackpointer and basepointer
					calleeStart();

					calleeSave();
					//Push arguments for print then form for print
					arg1 = IR_builder_expression(st->value.exp);
					callerSave();

					IR_INSTRUCTION *params = make_instruction_pushl(arg1, NULL);
					append_element(ir_lines, params);

					if(st->value.exp->value.term->kind == boolTrue_T_K){
						ARGUMENT *arg3 = make_argument_label("$formTRUE");
						IR_INSTRUCTION *pushform = make_instruction_pushl(arg3, NULL);
						append_element(ir_lines, pushform);

					} else if(st->value.exp->value.term->kind == boolFalse_T_K){
						ARGUMENT *arg3 = make_argument_label("$formFALSE");
						IR_INSTRUCTION *pushform = make_instruction_pushl(arg3, NULL);
						append_element(ir_lines, pushform);						
					}

					arg2 = make_argument_label("printf");
					IR_INSTRUCTION *call = make_instruction_call(arg1, arg2);
					append_element(ir_lines, call);

					moveStackpointer(2);

					callerRestore();
					calleeRestore();

					//move stackpointer and basepointer
					calleeEnd();
					break;

				default:
					printf("%s\n", "DEFAULT CASE PRINT");
					break;
			}
			break;

		case assign_S_K:


			break;

		case ifbranch_S_K:
			tmp = getNextLabel();

			arg1 = IR_builder_expression(st->value.ifbranchS.exp);

			sprintf(elselabel, "else%d:", tmp);
			sprintf(endlabelstring, "endIf%d:", tmp);

			falsearg = make_argument_label(elselabel);
			endarg = make_argument_label(endlabelstring);

			IR_INSTRUCTION *falselabel = make_instruction_globl(falsearg, NULL);
			IR_INSTRUCTION *endlabel = make_instruction_globl(endarg, NULL);

			compare = make_argument_constant(1); //Compare with true
			IR_INSTRUCTION *cmpinstr = make_instruction_cmp(arg1, compare);
			append_element(ir_lines ,cmpinstr);

			IR_INSTRUCTION *jneinstr = make_instruction_jne(elselabel);
			append_element(ir_lines, jneinstr);

			IR_builder_statement(st->value.ifbranchS.statement);
			IR_INSTRUCTION *jmpend = make_instruction_jmp(endlabelstring);
			append_element(ir_lines, jmpend);

			if(st->value.ifbranchS.opt_else->value.statement != NULL){
				append_element(ir_lines, falselabel);
				IR_builder_statement(st->value.ifbranchS.opt_else->value.statement);
			}
			
			append_element(ir_lines, endlabel);

			break;

		case allocate_S_K:
			break;

		case while_S_K:
			sprintf(truelabelstring, "whileStart%d:", current_label);
			truearg = make_argument_label(truelabelstring);
			IR_INSTRUCTION *truelabel = make_instruction_globl(truearg, NULL);

			sprintf(endlabelstring, "whileEnd%d:", current_label);
			endarg = make_argument_label(endlabelstring);
			IR_INSTRUCTION *whileend = make_instruction_globl(endarg, NULL);
			append_element(ir_lines, truelabel);

			arg1 = IR_builder_expression(st->value.whileS.exp);
			compare = make_argument_constant(1); //Compare with true
			IR_INSTRUCTION *cmpinst = make_instruction_cmp(arg1, compare);
			append_element(ir_lines, cmpinst);
			IR_INSTRUCTION *jneinst = make_instruction_jne(endlabelstring);
			append_element(ir_lines, jneinst);			

			IR_builder_statement(st->value.whileS.statement);
			IR_INSTRUCTION *jmpstart = make_instruction_jmp(truelabelstring);
			append_element(ir_lines, jmpstart);
			append_element(ir_lines, whileend);
			current_label++;
			break;

		case statement_list_S_K:
			IR_builder_statement_list(st->value.statement_list);
			break;

		default:
			printf("%s\n", "GOT NOTHING");

	}
} 

ARGUMENT *IR_builder_opt_length ( OPT_LENGTH *oplen) {
	return IR_builder_expression(oplen->value.exp);
}

//TODO
ARGUMENT *IR_builder_variable ( VAR __attribute__((__unused__)) *var) {

	ARGUMENT *arg = make_argument_constant(1);
	return arg;

}

ARGUMENT *IR_builder_expression ( EXPRES *exp) {
	int tmp = 0;

	ARGUMENT *argLeft;
	ARGUMENT *argRight;
	ARGUMENT *truearg;
	ARGUMENT *falsearg;
	ARGUMENT *endarg;
	ARGUMENT *result;
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
		case boolneq_E_K:
		case boolgreater_E_K:
		case boolless_E_K:
		case boolleq_E_K:
		case boolgeq_E_K:
			result = make_argument_tempregister(current_temporary++);
			tmp = getNextLabel();


			sprintf(booltruelabel, "booOPtrue%d:", tmp);
			truearg = make_argument_label(booltruelabel);
			IR_INSTRUCTION *truelabel = make_instruction_globl(truearg, NULL);

			sprintf(boolendlabel, "boolOPend%d:", tmp);
			endarg = make_argument_label(boolendlabel);
			IR_INSTRUCTION *endlabel = make_instruction_globl(endarg, NULL);

			instr = make_instruction_cmp(argLeft, argRight);
			append_element(ir_lines, instr);

			IR_INSTRUCTION *truejmp;

			switch(exp->kind){
				case booleq_E_K:
					truejmp = make_instruction_je(booltruelabel);
					break;

				case boolneq_E_K:
					truejmp = make_instruction_jne(booltruelabel);
					break;

				case boolgreater_E_K:
					truejmp = make_instruction_jl(booltruelabel);
					break;

				case boolless_E_K:
					truejmp = make_instruction_jg(booltruelabel);
					break;

				case boolleq_E_K:
					truejmp = make_instruction_JGE(booltruelabel);
					break;

				case boolgeq_E_K:
					truejmp = make_instruction_JLE(booltruelabel);
					break;

				default:
					return argRight;
			}

			append_element(ir_lines, truejmp);

			//false
			IR_INSTRUCTION *falsesave = make_instruction_movl(make_argument_constant(0), result);
			append_element(ir_lines, falsesave);

			IR_INSTRUCTION *endjmp = make_instruction_je(boolendlabel);
			append_element(ir_lines, endjmp);

			//true
			append_element(ir_lines, truelabel);
			IR_INSTRUCTION *truesave = make_instruction_movl(make_argument_constant(1), result);
			append_element(ir_lines, truesave);

			append_element(ir_lines, endlabel);
			return result;
			break;
		case booland_E_K:
			tmp = getNextLabel();

			sprintf(andFalselabel, "ANDfalse%d:", tmp);
			falsearg = make_argument_label(andFalselabel);
			IR_INSTRUCTION *andFalseinstr = make_instruction_globl(falsearg, NULL);

			sprintf(andEndlabel, "ANDend%d:", tmp);
			endarg = make_argument_label(andEndlabel);
			IR_INSTRUCTION *andEndinstr = make_instruction_globl(endarg, NULL);

			result = make_argument_tempregister(current_temporary++);

			ARGUMENT *cmpARG = make_argument_constant(1);

			IR_INSTRUCTION *cmp1 = make_instruction_cmp(argLeft, cmpARG);
			append_element(ir_lines, cmp1);

			IR_INSTRUCTION *jmpfalse = make_instruction_jne(andFalselabel);
			append_element(ir_lines, jmpfalse);

			IR_INSTRUCTION *cmp2 = make_instruction_cmp(argRight, cmpARG);
			append_element(ir_lines, cmp2);
			append_element(ir_lines, jmpfalse);

			IR_INSTRUCTION *trueand = make_instruction_movl(make_argument_constant(1), result);
			append_element(ir_lines, trueand);

			IR_INSTRUCTION *andjmpend = make_instruction_jmp(andEndlabel);
			append_element(ir_lines, andjmpend);

			append_element(ir_lines, andFalseinstr);
			IR_INSTRUCTION *falseand = make_instruction_movl(make_argument_constant(0), result);
			append_element(ir_lines, falseand);

			append_element(ir_lines, andEndinstr);

			break;
		case boolor_E_K://NEEDS WORK
			tmp = getNextLabel();

			sprintf(orOKlabel, "ORtrue%d:", tmp);
			truearg = make_argument_label(orOKlabel);
			IR_INSTRUCTION *oroklabel = make_instruction_globl(truearg, NULL);

			sprintf(orEndlabel, "ORend%d:", tmp);
			endarg = make_argument_label(orEndlabel);
			IR_INSTRUCTION *orendlabel = make_instruction_globl(endarg, NULL);

			result = make_argument_tempregister(current_temporary++);

			ARGUMENT *cmporARG = make_argument_constant(1);

			IR_INSTRUCTION *cmpor1 = make_instruction_cmp(argLeft, cmporARG);
			append_element(ir_lines, cmpor1);

			IR_INSTRUCTION *jmpOK = make_instruction_je(orOKlabel);
			append_element(ir_lines, jmpOK);

			IR_INSTRUCTION *cmpor2 = make_instruction_cmp(argRight, cmporARG);
			append_element(ir_lines, cmpor2);
			append_element(ir_lines, jmpOK);

			IR_INSTRUCTION *orfalsesave = make_instruction_movl(make_argument_constant(0), result);
			append_element(ir_lines, orfalsesave);

			IR_INSTRUCTION *jmpend = make_instruction_jmp(orEndlabel);
			append_element(ir_lines, jmpend);
			append_element(ir_lines, oroklabel);

			IR_INSTRUCTION *ortruesave = make_instruction_movl(make_argument_constant(1), result);
			append_element(ir_lines, ortruesave);

			append_element(ir_lines, orendlabel);

			break;
	}

	return argLeft;

}

//Missing Function calls, ABS, BANG
ARGUMENT *IR_builder_term ( TERM *term) {

	ARGUMENT *arg1;
	ARGUMENT *arg2;
	IR_INSTRUCTION *instr;
	ACT_LIST *funcparams;
	SYMBOL *symbol;
	int params;

	switch(term->kind){
		case num_T_K:
			arg1 = make_argument_constant(term->value.intconst);
			arg2 = make_argument_tempregister(current_temporary++);
			instr = make_instruction_movl(arg1, arg2);
			append_element(ir_lines, instr);
			return arg2; //Return arg2 to keep track of temps

		case boolTrue_T_K:
			arg1 = make_argument_constant(1);
			arg2 = make_argument_tempregister(current_temporary++);
			instr = make_instruction_movl(arg1, arg2);
			append_element(ir_lines, instr);
			return arg2; //Return arg2 to keep track of temps

		case boolFalse_T_K:
			arg1 = make_argument_constant(0);
			arg2 = make_argument_tempregister(current_temporary++);
			instr = make_instruction_movl(arg1, arg2);
			append_element(ir_lines, instr);
			return arg2; //Return arg2 to keep track of temps

		case null_T_K:
			arg1 = make_argument_constant(0);
			arg2 = make_argument_tempregister(current_temporary++);
			instr = make_instruction_movl(arg1, arg2);
			append_element(ir_lines, instr);
			return arg2; //Return arg2 to keep track of temps

		case expresParent_T_K:
			arg1 = IR_builder_expression(term->value.exp);
			return arg1;

		case var_T_K:
			arg1 = IR_builder_variable(term->value.var);
			arg2 = make_argument_tempregister(current_temporary++);
			instr = make_instruction_movl(arg1, arg2);
			append_element(ir_lines, instr);
			return arg2; //Return arg2 to keep track of temps			

		case actList_T_K:

			//oprette static link?
			symbol = getSymbol(term->symboltable, term->value.actlistT.id);
			params = 0;
			EXPRES *par;

			while(params < symbol->noArguments){

				if(term->value.actlistT.actlist->value.exp_list->kind == exp_EL_K){
					arg1 = IR_builder_expression(term->value.actlistT.actlist->value.exp_list->value.exp);
					instr = make_instruction_pushl(arg1, NULL);
					append_element(ir_lines, instr);
				} else {
					arg1 = IR_builder_expression(term->value.actlistT.actlist->value.exp_list->value.commaEL.exp);
					instr = make_instruction_pushl(arg1, NULL);
					append_element(ir_lines, instr);
				}

				params++;
			}

			arg2 = make_argument_label(symbol->uniquename);
			IR_INSTRUCTION *call = make_instruction_call(NULL, arg2);
			append_element(ir_lines, call);

			moveStackpointer(symbol->noArguments);

			callerRestore();
			return arg2 = make_argument_tempregister(current_temporary++);



		default:
			break;
	}

	return NULL;
}

// TODO function // void IR_builder_act_list ( ACT_LIST *actlst) { }

void IR_builder_expression_list ( EXP_LIST *explst) {

	if(explst == NULL){
		return;
	}

	if(explst->kind == commalist_EL_K){
		IR_builder_expression_list(explst->value.commaEL.explist);
	}

	IR_builder_expression(explst->value.exp);
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

void calleeStart(){

	IR_INSTRUCTION *instr1 = make_instruction_pushl(ebp, NULL);
	IR_INSTRUCTION *instr2 = make_instruction_movl(esp, ebp);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);

}

void calleeEnd(){

	IR_INSTRUCTION *instr1 = make_instruction_movl(ebp, esp);
	IR_INSTRUCTION *instr2 = make_instruction_popl(ebp, NULL);
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
				printf("\t%s\n", "ret");
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
				printf("\t%s", "IDIV ");
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

			case cmp:
				printf("\t%s", "cmp ");
				IR_print_arguments(instr_to_print->arg1);
				printf("%s", ", ");
				IR_print_arguments(instr_to_print->arg2);
				printf("\n");
				break;

			case jne:
				printf("\t%s", "jne ");
				printf("%s", instr_to_print->label);
				printf("\n");
				break;

			case jmp:
				printf("\t%s", "jmp ");
				printf("%s", instr_to_print->label);
				printf("\n");
				break;

			case je:
				printf("\t%s", "je ");
				printf("%s", instr_to_print->label);
				printf("\n");
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
