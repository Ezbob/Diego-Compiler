#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "kittyir.h"
#include "irInstructions.h"
#include "kittyemit.h"

static int current_temporary = 1;
static int current_label = 0;
static int function_label = 0;
static int local_variable_size = 0;
static int instructionnumber = 0;

static linked_list *ir_lines; // plug IR code in here
static linked_list *data_lines; // for allocates

extern BODY *_main_;
extern SYMBOLTABLE *globalTable;
extern SECTION *mainSection;

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

int getNextLabel(){
	return current_label++;
}

int getNextFunction(){
	return function_label++;
}

linked_list *IR_build(BODY *program, SYMBOLTABLE *symboltable) {
	fprintf(stderr, "%s\n", "Initializing intermediate code generation");
	ir_lines = initialize_list();
	data_lines = initialize_list();
	initRegisters();

	globalTable = symboltable;
	_main_ = program;

	append_element(ir_lines, 
		make_instruction_globl(
			make_argument_label(".text"), 
			NULL
			)
		);

	mainSection = NEW(SECTION);
	mainSection->symboltable = globalTable;
	mainSection->temps = globalTable->temps;
	mainSection->sectionName = calloc(MAXLABELSIZE,sizeof(char));
	
	sprintf(mainSection->sectionName, "%s", "main");
	SECTION *tmp2 = mainSection;
	IR_builder_decl_list(_main_->decl_list);

	// make ".globl main" directive
	ARGUMENT *global_label = make_argument_label(".globl main");
	IR_INSTRUCTION *globl_directive = 
		make_instruction_globl(global_label, NULL);
	append_element(ir_lines, globl_directive);

	// make "main:" label line
	ARGUMENT *main_label = make_argument_label("main");
	IR_INSTRUCTION *globl_main = make_instruction_label(main_label, NULL);
	mainSection->first = globl_main;
	append_element(ir_lines, globl_main);

	calleeStart();
	calleeSave();

	localVariableAllocation();

	callerSave();
	
	IR_builder_statement_list(_main_->statement_list);
	
	calleeRestore();
	callerRestore();
	calleeEnd();

	local_variable_size = 0;
	IR_INSTRUCTION *ret = make_instruction_ret();
	mainSection->last = ret;

	append_element(ir_lines, ret);

	build_data_section();

	mainSection = tmp2;
	linked_list *save = ir_lines;
	assign_instructionnumber(ir_lines);
	basic_assign(ir_lines);
	ir_lines = save;
	IR_printer(ir_lines);
	
	return ir_lines;
}

void IR_builder_function(FUNC *func) {

	char *functionlabel = calloc(MAXLABELSIZE ,sizeof(char));

	SYMBOL *symbol = getSymbol(globalTable, func->functionF.head->headH.id);

	sprintf(functionlabel, "func%d", getNextFunction());

	strcpy(symbol->uniquename, functionlabel);

	sprintf(functionlabel, "%s", functionlabel);

	//Switching scope begins new section
	SECTION *temp = mainSection;
	while(mainSection->nextSection != NULL){
		mainSection = mainSection->nextSection;
	}

	mainSection->nextSection = NEW(SECTION);
	mainSection->nextSection->prevSection = mainSection;
	mainSection = mainSection->nextSection;
	mainSection->symboltable = globalTable;
	mainSection->sectionName = calloc(32,sizeof(char));
	sprintf(mainSection->sectionName, "%s", functionlabel);


	IR_INSTRUCTION *func_main = make_instruction_label(
									make_argument_label(
										functionlabel
									), 
									NULL
								);

	mainSection->first = func_main;

	append_element(ir_lines, func_main);

	
	IR_builder_head(func->functionF.head);
	IR_builder_body(func->functionF.body);

	//Section is done, restore old section
	mainSection = temp;

	//move stackpointer and basepointer
	calleeEnd();
	local_variable_size = 0;
	IR_INSTRUCTION *ret = make_instruction_ret();
	mainSection->last = ret;

	append_element(ir_lines, ret);

}


//TODO sæt ordenligt offset OBS reverse vars
void IR_builder_head (HEAD *header) {

	SYMBOL *symbol;
	SYMBOL *args = getSymbol(header->symboltable, header->headH.id);
	mainSection->temps = args->noArguments;
	int count = 0;
	int offset = 2; 

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
			offset++;
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

	//move stackpointer and basepointer
	calleeStart();
	calleeSave();

	localVariableAllocation();

	IR_builder_statement_list(body->statement_list);
}

void IR_builder_var_decl_list ( VAR_DECL_LIST *vdecl) {
	switch(vdecl->kind){
		case comma_VDL_K:
			IR_builder_var_decl_list(vdecl->value.commaVDL.var_decl_list);
			IR_builder_var_type(vdecl->value.commaVDL.var_type);
			break;
		case var_VDL_typeK:
			IR_builder_var_type(vdecl->value.var_type);
			break;
	}
	
}

 void IR_builder_var_type ( VAR_TYPE * vtype ){
	switch(vtype->type->kind){ // note: switching on type kind
		case int_TY_K:
			local_variable_size += WORDSIZE;
			break;
 
		case bool_TY_K:
			local_variable_size += WORDSIZE; 
			break;

		case arrayof_TY_K:
			// local_variable_size += WORDSIZE; // parse by reference but we have to find out if it's parameters 
			break;
		case recordof_TY_K:
			// local_variable_size += WORDSIZE; // like arrays
			break;
		default:
			break;
	}
 }

void IR_builder_decl_list ( DECL_LIST *dlst) {

	switch(dlst->kind){
		case compound_DL_K:
			IR_builder_decl_list(dlst->value.compoundDL.decl_list);
			IR_builder_declaration(dlst->value.compoundDL.declaration);
			break;
		case empty_DL_K:
			break;
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
			IR_builder_var_decl_list(decl->value.var_decl_list);
			break; 
	}

}

void IR_builder_statement_list ( STATEMENT_LIST *slst) {
	switch(slst->kind) {
		case compound_SL_K:
			IR_builder_statement_list(slst->value.compoundSL.statement_list);
			IR_builder_statement(slst->value.compoundSL.statement);
			break;
		case statement_SL_K:
			IR_builder_statement(slst->value.statement);
			break;
		default:
			break;
	}
}


void IR_builder_statement ( STATEMENT *st ) {
	int tmp = 0; 

	char *endwhilestring;
	char *truewhilestring;
	char *endlabelstring;
	char *elselabel;
	char *printlabel;
	char *falselabel;
	char *truelabel;

	ARGUMENT *returnvalue;
	ARGUMENT *arg1;
	ARGUMENT *arg2;
	ARGUMENT *arg3;
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
			calleeRestore();
			
			break;

		case print_S_K: 
			switch(st->value.exp->symboltype->type){

				case SYMBOL_BOOL:
					
					// generating argument for boolean expression
					arg1 = IR_builder_expression(st->value.exp);

					tmp = getNextLabel();

					falselabel = calloc(MAXLABELSIZE,sizeof(char));
					truelabel = calloc(MAXLABELSIZE,sizeof(char));
					printlabel = calloc(MAXLABELSIZE, sizeof(char));

					sprintf(falselabel,"bfalse%d", tmp );
					sprintf(truelabel, "btrue%d",tmp);
					sprintf(printlabel,"printbool%d",tmp);

					append_element( // compaire boolean value to true
						ir_lines,
						make_instruction_cmp(
							make_argument_constant(1),
							arg1
							)
						);

					append_element( // true has to be printed?
						ir_lines,
						make_instruction_jne(
								falselabel
							)
						);

					callerSave();

					append_element(ir_lines, // making a push to the stack
											// with result of expression
						make_instruction_pushl(
							arg1,
							NULL
							)
						);

					append_element( // true case here
						ir_lines,
						make_instruction_pushl(
							make_argument_label("$formTRUE"),
							NULL
							)
						);

					append_element( // jmp to printf call 
						ir_lines,
						make_instruction_jmp(
								printlabel
							)
						);

					append_element( // false label section
						ir_lines,
						make_instruction_label(
							make_argument_label(falselabel),
							NULL
							)
						);

					callerSave();
					
					append_element(ir_lines, // making a push to the stack
											// with result of expression
						make_instruction_pushl(
							arg1,
							NULL
							)
						);

					append_element( // false case here
						ir_lines,
						make_instruction_pushl(
							make_argument_label("$formFALSE"),
							NULL
							)
						);

					append_element( // printing section
						ir_lines,
						make_instruction_label(
							make_argument_label(printlabel),
							NULL
							)
						);

					append_element( // call to print
						ir_lines,
						make_instruction_call(
							arg1,
							make_argument_label("printf")
							)
						);

					moveStackpointer(2); // clean up
					callerRestore();

					break;

				case SYMBOL_INT:
				case SYMBOL_NULL:

					//Push arguments for print then form for print
					arg1 = IR_builder_expression(st->value.exp);
					callerSave();

					params = make_instruction_pushl(arg1, NULL);
					append_element(ir_lines, params);
		
					if (st->value.exp->value.term->kind == null_T_K){
						arg3 = make_argument_label("$formNULL");
						IR_INSTRUCTION *pushform = make_instruction_pushl(arg3, NULL);
						append_element(ir_lines, pushform);
					} else {
						arg3 = make_argument_label("$formNUM");
						IR_INSTRUCTION *pushform = make_instruction_pushl(arg3, NULL);
						append_element(ir_lines, pushform);
					}

					arg2 = make_argument_label("printf");
					call = make_instruction_call(arg1, arg2);
					append_element(ir_lines, call);

					moveStackpointer(2);
					callerRestore();

					break;

				case SYMBOL_ARRAY:
					//	printf("%s\n", "IN WRITE");
					callerSave();
					arg1 = IR_builder_expression(st->value.exp);

					append_element(
						ir_lines,
						make_instruction_pushl(
							arg1,
							NULL
						)
					);

					arg3 = make_argument_label("$formNUM");
					IR_INSTRUCTION *pushform = make_instruction_pushl(arg3, NULL);
					append_element(ir_lines, pushform);
					arg2 = make_argument_label("printf");
					call = make_instruction_call(NULL, arg2);
					append_element(ir_lines, call);

					moveStackpointer(2);
					callerRestore();
					break;
				default:
				//	printf("%s\n", "DEFAULT CASE PRINT");
					break;
			}
			break;

		case assign_S_K:  // todo with local variables

			switch(st->value.assignS.variable->kind){

				case indexing_V_K:
					arg2 = IR_builder_variable(st->value.assignS.variable);
					arg1 = IR_builder_expression(st->value.assignS.exp);
					
					append_element( 
						ir_lines,
						make_instruction_movl(
							arg1,
							arg2
						)
					);
					break;

				case dot_V_K:

					arg1 = IR_builder_expression(st->value.assignS.exp);
					arg2 = IR_builder_variable(st->value.assignS.variable);

					append_element( 
						ir_lines,
						make_instruction_movl(
							arg1,
							arg2
						)
					);
					break;

				case id_V_K:
					arg1 = IR_builder_expression(st->value.assignS.exp);

					SYMBOL *symbol = getSymbol(st->symboltable,st->value
						.assignS.variable->value.id);

					if( symbol->offset == 0 && 
						local_variable_size >= WORDSIZE && symbol != NULL ) { 
						// assigning offsets in stack
					
						symbol->offset = -1 * (local_variable_size / 
																	WORDSIZE);
						local_variable_size -= WORDSIZE;
					}
					
					append_element( // actual assignment
						ir_lines,
						make_instruction_movl (
							arg1, 
							make_argument_address ( 
								(symbol->offset * WORDSIZE)
								)
							)
						);
					break;
				}
			break;

		case ifbranch_S_K:
			tmp = getNextLabel();

			elselabel = calloc(MAXLABELSIZE,sizeof(char));
			endlabelstring = calloc(MAXLABELSIZE,sizeof(char));

			sprintf(elselabel, "else%d", tmp);
			sprintf(endlabelstring, "endIf%d", tmp);

			arg1 = IR_builder_expression(st->value.ifbranchS.exp); 
				// generate code for boolean expression(s)

			append_element( //Comparison with "true" boolean value
				ir_lines, 
				make_instruction_cmp(
					make_argument_constant(1), 
					arg1
					) 
				);

			if(st->value.ifbranchS.opt_else->kind != empty_OE_K){
				append_element( // if not equal goto else part
					ir_lines,
					make_instruction_jne(elselabel) 
				);
			} else {
				append_element( // if not equal goto end-of-if
					ir_lines,
					make_instruction_jne(endlabelstring) 
				);
			}

			IR_builder_statement(st->value.ifbranchS.statement);
				// build statements in if-case


			if(st->value.ifbranchS.opt_else->kind != empty_OE_K){

				append_element( // we have to jump over 
									//else when if-case is true
					ir_lines, 
					make_instruction_jmp(
						endlabelstring
						)
					);

				append_element( // make else-label
					ir_lines, 
					make_instruction_label(
						make_argument_label(elselabel),
						NULL 
					)
				);

				IR_builder_statement( // build else statements
					st->value.ifbranchS.opt_else->value.statement);
			}
			
			append_element( // end-of-if label
				ir_lines, 
				make_instruction_label(
					make_argument_label(endlabelstring), 
					NULL
					)
			);

			break;

		case allocate_S_K:

			switch(st->value.allocateS.variable->symboltype->type){

				case SYMBOL_ARRAY:
					if (st->value.allocateS.opt_length->kind == lengthof_OL_K) {
						
						if(get_length(data_lines) == 0)	{ // init heap counter
							append_element(
								ir_lines, 
								make_instruction_movl(
									make_argument_label("$heap"),
									make_argument_label("(heapNext)")		
									)
							);
						}

						// put check out of memory here

						char *address_of_id = calloc(strlen(st->value.
						allocateS.variable->value.id) + 4, sizeof(char));

						sprintf(address_of_id, "(%s)",st->value.
							allocateS.variable->value.id);

						append_element(
							ir_lines,
							make_instruction_movl(
								make_argument_label("$heapNext"),
								make_argument_label(address_of_id)
							)
						);

						arg1 = IR_builder_expression(st->value.allocateS.
							opt_length->value.exp); // length of-result

						arg2 = make_argument_tempregister(current_temporary++);

						append_element(
							ir_lines,
							make_instruction_pushl(
								arg2,
								NULL
							)
						);

						append_element( // xored to get zero
							ir_lines,
							make_instruction_xor(
								arg2,
								arg2
							)
						);

						append_element(
							ir_lines,
							make_instruction_movl(
								arg1,
								make_argument_labelAddring(
									st->value.allocateS.variable->value.id,
									arg2
								)
							)
						);

						append_element( // first element is the length so incl
							ir_lines,
							make_instruction_incl(
								arg1
							)
						);

						// type check maybe? but symboltype of variabe is SYMBOL_ARRAY 
						append_element(
							ir_lines,
							make_instruction_imul(
								make_argument_constant(WORDSIZE),
								arg1
							)
						);

						append_element(
							ir_lines, // add to the next pointer
							make_instruction_addl(
								arg1, 
								make_argument_label("(heapNext)")
							)
						);

						append_element(
							ir_lines,
							make_instruction_popl(
								arg2,
								NULL
							)
						);
					
						append_element(data_lines,st);
					}else if( st->value.allocateS.opt_length->kind == empty_OL_K ) {
							// do something else?
					}
					break;

				case SYMBOL_RECORD:

						if(get_length(data_lines) == 0)	{ // init heap counter
							append_element(
								ir_lines, 
								make_instruction_movl(
									make_argument_label("$heap"),
									make_argument_label("(heapNext)")		
									)
							);
						}

						char *address_of_id = calloc(strlen(st->value.
						allocateS.variable->value.id) + 4, sizeof(char));

						sprintf(address_of_id, "(%s)",st->value.
							allocateS.variable->value.id);

						append_element(
							ir_lines,
							make_instruction_movl(
								make_argument_label("$heapNext"),
								make_argument_label(address_of_id)
							)
						);

						arg1 = make_argument_constant(st->value.allocateS.
							opt_length->emptyLength); // length of-result



						arg2 = make_argument_tempregister(current_temporary++);

						append_element(
							ir_lines,
							make_instruction_pushl(
								arg2,
								NULL
							)
						);


						append_element(
							ir_lines,
							make_instruction_movl(
								arg1,
								arg2
							)
						);

						// type check maybe? but symboltype of variabe is SYMBOL_RECORD
						append_element(
							ir_lines,
							make_instruction_imul(
								make_argument_constant(WORDSIZE),
								ebx
							)
						);

						append_element(
							ir_lines, // add to the next pointer
							make_instruction_addl(
								ebx, 
								make_argument_label("(heapNext)")
							)
						);

						append_element(
							ir_lines,
							make_instruction_popl(
								arg2,
								NULL
							)
						);
				
						append_element(data_lines,st);
					break;

					default:
						break;
				}
			break;

		case while_S_K:

			tmp = getNextLabel();

			truewhilestring = calloc(MAXLABELSIZE,sizeof(char));
			endlabelstring = calloc(MAXLABELSIZE,sizeof(char));

			sprintf(truewhilestring, "whileStart%d", current_label);
			sprintf(endlabelstring, "whileEnd%d", current_label);

		
			append_element( // while-start label insert 
				ir_lines, 
				make_instruction_label(
					make_argument_label(
						truewhilestring
						), 
					NULL
				)
			);

			// evaluting expressions
			arg1 = IR_builder_expression(st->value.whileS.exp);
			
			append_element( //Compare evaluted expression with true
				ir_lines, 
				make_instruction_cmp(
					make_argument_constant(1),
					arg1
				)
			);
			
			append_element( // jump to end if while condition is false 
				ir_lines, 
				make_instruction_jne(
					endlabelstring
				)
			);			

			// generate code for statements
			IR_builder_statement(st->value.whileS.statement);
			
			append_element( // repeating statement jump
				ir_lines, 
				make_instruction_jmp(truewhilestring)
			);
			
			append_element( // insertion of while-end
				ir_lines, 
				make_instruction_label(
					make_argument_label(
						endlabelstring)
					, NULL
				)
			);

			current_label++;
			break;

		case statement_list_S_K:
			IR_builder_statement_list(st->value.statement_list);
			break;

		default:
		//	printf("%s\n", "GOT NOTHING");
			break;

	}
} 

ARGUMENT *IR_builder_opt_length ( OPT_LENGTH *oplen ) {
	return IR_builder_expression(oplen->value.exp);
}

//TODO: needs label
ARGUMENT *IR_builder_variable (VAR *var) {

	SYMBOL *symbol;
	ARGUMENT *arg;
	ARGUMENT *arg1;
	ARGUMENT *resultOfSubExp;
	ARGUMENT *address_of_id;
	SYMBOLTABLE *tmpTable;
	
	switch (var->kind){
		case id_V_K:
			symbol = getSymbol(var->symboltable, var->value.id);
			arg = make_argument_address(WORDSIZE*(symbol->offset));
			break;
		case indexing_V_K:
				resultOfSubExp = IR_builder_expression(var->value.indexingV.exp);
				arg = make_argument_tempregister(current_temporary++);

				append_element(ir_lines,
					make_instruction_incl(resultOfSubExp)
				);

				arg1 = make_argument_labelAddring(
							var->value.indexingV.variable->value.id, // arrays in records problem
							resultOfSubExp
						);
			return arg1;

			break;

		case dot_V_K:

				if((symbol = getSymbol(var->symboltable, var->value.dotV.variable->value.id)) != NULL){
					tmpTable = symbol->symboltype->child;
				}

				if((symbol = getSymbol(tmpTable, var->value.dotV.id)) != NULL){
					resultOfSubExp = make_argument_constant(symbol->offset);
				}

				arg = make_argument_tempregister(current_temporary++);

				append_element(ir_lines,
						make_instruction_movl(
							resultOfSubExp,
							arg
						)
				);

				address_of_id = make_argument_labelAddring(
							var->value.dotV.variable->value.id,
							arg
						);
			return address_of_id;
		default:
			fprintf(stderr, "Error: variable kind not supported\n" );
			arg = NULL; // this will probably
				// generate a seg fault in upper levels
			break;
	}
	
	return arg;

}

ARGUMENT *IR_builder_expression ( EXPRES *exp ) {
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

		case plus_E_K:
			instr = make_instruction_addl(argLeft, argRight);
			append_element(ir_lines, instr);
			return argRight; 

		case minus_E_K:
			instr = make_instruction_subl( argRight, argLeft);
			append_element(ir_lines, instr);
			return argLeft; 

		case times_E_K:
			instr = make_instruction_imul(argLeft, argRight);
			append_element(ir_lines, instr);
			return argRight;

		case divide_E_K:
			tmp = getNextLabel();

			char *zeroden = calloc(MAXLABELSIZE,sizeof(char));
			sprintf(zeroden, "zeroDen%d", tmp);
			ARGUMENT *zerodenarg = make_argument_label(zeroden);
			IR_INSTRUCTION *zerodenlabel = make_instruction_label(zerodenarg, NULL);

			ARGUMENT *zeroArg = make_argument_constant(0);

			IR_INSTRUCTION *cmpvszero = make_instruction_cmp(zeroArg, argRight);
			append_element(ir_lines, cmpvszero);

			IR_INSTRUCTION *notzero = make_instruction_jne(zeroden);
			append_element(ir_lines, notzero);

			ARGUMENT *exitARG1 = make_argument_constant(3);
			IR_INSTRUCTION *moveExitArg1 = make_instruction_movl(exitARG1, ebx);
			append_element(ir_lines, moveExitArg1);

			ARGUMENT *exitARG2 = make_argument_constant(1);
			IR_INSTRUCTION *moveExitArg2 = make_instruction_movl(exitARG2, eax);
			append_element(ir_lines, moveExitArg2);

			IR_INSTRUCTION *sysexit = make_instruction_intcode("0x80");
			append_element(ir_lines, sysexit);

			append_element(ir_lines, zerodenlabel);

			//HACK -------------------------

			IR_INSTRUCTION *hack1 = make_instruction_pushl(ebx, NULL);
			append_element(ir_lines, hack1);

			IR_INSTRUCTION *hack2 = make_instruction_movl(argRight, ebx);
			append_element(ir_lines, hack2);

			//------------------------------
			edxsave = make_instruction_pushl(edx, NULL); //Saving edx register
			append_element(ir_lines, edxsave);

			instr = make_instruction_xor(edx, edx); //Clear edx for DIV
			append_element(ir_lines, instr);

			IR_INSTRUCTION *instr1 = make_instruction_movl(argLeft, eax);
			append_element(ir_lines, instr1);

			IR_INSTRUCTION*instr2 = make_instruction_div(ebx, NULL);
			append_element(ir_lines, instr2);

			IR_INSTRUCTION *instr3 = make_instruction_movl(eax, argLeft);
			append_element(ir_lines, instr3);

			IR_INSTRUCTION *hack3 = make_instruction_popl(ebx, NULL);
			append_element(ir_lines, hack3);

			edxrestore = make_instruction_popl(edx, NULL);
			append_element(ir_lines, edxrestore); //Restoring edx register

			return argLeft;

		case booleq_E_K:
		case boolneq_E_K:
		case boolgreater_E_K:
		case boolless_E_K:
		case boolleq_E_K:
		case boolgeq_E_K:
			result = make_argument_tempregister(current_temporary++);

			char *booltruelabel = calloc(MAXLABELSIZE,sizeof(char));
			char *boolendlabel = calloc(MAXLABELSIZE,sizeof(char));

			sprintf(booltruelabel, "booOPtrue%d", current_label);
			truearg = make_argument_label(booltruelabel);
			IR_INSTRUCTION *truelabel = make_instruction_label(truearg, NULL);

			sprintf(boolendlabel, "boolOPend%d", current_label);
			endarg = make_argument_label(boolendlabel);
			IR_INSTRUCTION *endlabel = make_instruction_label(endarg, NULL);

			instr = make_instruction_cmp( argRight, argLeft);
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
					truejmp = make_instruction_jg(booltruelabel);
					break;

				case boolless_E_K:
					truejmp = make_instruction_jl(booltruelabel);
					break;

				case boolleq_E_K:
					truejmp = make_instruction_JLE(booltruelabel);
					break;

				case boolgeq_E_K:
					truejmp = make_instruction_JGE(booltruelabel);
					break;

				default:
					return argRight;
			}

			append_element(ir_lines, truejmp);

			//false
			IR_INSTRUCTION *falsesave = make_instruction_movl(make_argument_constant(0), result);
			append_element(ir_lines, falsesave);


			IR_INSTRUCTION *endjmp = make_instruction_jmp(boolendlabel);
			append_element(ir_lines, endjmp);

			//true
			append_element(ir_lines, truelabel);
			IR_INSTRUCTION *truesave = make_instruction_movl(make_argument_constant(1), result);
			append_element(ir_lines, truesave);

			append_element(ir_lines, endlabel);
			current_label++;
			return result;

		case booland_E_K:
			tmp = getNextLabel();

			char *andFalselabel = calloc(MAXLABELSIZE,sizeof(char));
			char *andEndlabel = calloc(MAXLABELSIZE,sizeof(char));

			sprintf(andFalselabel, "ANDfalse%d", tmp);
			falsearg = make_argument_label(andFalselabel);
			IR_INSTRUCTION *andFalseinstr = make_instruction_label(falsearg, NULL);

			sprintf(andEndlabel, "ANDend%d", tmp);
			endarg = make_argument_label(andEndlabel);
			IR_INSTRUCTION *andEndinstr = make_instruction_label(endarg, NULL);

			result = make_argument_tempregister(current_temporary++);

			ARGUMENT *cmpARG = make_argument_constant(1);

			IR_INSTRUCTION *cmp1 = make_instruction_cmp(cmpARG, argLeft);
			append_element(ir_lines, cmp1);

			IR_INSTRUCTION *jmpfalse = make_instruction_jne(andFalselabel);
			append_element(ir_lines, jmpfalse);

			IR_INSTRUCTION *cmp2 = make_instruction_cmp(cmpARG, argRight);
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
		case boolor_E_K:
			tmp = getNextLabel();

			char *orOKlabel = calloc(MAXLABELSIZE,sizeof(char));
			char *orEndlabel = calloc(MAXLABELSIZE,sizeof(char));

			sprintf(orOKlabel, "ORtrue%d", tmp);
			truearg = make_argument_label(orOKlabel);
			IR_INSTRUCTION *oroklabel = make_instruction_label(truearg, NULL);

			sprintf(orEndlabel, "ORend%d", tmp);
			endarg = make_argument_label(orEndlabel);
			IR_INSTRUCTION *orendlabel = make_instruction_label(endarg, NULL);

			result = make_argument_tempregister(current_temporary++);

			ARGUMENT *cmporARG = make_argument_constant(1);

			IR_INSTRUCTION *cmpor1 = make_instruction_cmp(cmporARG, argLeft);
			append_element(ir_lines, cmpor1);

			IR_INSTRUCTION *jmpOK = make_instruction_je(orOKlabel);
			append_element(ir_lines, jmpOK);

			IR_INSTRUCTION *cmpor2 = make_instruction_cmp(cmporARG, argRight);
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

ARGUMENT *IR_builder_term ( TERM *term) {

	ARGUMENT *arg1;
	ARGUMENT *arg2;
	IR_INSTRUCTION *instr;
	SYMBOL *symbol;
	int params;
	int tmp;
	char *endlabelstring;
	char *falselabel;

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

		case null_T_K:
		case boolFalse_T_K:
			arg1 = make_argument_constant(0);
			arg2 = make_argument_tempregister(current_temporary++);
			instr = make_instruction_movl(arg1, arg2);
			append_element(ir_lines, instr);
			return arg2; //Return arg2 to keep track of temps

		case expresParent_T_K: // paranteses just parses
			return IR_builder_expression(term->value.exp);

		case var_T_K:
			arg1 = IR_builder_variable(term->value.var);
			arg2 = make_argument_tempregister(current_temporary++);
			instr = make_instruction_movl(arg1, arg2);
			append_element(ir_lines, instr);
			return arg2; //Return arg2 to keep track of temps			

		case actList_T_K:
			// static link ?
			symbol = getSymbol(term->symboltable, term->value.actlistT.id);
			params = 0;

			EXP_LIST *args = term->value.actlistT.actlist->value.exp_list;

			while(params < symbol->noArguments && args != NULL){

				if(args->kind == exp_EL_K){
					arg1 = IR_builder_expression(args->value.exp);
					instr = make_instruction_pushl(arg1, NULL);
					append_element(ir_lines, instr);
				} else if(args->kind == commalist_EL_K) {
					arg1 = IR_builder_expression(args->value.commaEL.exp);
					instr = make_instruction_pushl(arg1, NULL);
					append_element(ir_lines, instr);
				}
				args = args->value.commaEL.explist;
				params++;
			}

			arg2 = make_argument_label(symbol->uniquename);
			IR_INSTRUCTION *call = make_instruction_call(NULL, arg2);
			append_element(ir_lines, call);
			//function paramerters are useless after function call
			moveStackpointer(symbol->noArguments);

			//Handle return value as it can sit in eax
			ARGUMENT *returnarg = make_argument_tempregister(current_temporary++);
			IR_INSTRUCTION *savereturn = make_instruction_movl(eax, returnarg);
			append_element(ir_lines, savereturn);
			return returnarg; // by convention eax holds return values

		case termBang_T_K:
			tmp = getNextLabel();
			arg1 = IR_builder_term(term->value.term);

			falselabel = calloc(MAXLABELSIZE,sizeof(char));
			endlabelstring = calloc(MAXLABELSIZE,sizeof(char));

			sprintf(falselabel,"negf%d",tmp);
			sprintf(endlabelstring, "nege%d",tmp);

			append_element( // compaire with zero(false)
				ir_lines,
				make_instruction_cmp( 
					make_argument_constant(0),
					arg1
					)
				);

			append_element( // goto false label if it's not zero
				ir_lines,
				make_instruction_jne(
					falselabel
					)
				);

			append_element( // since it's zero add 1 to negate
				ir_lines,
				make_instruction_addl(
					make_argument_constant(1),
					arg1
					)
				);

			append_element( // get to the end of the negate statement
				ir_lines,
				make_instruction_jmp(
					endlabelstring
					)
				);

			append_element( // here goes the false label
				ir_lines,
				make_instruction_label(
					make_argument_label(falselabel),
					NULL
					)
				);

			append_element( // since it's one make it zero
				ir_lines,
				make_instruction_xor(
					arg1,
					arg1
					)
				);

			append_element( // this is end (label) !
				ir_lines,
				make_instruction_label(
					make_argument_label(endlabelstring),
					NULL
					)
				);
			
			return arg1;

		case expresPipes_T_K:
			params = 0;
			char *pipeEnd = calloc(MAXLABELSIZE,sizeof(char));
			sprintf(pipeEnd, "pipeEnd%d", getNextLabel());
			ARGUMENT *pipeArg = make_argument_label(pipeEnd);
			IR_INSTRUCTION *pipeLabel = make_instruction_label(pipeArg, NULL);

			arg1 = IR_builder_expression(term->value.exp);
			if(term->symboltype->type == SYMBOL_INT){
				//2 cases, positive numbers and negative numbers
				ARGUMENT *pipeCMPArg = make_argument_constant(0);

				IR_INSTRUCTION *pipeCMP = make_instruction_cmp(pipeCMPArg, arg1);
				append_element(ir_lines, pipeCMP);
				IR_INSTRUCTION *pipeEndJmp = make_instruction_JGE(pipeEnd);
				append_element(ir_lines, pipeEndJmp);
				IR_INSTRUCTION *negl = make_instruction_negl(arg1);
				append_element(ir_lines, negl);
				append_element(ir_lines, pipeLabel);
				return arg1;
			}else if(term->symboltype->type == SYMBOL_ARRAY){
				char *id = term->value.exp->value.term->value.var->value.id;

				ARGUMENT *firstElement = make_argument_tempregister(
					current_temporary++);

				ARGUMENT *zeroElement = make_argument_tempregister(
					current_temporary++);

				append_element( // xored to get zero
					ir_lines,
					make_instruction_xor(
						zeroElement,
						zeroElement
					)
				);

				append_element( // move from first place in array to reg
					ir_lines,
					make_instruction_movl(
						make_argument_labelAddring(
							id, // label to pointer to heap
							zeroElement // index
						),
						firstElement // result
					)
				);
				return firstElement;
			}
		//	printf("%s\n", "NOTHING");
		default:
			//printf("%s\n", "DEFAULT");
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

/* Adding allocation of local variables, this is by convention 
 *	a subtration of the stack pointer 
 */
IR_INSTRUCTION *localVariableAllocation() {
	if (local_variable_size > 0){
		IR_INSTRUCTION *instr = make_instruction_subl(
				make_argument_constant(local_variable_size),
				esp);
		append_element(ir_lines, instr);
		return instr; 
	 // reset the size counting	?
	}
	return NULL;
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

	ARGUMENT *arg = make_argument_constant(i * WORDSIZE);
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

//Very basic register allocation, round robin style
void basic_assign(linked_list *ir_lines){

	int count = 1;
	linked_list *temp;
	linked_list *save;
	temp = ir_lines->next;
	ARGUMENT *reg;
	IR_INSTRUCTION *instr1;
	IR_INSTRUCTION *instr2;


	while(temp != ir_lines){

		instr1 = (IR_INSTRUCTION *) temp->data;

		if(instr1->arg1 != NULL && instr1->arg1->kind == tempreg_arg){
			reg = get_register(count);
			save = temp;
			int cmp1 = instr1->arg1->tempid;

			while(temp != ir_lines){

				instr2 = (IR_INSTRUCTION *) temp->data;

				if(instr2->arg1 != NULL && instr2->arg1->kind == tempreg_arg){
					if(instr2->arg1->tempid == cmp1){

						instr2->arg1 = reg;
					}
				}
				//Arrays
				if(instr2->arg1 != NULL && instr2->arg1->kind == indexing_arg){
					if(instr2->arg1->index->tempid == cmp1){

						instr2->arg1->index = reg;
					}
				}

				if(instr2->arg2 != NULL && instr2->arg2->kind == tempreg_arg){
					if(instr2->arg2->tempid == cmp1){

						instr2->arg2 = reg;
					}
				}

                                //Arrays
                                if(instr2->arg2 != NULL && instr2->arg2->kind == indexing_arg){
                                        if(instr2->arg2->index->tempid == cmp1){

                                                instr2->arg2->index = reg;
                                        }
                                }

				temp = temp->next;
			} 
			if(++count > 5){
				count = 1;
			}
			instr1->arg1 = reg;
			temp = save;
		}

		if(instr1->arg2 != NULL && instr1->arg2->kind == tempreg_arg){
			reg = get_register(count);
			save = temp;
			int cmp2 = instr1->arg2->tempid;

			while(temp != ir_lines){
				instr2 = (IR_INSTRUCTION *) temp->data;

				if(instr2->arg1 != NULL && instr2->arg1->kind == tempreg_arg){

					if(instr2->arg1->tempid == cmp2){

						instr2->arg1 = reg;
					}
				}

                                //Arrays
                                if(instr2->arg1 != NULL && instr2->arg1->kind == indexing_arg){
                                        if(instr2->arg1->index->tempid == cmp2){

                                                instr2->arg1->index = reg;
                                        }
                                }


				if(instr2->arg2 != NULL && instr2->arg2->kind == tempreg_arg){

					if(instr2->arg2->tempid == cmp2){

						instr2->arg2 = reg;
					}
				}

                                //Arrays
                                if(instr2->arg2 != NULL && instr2->arg2->kind == indexing_arg){
                                        if(instr2->arg2->index->tempid == cmp2){

                                                instr2->arg2->index = reg;
                                        }
                                }


				temp = temp->next;
			} 
			if(++count > 5){
				count = 1;
			}
			instr1->arg2 = reg;
			temp = save;
		}
		

		temp = temp->next;
	}

}

ARGUMENT *get_register(int n){

	switch(n){
		case 0:
			return eax;
		break;
		case 1:
			return ebx;
		break;
		case 2:
			return ecx;
		break;
		case 3:
			return edx;
		break;
		case 4:
			return esi;
		break;
		case 5:
			return edi;
		break;
		default:
			if(n < 0 || n > 5){
				return NULL;
			}
		break;
	}
	return NULL;
}

void assign_instructionnumber(linked_list *ir_lines){

	linked_list *temp;
	temp = ir_lines->next;
	IR_INSTRUCTION *instr;

	while(temp != ir_lines){
		instr = (IR_INSTRUCTION *) temp->data;
		instr->id = instructionnumber++;
		temp = temp->next;
	}
}

// builds the data section at the end of the file
// cannot build in top because data_lines is not filled
void build_data_section(){

	append_element(ir_lines, 
	make_instruction_globl(make_argument_label(".data"),NULL));

	buildForm("formNUM:", ".string \"%d\\n\" ");
	buildForm("formTRUE:", ".string \"TRUE\\n\" ");
	buildForm("formFALSE:", ".string \"FALSE\\n\" ");
	buildForm("formNULL:", ".string \"NULL\\n\" ");

	if(get_length(data_lines) > 0){

		linked_list *temp;

		append_element(ir_lines,
			make_instruction_space(
				make_argument_label("heap"),
				make_argument_label("4194304") // allocate 4MB for heap
				)
			);
		append_element(ir_lines,
			make_instruction_space(
				make_argument_label("heapNext"),
				make_argument_label("4")
				)
			);

		temp = data_lines->next;

		while ( temp != data_lines ) { // making label pointers for allocated 
										// items
			STATEMENT *st = (STATEMENT *) temp->data;

			append_element(ir_lines,
				make_instruction_space(
					make_argument_label(st->value.allocateS.
						variable->value.id), 
					make_argument_label("4")
					)
				);
			temp = temp->next;
		}
		
		terminate_list(&data_lines);
	}
}
