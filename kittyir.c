#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "kittyir.h"
#include "irInstructions.h"

static int current_temporary = 1;
static int current_label = 0;
static int function_label = 0;
static int instruction_number = 0;

static linked_list *ir_lines; // plug IR code in here
static linked_list *data_lines; // for allocates

ARGUMENT *eax, *ebx, *ecx, *edx, *edi, *esi, *ebp, *esp;

void initRegisters() {
	eax = make_argument_register(r_eax, "eax");
	ebx = make_argument_register(r_ebx, "ebx");
	ecx = make_argument_register(r_ecx, "ecx");
	edx = make_argument_register(r_edx, "edx");
	edi = make_argument_register(r_edi, "edi");
	esi = make_argument_register(r_esi, "esi");
	ebp = make_argument_register(r_ebp, "ebp");
	esp = make_argument_register(r_esp, "esp");
}

int get_next_label() {
	return current_label++;
}

int get_next_function() {
	return function_label++;
}

void initStaticLink() {

	if (get_length(data_lines) == 0) { // init heap counter
		
		append_element(
			ir_lines,
			make_instruction_movl(
				make_argument_label("$heapNext"),
				make_argument_label("(staticLinks)")
			)
		);

		ARGUMENT *arg1 = make_argument_constant(function_label);
		ARGUMENT *arg2 = make_argument_temp_register(current_temporary++);

		append_element(ir_lines, make_instruction_movl(arg1, arg2));

		// type check maybe? but symbolType of variabe is SYMBOL_ARRAY
		append_element(
			ir_lines,
			make_instruction_imul(
				make_argument_constant(WORD_SIZE),
				arg2
			)
		);

		append_element(
			ir_lines, // add to the next pointer
			make_instruction_addl(
				arg2, 
				make_argument_label("(heapNext)")
			)
		);
	}
}

void addStaticLink( int id ) {

	ARGUMENT *place = make_argument_constant(id);

	append_element(ir_lines, make_instruction_pushl(ebx));

	append_element(ir_lines, make_instruction_movl(place, ebx));	

	append_element(ir_lines, 
		make_instruction_movl(ebp,
			make_argument_indexing( 
				make_argument_label("staticLinks"),ebx)));

	append_element(ir_lines,make_instruction_popl(ebx));
}


linked_list *IR_build( BODY *program ) {
	fprintf(stderr, "%s\n", "Initializing intermediate code generation");
	ir_lines = initialize_list();
	data_lines = initialize_list();
	initRegisters();

	int offsetCount = -1;

	SYMBOL *symbol;

	for ( int i = 0; i < HASH_SIZE; i++ ) {
		symbol = program->symboltable->table[i];

		if ( symbol != NULL && symbol->symbolType->type != SYMBOL_FUNCTION ) {
			symbol->offset = offsetCount--;
		}
	}

	// adding text section for completion
	append_element(ir_lines, make_instruction_directive(".text"));

	IR_builder_decl_list(program->decl_list);

	// make ".globl main" directive
	append_element(ir_lines, make_instruction_directive(".globl main"));

	// make "main:" label line
	append_element(ir_lines, make_instruction_label("main"));

	callee_start();
	callee_save();

	local_variable_allocation(program->symboltable);

	caller_save();
	initStaticLink();
	IR_builder_statement_list(program->statement_list);

	callee_restore();
	caller_restore();
	callee_end();

	append_element(ir_lines, make_instruction_movl(
		make_argument_constant(0),eax));

	program->symboltable->localVars = 0; // reseting local variables counter
	append_element(ir_lines, make_instruction_ret());

	build_data_section();

	linked_list *save = ir_lines;
	assign_instruction_number(ir_lines);
	basic_assign(ir_lines);
	ir_lines = save;

	repairMem(ir_lines);
	ir_lines = save;

	return ir_lines;
}

void IR_builder_function(FUNC *func) {

	int functionId = get_next_function();
	char *functionStartLabel = NEW_LABEL;
	char *functionEndLabel = NEW_LABEL;

	SYMBOL *symbol = getSymbol(func->symboltable, 
		func->head->id);

	sprintf(functionStartLabel, "func%d", functionId);
	sprintf(functionEndLabel,"endFunc%d", functionId);

	strcpy(symbol->uniqueName, functionStartLabel);

	sprintf(functionStartLabel, "%s", functionStartLabel);
	

	// move the handling of the declaration list here instead of the body to
	// avoid nested function getting generated inside each others 
	IR_builder_decl_list(func->body->decl_list);

	// start function label
	append_element(ir_lines, make_instruction_label(functionStartLabel));

	IR_builder_head(func->head);
	IR_builder_body(func->body);

	append_element( // end of function label
		ir_lines,
		make_instruction_label(functionEndLabel)
		);


	callee_restore();
	caller_restore();
	callee_end();
	func->symboltable->localVars = 0; // reset local variables in scope 

	append_element(ir_lines, make_instruction_ret());

}

void IR_builder_head (HEAD *header) {

	SYMBOL *symbol;
	SYMBOL *args = getSymbol(header->symboltable, header->id);
	int count = 0;
	int offset = 2; 

	VAR_DECL_LIST *vars = header->pdlist->var_decl_list;
	
	while ( count < args->noParameters) {

		if ( vars->var_type != NULL ) {

			symbol = getSymbol(header->pdlist->symboltable,
				vars->var_type->id);
			if ( symbol == NULL ) {
				fprintf(stderr, "%s\n", "Variable not found in symboltable");
				exit(1);
			}

			symbol->offset = offset;
			offset++;
			vars = vars->var_decl_list;
			count++;
		} else {
			break;
		}
	}
}

void IR_builder_body (BODY *body) {
	callee_start(); // shift in stackframe
 	addStaticLink(body->symboltable->id);
	callee_save();
	local_variable_allocation(body->symboltable);
	caller_save();

	IR_builder_statement_list(body->statement_list);
}

void IR_builder_var_decl_list ( VAR_DECL_LIST *vdecl) {
	switch(vdecl->kind){
		case VAR_DECL_LIST_LIST:
			IR_builder_var_decl_list(vdecl->var_decl_list);
			IR_builder_var_type(vdecl->var_type);
			break;
		case VAR_DECL_LIST_TYPE:
			IR_builder_var_type(vdecl->var_type);
			break;
	}
	
}

 void IR_builder_var_type ( VAR_TYPE * vtype ) {

	switch(vtype->type->kind){ // note: switching on type kind
		case TYPE_INT:
			vtype->symboltable->localVars += WORD_SIZE;
			break;
 
		case TYPE_BOOL:
			vtype->symboltable->localVars += WORD_SIZE;
			break;

		default:
			break;
	}
 }

void IR_builder_decl_list ( DECL_LIST *dlst ) {

	switch(dlst->kind){
		case DECL_LIST_LIST:
			IR_builder_decl_list(dlst->decl_list);
			IR_builder_declaration(dlst->declaration);
			break;
		case DECL_LIST_EMPTY:
			break;
	}

}

void IR_builder_declaration ( DECLARATION *decl ) {
	switch(decl->kind){
		case DECLARATION_ID:
			break;
		case DECLARATION_FUNC:
			IR_builder_function(decl->value.function);
			break;
		case DECLARATION_VAR:
			IR_builder_var_decl_list(decl->value.var_decl_list);
			break; 
	}
}

void IR_builder_statement_list ( STATEMENT_LIST *slst ) {
	switch(slst->kind) {
		case STATEMENT_LIST_LIST:
			IR_builder_statement_list(slst->statement_list);
			IR_builder_statement(slst->statement);
			break;
		case STATEMENT_LIST_STATEMENT:
			IR_builder_statement(slst->statement);
			break;
		default:
			break;
	}
}


void IR_builder_statement ( STATEMENT *st ) {
	int tempLabelCounter = 0;

	char *trueWhileString;
	char *endLabelString;
	char *elseLabel;
	char *printLabel;
	char *falseLabel;
	char *trueLabel;
	char *address_of_id;

	ARGUMENT *returnValue;
	ARGUMENT *arg1;
	ARGUMENT *arg2;
	ARGUMENT *arg3;

	IR_INSTRUCTION *params;
	IR_INSTRUCTION *call;

	switch(st->kind){

		case STATEMENT_RETURN:	
			returnValue = IR_builder_expression(st->value.exp);
			append_element(ir_lines, make_instruction_movl(returnValue, eax));
			break;

		case STATEMENT_WRITE: 
			switch(st->value.exp->symboltype->type){

				case SYMBOL_BOOL:
					
					// generating argument for boolean expression
					arg1 = IR_builder_expression(st->value.exp);

					tempLabelCounter = get_next_label();

					falseLabel = NEW_LABEL;
					trueLabel = NEW_LABEL;
					printLabel = NEW_LABEL;

					sprintf(falseLabel,"bfalse%d", tempLabelCounter);
					sprintf(trueLabel, "btrue%d", tempLabelCounter);
					sprintf(printLabel,"printbool%d", tempLabelCounter);

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
								falseLabel
							)
						);

					append_element(ir_lines, // making a push to the stack
											// with result of expression
						make_instruction_pushl(
							arg1
							)
						);

					append_element( // true case here
						ir_lines,
						make_instruction_pushl(
							make_argument_label("$formTRUE")
							)
						);

					append_element( // jmp to printf call 
						ir_lines,
						make_instruction_jmp(
								printLabel
							)
						);

					append_element( // false label section
						ir_lines,
						make_instruction_label(falseLabel)
						);

					append_element(ir_lines, // making a push to the stack
											// with result of expression
						make_instruction_pushl(arg1)
						);

					append_element( // false case here
						ir_lines,
						make_instruction_pushl(
							make_argument_label("$formFALSE")
							)
						);

					append_element( // printing section
						ir_lines,
						make_instruction_label(printLabel)
						);

					append_element( // call to print
						ir_lines,
						make_instruction_call(
							make_argument_label("printf")
							)
						);

					add_to_stack_pointer(2); // clean up
					caller_restore();

					break;

				case SYMBOL_INT:
				case SYMBOL_NULL:
					//Push arguments for print then form for print
					caller_save();
					arg1 = IR_builder_expression(st->value.exp);

					params = make_instruction_pushl(arg1);
					append_element(ir_lines, params);
		
					if (st->value.exp->value.term->kind == TERM_NULL){
						arg3 = make_argument_label("$formNULL");
						IR_INSTRUCTION *pushform = 
							make_instruction_pushl(arg3);
						append_element(ir_lines, pushform);
					} else {
						arg3 = make_argument_label("$formNUM");
						IR_INSTRUCTION *pushform = 
							make_instruction_pushl(arg3);
						append_element(ir_lines, pushform);
					}

					arg2 = make_argument_label("printf");
					call = make_instruction_call(arg2);
					append_element(ir_lines, call);

					add_to_stack_pointer(2);
					caller_restore();

					break;

				case SYMBOL_ARRAY:
					caller_save();
					arg1 = IR_builder_expression(st->value.exp);

					append_element(
						ir_lines,
						make_instruction_pushl(
							arg1
						)
					);

					append_element(ir_lines, make_instruction_pushl(
							make_argument_label("$formNUM")));

					append_element(ir_lines, make_instruction_call(
							make_argument_label("printf")));

					add_to_stack_pointer(2);
					caller_restore();
					break;
				default:
					break;
			}
			break;

		case STATEMENT_ASSIGN:  // todo with local variables
			switch(st->value.statement_assign.var->kind) {
				case VAR_ARRAY:
				case VAR_RECORD:

					arg1 = IR_builder_expression(st->value.
							statement_assign.exp);
					arg2 = IR_builder_variable(st->value.
							statement_assign.var);

					append_element(
							ir_lines,
							make_instruction_movl(
									arg1,
									arg2
							)
					);
					break;

				case VAR_ID:
					arg1 = IR_builder_expression(st->value.
							statement_assign.exp);

					SYMBOL *symbol = getSymbol(st->symboltable, st->value
							.statement_assign.var->id);

					if ( symbol != NULL ) {

						if (symbol->tableId != st->symboltable->id) {

						append_element(ir_lines, make_instruction_pushl(ecx));

						append_element(ir_lines, make_instruction_movl(
											   make_argument_constant(
													   symbol->tableId),
											   ecx));

						append_element(ir_lines, make_instruction_movl(
											   make_argument_indexing(
													   make_argument_label(
															   "staticLinks"),
													   ecx), ebx));

						append_element(ir_lines, make_instruction_popl(ecx));

						arg2 = make_argument_static(
								WORD_SIZE * (symbol->offset)
						);

						append_element(
								ir_lines,
								make_instruction_movl(
										arg1,
										arg2
								)
						);
					} else if (symbol->offset == 0 &&
						   st->symboltable->localVars >= WORD_SIZE) {
						// assigning offsets in stack

							symbol->offset = -1 * (st->symboltable->
									localVars / WORD_SIZE);
							st->symboltable->localVars -= WORD_SIZE;
						}
						append_element( // actual assignment
								ir_lines,
								make_instruction_movl(
										arg1,
										make_argument_address(
												(symbol->offset * WORD_SIZE)
										)
								)
						);
					}
					break;
				}
			break;

		case STATEMENT_IFBRANCH:
			// generate code for boolean expression(s)
			arg1 = IR_builder_expression(st->value.statement_ifbranch.exp); 

			elseLabel = NEW_LABEL;
			endLabelString = NEW_LABEL;

			int labelno = get_next_label();

			sprintf(elseLabel, "else%d", labelno);
			sprintf(endLabelString, "endIf%d", labelno);

			append_element( //Comparison with "true" boolean value
				ir_lines, 
				make_instruction_cmp(
					make_argument_constant(1), 
					arg1
					) 
				);

			if(st->value.statement_ifbranch.opt_else->kind != OPT_ELSE_EMPTY){
				append_element( // if not equal goto else part
					ir_lines,
					make_instruction_jne(elseLabel)
				);
			} else {
				append_element( // if not equal goto end-of-if
					ir_lines,
					make_instruction_jne(endLabelString)
				);
			}

			IR_builder_statement(st->value.statement_ifbranch.statement);
				// build statements in if-case


			if(st->value.statement_ifbranch.opt_else->kind != OPT_ELSE_EMPTY){

				append_element( // we have to jump over 
									//else when if-case is true
					ir_lines, 
					make_instruction_jmp(
							endLabelString
						)
					);

				append_element( // make else-label
					ir_lines, 
					make_instruction_label(elseLabel)
				);

				IR_builder_statement( // build else statements
					st->value.statement_ifbranch.opt_else->statement);
			}


			IR_INSTRUCTION *test = make_instruction_label(endLabelString);
			append_element( // end-of-if label
				ir_lines, 
				test
			);
			break;

		case STATEMENT_ALLOCATE:

			switch(st->value.statement_allocate.var->symboltype->type){

				case SYMBOL_ARRAY:
					// assume that the checker has checked if "length of" is
					// present

					if (get_length(data_lines) == 0) {
						// init heap counter
						append_element(
							ir_lines,
							make_instruction_movl(
								make_argument_label("$heap"),
								make_argument_label("(heapNext)")
								)
						);
					}

					// put check out of memory here

					address_of_id = calloc(strlen(st->value.
					statement_allocate.var->id) + 4, sizeof(char));
					sprintf(address_of_id, "(%s)",st->value.
						statement_allocate.var->id);
						// address of array

					append_element( // allocate space to array
						ir_lines,
						make_instruction_movl(
							make_argument_label("$heapNext"),
							make_argument_label(address_of_id)
						)
					);

					arg2 = make_argument_temp_register( // grab a new reg
							current_temporary++);

					append_element( // store the value of the new reg
						ir_lines,
						make_instruction_pushl(
							arg2
						)
					);

					ARGUMENT *arraySize = make_argument_temp_register(
							current_temporary++); // grab a new reg

					append_element( // save the reg value
							ir_lines,
							make_instruction_pushl(
								arraySize
							)
					);

					append_element( // xored to get zero, aka the first index
						ir_lines,
						make_instruction_xor(
							arg2,
							arg2
						)
					);

					append_element( // get the array size in terms of elements
						ir_lines,
						make_instruction_movl(
								IR_builder_opt_length(st->value.
										statement_allocate.opt_length),
								arraySize
						)
					);

					append_element( // move the array size to the first index
						ir_lines,
						make_instruction_movl(
								arraySize,
							make_argument_indexing(
								make_argument_label(
									st->value.statement_allocate.var->id
									),
								arg2
							)
						)
					);

					append_element(
						ir_lines,
						make_instruction_incl(
								arraySize
						)
					);

					append_element( // getting array size in bytes
						ir_lines,
						make_instruction_imul(
							make_argument_constant(WORD_SIZE),
							arraySize
						)
					);

					append_element( // update the heap free pointer
						ir_lines,
						make_instruction_addl(
								arraySize,
							make_argument_label("(heapNext)")
						)
					);

					append_element(
						ir_lines,
						make_instruction_popl(
								arraySize
						)
					);
					append_element(
						ir_lines,
						make_instruction_popl(
							arg2
						)
					);
					// restore the used regs

					append_element(data_lines, st);
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

						address_of_id = calloc(strlen(st->value.
						statement_allocate.var->id) + 4, sizeof(char));

						sprintf(address_of_id, "(%s)",st->value.
							statement_allocate.var->id);

						append_element(
							ir_lines,
							make_instruction_movl(
								make_argument_label("$heapNext"),
								make_argument_label(address_of_id)
							)
						);
						/* Something wrong here TODO */
						arg1 = make_argument_constant(0);

						arg2 = make_argument_temp_register(
								current_temporary++);

						append_element(
							ir_lines,
							make_instruction_pushl(
								arg2
							)
						);

						append_element(
							ir_lines,
							make_instruction_movl(
								arg1,
								arg2
							)
						);

						// type check maybe? but symbolType
						// of variabe is SYMBOL_RECORD
						append_element(
							ir_lines,
							make_instruction_imul(
								make_argument_constant(WORD_SIZE),
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
								arg2
							)
						);
				
						append_element(data_lines,st);
					break;
				default:
					break;
				}
			break;

		case STATEMENT_WHILE:

			tempLabelCounter = get_next_label();

			trueWhileString = NEW_LABEL;
			endLabelString = NEW_LABEL;

			sprintf(trueWhileString, "whileStart%d", current_label);
			sprintf(endLabelString, "whileEnd%d", current_label);

		
			append_element( // while-start label insert 
				ir_lines, 
				make_instruction_label(trueWhileString)
			);

			// evaluating expressions
			arg1 = IR_builder_expression(st->value.statement_while.exp);

			append_element( //Compare evaluated expression with true
				ir_lines, 
				make_instruction_cmp(
					make_argument_constant(1),
					arg1
				)
			);
	
			append_element( // jump to end if while condition is false 
				ir_lines, 
				make_instruction_jne(
						endLabelString
				)
			);			

			// generate code for statements
			IR_builder_statement(st->value.statement_while.statement);
			
			append_element( // repeating statement jump
				ir_lines, 
				make_instruction_jmp(trueWhileString)
			);
			
			append_element( // insertion of while-end
				ir_lines, 
				make_instruction_label(endLabelString)
			);
			break;

		case STATEMENT_LISTS:
			IR_builder_statement_list(st->value.statement_list);
			break;

		default:
			break;
	}
} 

ARGUMENT *IR_builder_opt_length ( OPT_LENGTH *opt_length ) {
	return IR_builder_expression(opt_length->exp);
}

ARGUMENT *IR_builder_variable (VAR *var) {

	SYMBOL *symbol;
	ARGUMENT *arg;
	ARGUMENT *arg1;
	ARGUMENT *resultOfSubExp;
	ARGUMENT *address_of_id;
	SYMBOL_TABLE *tmpTable;
	
	switch ( var->kind ) {
		case VAR_ID:
			symbol = getSymbol(var->symboltable, var->id);
			arg = NULL;

			if( symbol != NULL ){

				if ( symbol->symbolType->type == SYMBOL_ARRAY ||
						symbol->symbolType->type == SYMBOL_RECORD ) {

					// They're on the heap so we just use labels
					arg = make_argument_label(var->id);

				} else if (symbol->tableId != var->symboltable->id) {
					// basically, if variable is not in current,
					// use static link

					append_element(ir_lines, make_instruction_pushl(ecx));
					append_element(ir_lines, make_instruction_movl(
							make_argument_constant(symbol->tableId), ecx));

					append_element(ir_lines,
								   make_instruction_movl(
										   make_argument_indexing(
												   make_argument_label(
														   "staticLinks"),
												   ecx ), ebx ));

					append_element(ir_lines, make_instruction_popl(ecx));

					arg = make_argument_static( WORD_SIZE *
														(symbol->offset) );
				} else {
					arg = make_argument_address( WORD_SIZE *
														 (symbol->offset) );
				}
			}
			return arg;
		case VAR_ARRAY:
			resultOfSubExp = IR_builder_expression(var->value.var_array.exp);

			arg = make_argument_temp_register(current_temporary++);

			append_element(ir_lines,
				make_instruction_movl(resultOfSubExp, arg));

			append_element(ir_lines,
				make_instruction_incl(arg));
				// increment since we use the first element as the size

			arg1 = make_argument_indexing(
						make_argument_label(
								var->value.var_array.var->id), arg );
					// return the indexing into the array

			return arg1;
		case VAR_RECORD:

			if((symbol = getSymbol(var->symboltable,
				var->value.var_record.var->id)) != NULL){
				tmpTable = symbol->symbolType->child;
				// get the inner scope of the record
			}

			if((symbol = getSymbol(tmpTable,
				var->value.var_record.id)) != NULL){
				arg1 = make_argument_constant(symbol->offset);
				// member index in the record as argument
			}

			arg = make_argument_temp_register(current_temporary++);

			append_element(ir_lines, make_instruction_movl(arg1, arg));

			address_of_id = make_argument_indexing(
					make_argument_label(var->value.var_record.var->id), arg);
				// get the index of the member

			return address_of_id;
	}

	return NULL;
}

ARGUMENT *IR_builder_expression ( EXPRES *exp ) {

	int tempLabelCounter = 0;
	ARGUMENT *argLeft;
	ARGUMENT *argRight;
	ARGUMENT *result;
	ARGUMENT *truth;

	if ( exp->kind != EXPRES_TERM ) {
		argLeft = IR_builder_expression(exp->value.sides.left);
		argRight = IR_builder_expression(exp->value.sides.right);
	}

	switch(exp->kind){
		case EXPRES_TERM: 
			return IR_builder_term(exp->value.term);

		case EXPRES_PLUS:
			result = make_argument_temp_register(current_temporary++);
			append_element(ir_lines,make_instruction_movl(argRight,result));
				// right hand side must be a register
 			append_element(ir_lines, make_instruction_addl(argLeft, result));
 			return result;

		case EXPRES_MINUS:
			result = make_argument_temp_register(current_temporary++);
			append_element(ir_lines,make_instruction_movl(argLeft,result));
			append_element(ir_lines, make_instruction_subl(argRight, result));
 			return result;

		case EXPRES_TIMES:
			result = make_argument_temp_register(current_temporary++);
			append_element(ir_lines,make_instruction_movl(argRight,result));
 			append_element(ir_lines, make_instruction_imul(argLeft, result));
 			return result;

		case EXPRES_DIVIDE:
			tempLabelCounter = get_next_label();

			char *notZeroDenominator = NEW_LABEL;
			sprintf(notZeroDenominator, "NotZeroDen%d", tempLabelCounter);

			append_element(ir_lines, make_instruction_pushl(ebx));
			append_element(ir_lines, make_instruction_movl(argRight, ebx));
				// save ebx and use it to hold the right hand

			append_element(ir_lines, make_instruction_cmp(
					make_argument_constant(0), ebx));
			append_element(ir_lines, make_instruction_jne(
					notZeroDenominator));
				// denominator has to be check if zero

			build_exception("0x0");

			append_element(ir_lines, make_instruction_label(
					notZeroDenominator));

			append_element(ir_lines, make_instruction_pushl(edx));
				// Saving edx register; contains modulo after division

			append_element(ir_lines, make_instruction_xor(edx, edx));
				// Clear edx for modulo

			append_element(ir_lines, make_instruction_pushl(eax));
			append_element(ir_lines, make_instruction_movl(argLeft, eax));
				// move nominator to eax

			append_element(ir_lines, make_instruction_div(ebx));
				// divide eax with eax to get result in eax

			append_element(ir_lines, make_instruction_movl(eax, argLeft));
				// move the result to argLeft that gets returned

			append_element(ir_lines, make_instruction_popl(eax));
			append_element(ir_lines, make_instruction_popl(edx));
			append_element(ir_lines, make_instruction_popl(ebx));
				// restore the saved registers

			return argLeft;

		case EXPRES_EQ:
		case EXPRES_NEQ:
		case EXPRES_GREATER:
		case EXPRES_LESS:
		case EXPRES_LEQ:
		case EXPRES_GEQ:
			result = make_argument_temp_register(current_temporary++);
			tempLabelCounter = get_next_label();

			char *boolTrueLabel = NEW_LABEL;
			char *boolEndLabel = NEW_LABEL;

			sprintf(boolTrueLabel, "booOPtrue%d", tempLabelCounter);
			IR_INSTRUCTION *trueLabel = make_instruction_label(boolTrueLabel);

			sprintf(boolEndLabel, "boolOPend%d", tempLabelCounter);
			IR_INSTRUCTION *endLabel = make_instruction_label(boolEndLabel);

			append_element(ir_lines,
						   make_instruction_cmp( argRight, argLeft));
				// compare both sides
	
			IR_INSTRUCTION *trueJump;

			switch(exp->kind){
				case EXPRES_EQ:
					trueJump = make_instruction_je(boolTrueLabel);
					break;

				case EXPRES_NEQ:
					trueJump = make_instruction_jne(boolTrueLabel);
					break;

				case EXPRES_GREATER:
					trueJump = make_instruction_jg(boolTrueLabel);
					break;

				case EXPRES_LESS:
					trueJump = make_instruction_jl(boolTrueLabel);
					break;

				case EXPRES_LEQ:
					trueJump = make_instruction_JLE(boolTrueLabel);
					break;

				case EXPRES_GEQ:
					trueJump = make_instruction_JGE(boolTrueLabel);
					break;

				default:
					return argRight;
			}

			append_element(ir_lines, trueJump);
				// the jump to true instruction

			append_element(ir_lines, make_instruction_movl(
					make_argument_constant(0), result));
			append_element(ir_lines, make_instruction_jmp(boolEndLabel));
				// the false case

			append_element(ir_lines, trueLabel);
			append_element(ir_lines, make_instruction_movl(
					make_argument_constant(1), result));
				// the true case

			append_element(ir_lines, endLabel);
			return result;

		case EXPRES_AND:
			tempLabelCounter = get_next_label();

			char *andFalseLabel = NEW_LABEL;
			char *andEndLabel = NEW_LABEL;
			sprintf(andFalseLabel, "ANDfalse%d", tempLabelCounter);
			sprintf(andEndLabel, "ANDend%d", tempLabelCounter);

			result = make_argument_temp_register(current_temporary++);

			truth = make_argument_constant(1);
			IR_INSTRUCTION *jumpToFalse = make_instruction_jne(andFalseLabel);

			append_element(ir_lines, make_instruction_cmp(truth, argLeft));
			append_element(ir_lines, jumpToFalse);
			append_element(ir_lines, make_instruction_cmp(truth, argRight));
			append_element(ir_lines, jumpToFalse);
				// check if both arguments evaluates to true

			append_element(ir_lines,make_instruction_movl(
					truth, result));
			append_element(ir_lines, make_instruction_jmp(andEndLabel));
				// in case both arguments are true

			append_element(ir_lines, make_instruction_label(andFalseLabel));
			append_element(ir_lines, make_instruction_movl(
					make_argument_constant(0), result));
				// in case one of the arguments are false

			append_element(ir_lines, make_instruction_label(andEndLabel));

			return result;
			break;
		case EXPRES_OR:
			tempLabelCounter = get_next_label();

			char *orTrueLabel = NEW_LABEL;
			char *orEndLabel = NEW_LABEL;
			sprintf(orTrueLabel, "ORtrue%d", tempLabelCounter);
			sprintf(orEndLabel, "ORend%d", tempLabelCounter);

			result = make_argument_temp_register(current_temporary++);
			truth = make_argument_constant(1);

			IR_INSTRUCTION *jumpToTrue = make_instruction_je(orTrueLabel);

			append_element(ir_lines, make_instruction_cmp(truth, argLeft));
			append_element(ir_lines, jumpToTrue);
			append_element(ir_lines,make_instruction_cmp(truth, argRight));
			append_element(ir_lines, jumpToTrue);
				// like in "and" we compare both arguments but jumps to true
				// case instead of false case

			append_element(ir_lines, make_instruction_movl(
					make_argument_constant(0), result));
			append_element(ir_lines, make_instruction_jmp(orEndLabel));
				// false case

			append_element(ir_lines, make_instruction_label(orTrueLabel));
			append_element(ir_lines, make_instruction_movl(
					truth, result));
				// true case

			append_element(ir_lines, make_instruction_label(orEndLabel));

			return result;
			break;
	}

	return argLeft;

}

ARGUMENT *IR_builder_term ( TERM *term) {

	ARGUMENT *subArg;
	ARGUMENT *firstElement;
	ARGUMENT *zeroIndex;
	ARGUMENT *result;
	SYMBOL *symbol;
	char *positiveNumberLabel;

	switch(term->kind){
		case TERM_NUM:
			return make_argument_constant(term->value.intconst);

		case TERM_TRUE:
			return make_argument_constant(1);

		case TERM_NULL:
		case TERM_FALSE:
			return make_argument_constant(0);

		case TERM_PARENTESES: // parentheses just parses
			return IR_builder_expression(term->value.exp);

		case TERM_VAR:
			return IR_builder_variable(term->value.var);

		case TERM_ACT_LIST:
			addStaticLink(term->symboltable->id);

			symbol = getSymbol(term->symboltable, term->value.
					term_act_list.id);

			// push functionParameters on stack recursively
			IR_builder_act_list(term->value.term_act_list.actlist);

			append_element(ir_lines, make_instruction_call(
					make_argument_label(symbol->uniqueName)));

			// stack clean up for function functionParameters
			add_to_stack_pointer(symbol->noParameters);

			//Handle return value as it can sit in eax
			ARGUMENT *returnArg = make_argument_temp_register(
					current_temporary++);

			append_element(ir_lines, make_instruction_movl(eax, returnArg));

			return returnArg; // by convention eax holds return values

		case TERM_NOT:

			subArg = IR_builder_term(term->value.term);

			result = make_argument_temp_register(current_temporary++);
			append_element(ir_lines,make_instruction_movl(subArg,result));
				// we need a new register to work on

			append_element(ir_lines,make_instruction_xor(
					make_argument_constant(1), result));
			// xor the term expression with true (1) to get the negated value

			return result;

		case TERM_ABS:
			subArg = IR_builder_expression(term->value.exp);

			if ( term->symboltype->type == SYMBOL_INT ) {

				positiveNumberLabel = NEW_LABEL;
				sprintf(positiveNumberLabel, "posNum%i", get_next_label());

				result = make_argument_temp_register(current_temporary++);
				append_element(ir_lines,make_instruction_movl(subArg,result));

				append_element(ir_lines,make_instruction_cmp(
						make_argument_constant(0),result));
				append_element(ir_lines,make_instruction_JGE(
						positiveNumberLabel));
				// jump to end if number is positive

				append_element(ir_lines,
							   make_instruction_negl(result));
				// false negates the argument as two's complement

				append_element(ir_lines,
							   make_instruction_label(positiveNumberLabel));
				return result;

			} else if ( term->symboltype->type == SYMBOL_ARRAY ) {

				firstElement = make_argument_temp_register(
						current_temporary++);

				zeroIndex = make_argument_temp_register(
						current_temporary++);

				append_element( ir_lines, make_instruction_xor(zeroIndex,
															   zeroIndex));

				append_element( ir_lines, make_instruction_movl(
						make_argument_indexing(subArg, zeroIndex),
						firstElement));
					// gets the first element of the array where the size is
					// stored

				return firstElement;
			}
		default:
			break;
	}

	return NULL;
}

void IR_builder_act_list ( ACT_LIST *actList ) {

	switch(actList->kind){
		case ACT_LIST_EXPLIST:
			IR_builder_expression_list( actList->exp_list );
			break;
		case ACT_LIST_EMPTY:
			break;
	}
}

/* Since expression_list is used only by act list we can push function
 * functionParameters from here
 */
void IR_builder_expression_list ( EXP_LIST *expList ) {

	switch(expList->kind){
		case EXP_LIST_EXP:
			append_element( ir_lines, make_instruction_pushl(
					IR_builder_expression(expList->exp)));
			break;
		case EXP_LIST_LIST:
		
			IR_builder_expression_list(expList->explist);
			append_element( ir_lines, make_instruction_pushl(
					IR_builder_expression(expList->exp)));
			break;
	}
}

/* Adding allocation of local variables, this is by convention 
 *	a subtraction of the stack pointer
 */
IR_INSTRUCTION *local_variable_allocation(SYMBOL_TABLE *currentScope) {
	if (currentScope->localVars > 0){
		IR_INSTRUCTION *instr = make_instruction_subl(
				make_argument_constant(currentScope->localVars),
				esp);
		append_element(ir_lines, instr);
		return instr; 
		// reset counter at end of function
	}
	return NULL;
}

/*
 * Handy code sippet for throwing exceptions
 */
void build_exception(char *intCode) {
	append_element(ir_lines,make_instruction_movl(
			make_argument_constant(3), ebx));

	append_element(ir_lines, make_instruction_movl(
			make_argument_constant(1), eax));

	append_element(ir_lines, make_instruction_intcode(intCode));
}

void caller_save(){

	IR_INSTRUCTION *instr1 = make_instruction_pushl(ecx);
	IR_INSTRUCTION *instr2 = make_instruction_pushl(edx);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);

}

void caller_restore(){

	IR_INSTRUCTION *instr1 = make_instruction_popl(edx);
	IR_INSTRUCTION *instr2 = make_instruction_popl(ecx);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);

}

void callee_start(){

	IR_INSTRUCTION *instr1 = make_instruction_pushl(ebp);
	IR_INSTRUCTION *instr2 = make_instruction_movl(esp, ebp);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);

}

void callee_end(){

	IR_INSTRUCTION *instr1 = make_instruction_movl(ebp, esp);
	IR_INSTRUCTION *instr2 = make_instruction_popl(ebp);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);

}

void callee_save(){

	IR_INSTRUCTION *instr1 = make_instruction_pushl(ebx);
	IR_INSTRUCTION *instr2 = make_instruction_pushl(esi);
	IR_INSTRUCTION *instr3 = make_instruction_pushl(edi);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);
	append_element(ir_lines, instr3);
}

void callee_restore(){

	IR_INSTRUCTION *instr1 = make_instruction_popl(ebx);
	IR_INSTRUCTION *instr2 = make_instruction_popl(esi);
	IR_INSTRUCTION *instr3 = make_instruction_popl(edi);
	append_element(ir_lines, instr3);
	append_element(ir_lines, instr2);
	append_element(ir_lines, instr1);

}

void add_to_stack_pointer(int i){

	ARGUMENT *arg = make_argument_constant(i * WORD_SIZE);
	IR_INSTRUCTION *instr = make_instruction_addl(arg, esp);
	append_element(ir_lines, instr); 

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

		if(instr1->arg1 != NULL && instr1->arg1->kind == tempReg_arg){
			reg = get_register(count);
			save = temp;
			int cmp1 = instr1->arg1->temp_id;

			while(temp != ir_lines){

				instr2 = (IR_INSTRUCTION *) temp->data;

				if(instr2->arg1 != NULL && instr2->arg1->kind ==
										   tempReg_arg){
					if(instr2->arg1->temp_id == cmp1){

						instr2->arg1 = reg;
					}
				}
				//Arrays
				if(instr2->arg1 != NULL && instr2->arg1->kind == 
						indexing_arg){
					if(instr2->arg1->index->temp_id == cmp1){

						instr2->arg1->index = reg;
					}
				}

				if(instr2->arg2 != NULL && instr2->arg2->kind ==
										   tempReg_arg){
					if(instr2->arg2->temp_id == cmp1){

						instr2->arg2 = reg;
					}
				}

                //Arrays
                if(instr2->arg2 != NULL && instr2->arg2->kind == 
                		indexing_arg){
                        if(instr2->arg2->index->temp_id == cmp1){

                                instr2->arg2->index = reg;
                        }
                }

				temp = temp->next;
			} 
			if(++count > 4){
				count = 1;
			}
			instr1->arg1 = reg;
			temp = save;
		}

		if(instr1->arg2 != NULL && instr1->arg2->kind == tempReg_arg){
			reg = get_register(count);
			save = temp;
			int cmp2 = instr1->arg2->temp_id;

			while(temp != ir_lines){
				instr2 = (IR_INSTRUCTION *) temp->data;

				if(instr2->arg1 != NULL && instr2->arg1->kind == tempReg_arg){

					if(instr2->arg1->temp_id == cmp2){

						instr2->arg1 = reg;
					}
				}

                //Arrays
                if(instr2->arg1 != NULL && instr2->arg1->kind == 
                		indexing_arg){
                        if(instr2->arg1->index->temp_id == cmp2){

                                instr2->arg1->index = reg;
                        }
                }


				if(instr2->arg2 != NULL && instr2->arg2->kind == tempReg_arg){

					if(instr2->arg2->temp_id == cmp2){

						instr2->arg2 = reg;
					}
				}

                //Arrays
                if(instr2->arg2 != NULL && instr2->arg2->kind == 
                		indexing_arg){
                        if(instr2->arg2->index->temp_id == cmp2){

                                instr2->arg2->index = reg;
                        }
                }

				temp = temp->next;
			} 
			if(++count > 4){
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
		case 1:
			return ebx;
		case 2:
			return ecx;
		case 3:
			return edx;
		case 4:
			return esi;
		case 5:
			return edi;
		default:
			if(n < 0 || n > 5){
				return NULL;
			}
		break;
	}
	return NULL;
}

void assign_instruction_number(linked_list *ir_lines){

	linked_list *temp;
	temp = ir_lines->next;
	IR_INSTRUCTION *instr;

	while(temp != ir_lines){
		instr = (IR_INSTRUCTION *) temp->data;
		instr->id = instruction_number++;
		temp = temp->next;
	}
}

// builds the data section at the end of the file
// cannot build in top because data_lines is not filled
void build_data_section() {

	append_element(ir_lines, make_instruction_directive(".data"));
	append_element(ir_lines, 
		make_instruction_directive("formNUM: \n\t.string \"%d\\n\" ")
		);

	append_element(ir_lines, 
		make_instruction_directive("formTRUE: \n\t.string \"TRUE\\n\" ")
	);

	append_element(ir_lines, 
		make_instruction_directive("formFALSE: \n\t.string \"FALSE\\n\" ")
	);

	append_element(ir_lines, 
		make_instruction_directive("formNULL: \n\t.string \"NULL\\n\" ")
	);


		// if there is allocation to the heap or a function is declared
		// (need heap for the static link)
		append_element(ir_lines,
			make_instruction_space(
				make_argument_label("heap"),
				make_argument_label("4194304")
				)
			);

		append_element(ir_lines,
			make_instruction_space(
				make_argument_label("heapNext"),
				make_argument_label("4")
				)
			);			

		// make static link pointer (pointer to a array of static links)
		append_element(ir_lines,
		make_instruction_space(
			make_argument_label("staticLinks"),
			make_argument_label("4")
			)
		);	
	
	
	if ( get_length(data_lines) > 0 ) {
		// make pointers to records / arrays in heap

		linked_list *temp;
		temp = data_lines->next;

		while ( temp != data_lines ) { // making label pointers for allocated 
										// items
			STATEMENT *st = (STATEMENT *) temp->data;

			append_element(ir_lines,
				make_instruction_space(
					make_argument_label(st->value.statement_allocate.
						var->id), 
					make_argument_label("4")
					)
				);
			temp = temp->next;
		}
		
		terminate_list(&data_lines);
	}
}


void repairMem(linked_list *ir_lines){

	linked_list *temp;
	temp = ir_lines->next;
	IR_INSTRUCTION *instr1;

	while(temp != ir_lines){

		instr1 = (IR_INSTRUCTION *) temp->data;

		if(instr1->op_code == movl){
			switch(instr1->arg2->kind){

				case address_arg:
				case constant_arg:
				case indexing_arg:
				case staticLink_arg:
					//remove line from code

					append_element(temp, make_instruction_pushl(edi));
					append_element(temp, 
						make_instruction_movl(instr1->arg1,edi));
					append_element(temp, 
						make_instruction_movl(edi,instr1->arg2));
					append_element(temp, make_instruction_popl(edi));

					temp->previous->next = temp->next;
					temp->next->previous = temp->previous;

				default:
					break;

			}
		}

		if(instr1->op_code == cmp){
			switch(instr1->arg2->kind){

				case address_arg:
				case constant_arg:
				case indexing_arg:
				case staticLink_arg:
					//remove line from code

					append_element(temp, make_instruction_pushl(edi));
					append_element(temp, 
						make_instruction_movl(instr1->arg2, edi));
					append_element(temp, 
						make_instruction_cmp(instr1->arg1, edi));
					append_element(temp, make_instruction_popl(edi));

					temp->previous->next = temp->next;
					temp->next->previous = temp->previous;

				default:
					break;
				}
		}

		if(instr1->op_code == addl){
			switch(instr1->arg2->kind){

				case address_arg:
				case constant_arg:
				case indexing_arg:
				case staticLink_arg:
					//remove line from code
					append_element(temp, make_instruction_pushl(edi));
					append_element(temp, 
						make_instruction_movl(instr1->arg2, edi));
					append_element(temp, 
						make_instruction_addl(instr1->arg1, edi));
					append_element(temp, 
						make_instruction_movl(edi, instr1->arg2));
					append_element(temp, make_instruction_popl(edi));

					temp->previous->next = temp->next;
					temp->next->previous = temp->previous;

				default:
					break;
				}
		}

		if(instr1->op_code == imul){
			switch(instr1->arg2->kind){

				case address_arg:
				case constant_arg:
				case indexing_arg:
				case staticLink_arg:
					//remove line from code
					append_element(temp, make_instruction_pushl(edi));
					append_element(temp, 
						make_instruction_movl(instr1->arg2, edi));
					append_element(temp, 
						make_instruction_imul(instr1->arg1, edi));
					append_element(temp, 
						make_instruction_movl(edi, instr1->arg2));
					append_element(temp, make_instruction_popl(edi));

					temp->previous->next = temp->next;
					temp->next->previous = temp->previous;

				default:
					break;
				}
		}

		temp = temp->next;
	}
}
