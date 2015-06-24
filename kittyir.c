#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "kittyir.h"
#include "irInstructions.h"
#include "kittyemit.h"

static int current_temporary = 1;
static int current_label = 0;
static int function_label = 0;
static int instructionnumber = 0;

static linked_list *ir_lines; // plug IR code in here
static linked_list *data_lines; // for allocates

extern SYMBOLTABLE *globalTable;

struct SYMBOLTABLE *currentTable;

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

void initStaticLink(){

	if(get_length(data_lines) == 0)	{ // init heap counter
		
		append_element(
			ir_lines,
			make_instruction_movl(
				make_argument_label("$heapNext"),
				make_argument_label("(staticLinks)")
			)
		);

		ARGUMENT *arg1 = make_argument_constant(function_label);
		ARGUMENT *arg2 = make_argument_tempregister(current_temporary++);

		append_element(
			ir_lines,
			make_instruction_movl(
				arg1,
				arg2
			)
		);

		// type check maybe? but symboltype of variabe is SYMBOL_ARRAY 
		append_element(
			ir_lines,
			make_instruction_imul(
				make_argument_constant(WORDSIZE),
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

void addStaticLink(int id){

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

	SYMBOLTABLE *symboltable = program->symboltable;

	int offsetcount = -1;

	SYMBOL *symbol;

	for(int i = 0; i < HASH_SIZE; i++){
		symbol = symboltable->table[i];

		if(symbol != NULL && symbol->symboltype->type != SYMBOL_FUNCTION){
			symbol->offset = offsetcount--;
		}
	}

	char *mainLabel = calloc(MAXLABELSIZE,sizeof(char));
	sprintf(mainLabel, "%s", "main");

	globalTable = symboltable;

	// adding text section for completion
	append_element(ir_lines, make_instruction_directive(".text"));

	IR_builder_decl_list(program->decl_list);

	// make ".globl main" directive
	append_element(ir_lines, make_instruction_directive(".globl main"));

	// make "main:" label line

	IR_INSTRUCTION *globl_main = make_instruction_label(mainLabel);
	append_element(ir_lines, globl_main);

	calleeStart();
	calleeSave();

	localVariableAllocation(program->symboltable);


	callerSave();
	initStaticLink();
	IR_builder_statement_list(program->statement_list);
	
	calleeRestore();
	callerRestore();
	calleeEnd();

	append_element(ir_lines, make_instruction_movl(
		make_argument_constant(0),eax));

	program->symboltable->localVars = 0; // reseting local variables counter
	append_element(ir_lines, make_instruction_ret());

	build_data_section();

	linked_list *save = ir_lines;
	assign_instructionnumber(ir_lines);
	basic_assign(ir_lines);
	ir_lines = save;

	repairMem(ir_lines);
	ir_lines = save;
	IR_printer(ir_lines);
	
	return ir_lines;
}

void IR_builder_function(FUNC *func) {

	int functiondId = getNextFunction();
	char *functionlabel = calloc(MAXLABELSIZE ,sizeof(char));
	char *functionendlabel = calloc(MAXLABELSIZE ,sizeof(char));

	SYMBOL *symbol = getSymbol(func->symboltable, 
		func->head->id);

	sprintf(functionlabel, "func%d", functiondId);
	sprintf(functionendlabel,"endFunc%d", functiondId);

	strcpy(symbol->uniquename, functionlabel);

	sprintf(functionlabel, "%s", functionlabel);
	

	// move the handling of the declaration list here instead of the body to
	// avoid nested function getting generated inside each others 
	IR_builder_decl_list(func->body->decl_list);

	IR_INSTRUCTION *func_main = // start function label
		make_instruction_label( functionlabel );

	append_element(ir_lines, func_main);


	IR_builder_head(func->head);
	IR_builder_body(func->body);

	append_element( // end of function label
		ir_lines,
		make_instruction_label( functionendlabel )
		);


	calleeRestore();
	callerRestore();
	calleeEnd();
	func->symboltable->localVars = 0; // reset local variables in scope 

	append_element(ir_lines, make_instruction_ret());

}

void IR_builder_head (HEAD *header) {

	SYMBOL *symbol;
	SYMBOL *args = getSymbol(header->symboltable, header->id);
	int count = 0;
	int offset = 2; 

	VAR_DECL_LIST *vars = header->pdlist->var_decl_list;
	
	while(count < args->noArguments){

		if(globalTable != NULL || vars->var_type != NULL){
		
			if (vars->kind == VAR_DECL_LIST_LIST) {
				symbol = getSymbol(header->pdlist->symboltable, 
					vars->var_type->id);
			} else {
				symbol = getSymbol(header->pdlist->symboltable, 
					vars->var_type->id);
			}

			if(symbol == NULL){
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
	SYMBOLTABLE *temptable = currentTable;
	currentTable = body->symboltable;
 	calleeStart(); // shift in stackframe
 	addStaticLink(body->symboltable->id);
	calleeSave();
	localVariableAllocation(body->symboltable);
	callerSave();

	IR_builder_statement_list(body->statement_list);
	currentTable = temptable;
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
			vtype->symboltable->localVars += WORDSIZE;
			break;
 
		case TYPE_BOOL:
			vtype->symboltable->localVars += WORDSIZE;
			break;

		case TYPE_ARRAY:
			// vtype->symboltable->localVars += WORDSIZE; 
			// parse by reference but we have to find out if it's parameters 
			break;
		case TYPE_RECORD:
			// vtype->symboltable->localVars += WORDSIZE; // like arrays
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
	int tmp = 0; 

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

	IR_INSTRUCTION *params;
	IR_INSTRUCTION *call;

	switch(st->kind){

		case STATEMENT_RETURN:	
			returnvalue = IR_builder_expression(st->value.exp);
			IR_INSTRUCTION *movl = make_instruction_movl(returnvalue, eax);
			append_element(ir_lines, movl);
			break;

		case STATEMENT_WRITE: 
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

					//callerSave();

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
								printlabel
							)
						);

					append_element( // false label section
						ir_lines,
						make_instruction_label(falselabel)
						);

					//callerSave();
					
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
						make_instruction_label(printlabel)
						);

					append_element( // call to print
						ir_lines,
						make_instruction_call(
							make_argument_label("printf")
							)
						);

					moveStackpointer(2); // clean up
					callerRestore();

					break;

				case SYMBOL_INT:
				case SYMBOL_NULL:
					//Push arguments for print then form for print
					callerSave();
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

					moveStackpointer(2);
					callerRestore();

					break;

				case SYMBOL_ARRAY:
					callerSave();
					arg1 = IR_builder_expression(st->value.exp);

					append_element(
						ir_lines,
						make_instruction_pushl(
							arg1
						)
					);

					arg3 = make_argument_label("$formNUM");
					IR_INSTRUCTION *pushform = make_instruction_pushl(arg3);
					append_element(ir_lines, pushform);
					arg2 = make_argument_label("printf");
					call = make_instruction_call(arg2);
					append_element(ir_lines, call);

					moveStackpointer(2);
					callerRestore();
					break;
				default:
					break;
			}
			break;

		case STATEMENT_ASSIGN:  // todo with local variables
			switch(st->value.statement_assign.var->kind){

				case VAR_ARRAY:
					arg2 = IR_builder_variable(st->value.
						statement_assign.var);
					arg1 = IR_builder_expression(st->value.
						statement_assign.exp);
					
						append_element( 
							ir_lines,
							make_instruction_movl(
								arg1,
								arg2
							)
						);
					break;

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

					SYMBOL *symbol = getSymbol(st->symboltable,st->value
						.statement_assign.var->id);
		
					if(symbol->tableid != st->symboltable->id){
						append_element(
							ir_lines, 
							make_instruction_pushl(ecx)
							);
						append_element(ir_lines, 
							make_instruction_movl(
								make_argument_constant(symbol->tableid)
								, ecx
								)
							);

						append_element(ir_lines, 
							make_instruction_movl(
								make_argument_indexing(
									make_argument_label(
										"staticLinks"),ecx
								), ebx
							)
						);


						append_element(ir_lines, 
							make_instruction_popl(ecx)
							);

						arg2 = make_argument_static(
							WORDSIZE*(symbol->offset)
							);

						append_element( 
							ir_lines,
							make_instruction_movl(
								arg1,
								arg2
							)
						);
					} else if( symbol->offset == 0 && 
						st->symboltable->localVars >= WORDSIZE 
						&& symbol != NULL ) { 
						// assigning offsets in stack
					
						symbol->offset = -1 * (st->symboltable->localVars / 
																	WORDSIZE);
						st->symboltable->localVars -= WORDSIZE;
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

		case STATEMENT_IFBRANCH:
			// generate code for boolean expression(s)
			arg1 = IR_builder_expression(st->value.statement_ifbranch.exp); 

			elselabel = calloc(MAXLABELSIZE,sizeof(char));
			endlabelstring = calloc(MAXLABELSIZE,sizeof(char));

			int labelno = getNextLabel();

			sprintf(elselabel, "else%d", labelno);
			sprintf(endlabelstring, "endIf%d", labelno);

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
					make_instruction_jne(elselabel) 
				);
			} else {
				append_element( // if not equal goto end-of-if
					ir_lines,
					make_instruction_jne(endlabelstring) 
				);
			}

			IR_builder_statement(st->value.statement_ifbranch.statement);
				// build statements in if-case


			if(st->value.statement_ifbranch.opt_else->kind != OPT_ELSE_EMPTY){

				append_element( // we have to jump over 
									//else when if-case is true
					ir_lines, 
					make_instruction_jmp(
						endlabelstring
						)
					);

				append_element( // make else-label
					ir_lines, 
					make_instruction_label(elselabel)
				);

				IR_builder_statement( // build else statements
					st->value.statement_ifbranch.opt_else->statement);
			}


			IR_INSTRUCTION *test = make_instruction_label(endlabelstring);			
			append_element( // end-of-if label
				ir_lines, 
				test
			);
			break;

		case STATEMENT_ALLOCATE:

			switch(st->value.statement_allocate.var->symboltype->type){

				case SYMBOL_ARRAY:
					if (st->value.statement_allocate.
						opt_length->kind == OPT_LENGTH_EXPRES) {
						
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

						arg1 = IR_builder_expression(st->value.
							statement_allocate.opt_length->exp); 
								// length of-result

						arg2 = make_argument_tempregister(
							current_temporary++);

						append_element(
							ir_lines,
							make_instruction_pushl(
								arg2
							)
						);

						ARGUMENT *movArg = make_argument_tempregister(
							current_temporary++);

						append_element(
							ir_lines,
							make_instruction_pushl(
								movArg
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
								movArg
							)
						);

						append_element(
							ir_lines,
							make_instruction_movl(
								movArg,
								make_argument_indexing(
									make_argument_label(
										st->value.statement_allocate.var->id
										),
									arg2
								)
							)
						);

						append_element( // first element is the length so incl
							ir_lines,
							make_instruction_incl(
								movArg
							)
						);

						// type check maybe? 
						// but symboltype of variabe is SYMBOL_ARRAY 
						append_element(
							ir_lines,
							make_instruction_imul(
								make_argument_constant(WORDSIZE),
								movArg
							)
						);

						append_element(
							ir_lines, // add to the next pointer
							make_instruction_addl(
								movArg, 
								make_argument_label("(heapNext)")
							)
						);

						append_element(
							ir_lines,
							make_instruction_popl(
								movArg
							)
						);

						append_element(
							ir_lines,
							make_instruction_popl(
								arg2
							)
						);
					
						append_element(data_lines,st);
					}else if( st->value.statement_allocate.
						opt_length->kind == OPT_LENGTH_EMPTY ) {
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
							//st->value.statement_allocate.
							//opt_length->emptyLength); // length of-result

						arg2 = make_argument_tempregister(
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

						// type check maybe? but symboltype 
						// of variabe is SYMBOL_RECORD
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

			tmp = getNextLabel();

			truewhilestring = calloc(MAXLABELSIZE,sizeof(char));
			endlabelstring = calloc(MAXLABELSIZE,sizeof(char));

			sprintf(truewhilestring, "whileStart%d", current_label);
			sprintf(endlabelstring, "whileEnd%d", current_label);

		
			append_element( // while-start label insert 
				ir_lines, 
				make_instruction_label(truewhilestring)
			);

			// evaluting expressions
			arg1 = IR_builder_expression(st->value.statement_while.exp);

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
			IR_builder_statement(st->value.statement_while.statement);
			
			append_element( // repeating statement jump
				ir_lines, 
				make_instruction_jmp(truewhilestring)
			);
			
			append_element( // insertion of while-end
				ir_lines, 
				make_instruction_label(endlabelstring)
			);
			break;

		case STATEMENT_LISTS:
			IR_builder_statement_list(st->value.statement_list);
			break;

		default:
			break;
	}
} 

ARGUMENT *IR_builder_opt_length ( OPT_LENGTH *oplen ) {
	return IR_builder_expression(oplen->exp);
}

ARGUMENT *IR_builder_variable (VAR *var) {

	SYMBOL *symbol;
	ARGUMENT *arg;
	ARGUMENT *arg1;
	ARGUMENT *resultOfSubExp;
	ARGUMENT *address_of_id;
	SYMBOLTABLE *tmpTable;
	
	switch (var->kind){
		case VAR_ID:

			symbol = getSymbol(var->symboltable, var->id);

			if(symbol->tableid != var->symboltable->id){
				append_element(ir_lines, make_instruction_pushl(ecx));
				append_element(ir_lines, 
					make_instruction_movl(
						make_argument_constant(symbol->tableid), ecx));

				append_element(ir_lines, 
					make_instruction_movl(
						make_argument_indexing(
							make_argument_label(
								"staticLinks"),ecx
						), ebx
					)
				);


				append_element(ir_lines, make_instruction_popl(ecx));

				arg = make_argument_static(WORDSIZE*(symbol->offset));
			} else{
				arg = make_argument_address(WORDSIZE*(symbol->offset));
			}
			break;
		case VAR_ARRAY:
				resultOfSubExp = IR_builder_expression(
					var->value.var_array.exp);
				arg = make_argument_tempregister(current_temporary++);

				append_element(ir_lines,
					make_instruction_movl(resultOfSubExp, arg));

				append_element(ir_lines,
					make_instruction_incl(arg)
				);

				arg1 = make_argument_indexing(
							make_argument_label(var->value.var_array.
								var->id), 
								// arrays in records problem
							arg
						);

			return arg1;
		case VAR_RECORD:

				if((symbol = getSymbol(var->symboltable,
					var->value.var_record.var->id)) != NULL){
					tmpTable = symbol->symboltype->child;
				}

				if((symbol = getSymbol(tmpTable, 
					var->value.var_record.id)) != NULL){
					resultOfSubExp = make_argument_constant(symbol->offset);
				}

				arg = make_argument_tempregister(current_temporary++);

				append_element(ir_lines,
						make_instruction_movl(
							resultOfSubExp,
							arg
						)
				);

				address_of_id = 
						make_argument_indexing(
							make_argument_label(
								var->value.var_record.var->id
								),
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
	ARGUMENT *result;
	IR_INSTRUCTION *instr;
	IR_INSTRUCTION *edxsave;
	IR_INSTRUCTION *edxrestore;

	if(exp->kind != EXPRES_TERM){
		argLeft = IR_builder_expression(exp->value.sides.left);
		argRight = IR_builder_expression(exp->value.sides.right);
	}

	switch(exp->kind){
		case EXPRES_TERM: 
			return IR_builder_term(exp->value.term);

		case EXPRES_PLUS:
			instr = make_instruction_addl(argLeft, argRight);
 			append_element(ir_lines, instr);
 			return argRight;

		case EXPRES_MINUS:
			instr = make_instruction_subl( argRight, argLeft);
 			append_element(ir_lines, instr);
 			return argLeft; 

		case EXPRES_TIMES:
			instr = make_instruction_imul(argLeft, argRight);
 			append_element(ir_lines, instr);
 			return argRight;

		case EXPRES_DIVIDE:
			tmp = getNextLabel();

			char *zeroden = calloc(MAXLABELSIZE,sizeof(char));
			sprintf(zeroden, "zeroDen%d", tmp);
			IR_INSTRUCTION *zerodenlabel = 
				make_instruction_label(zeroden);

			ARGUMENT *zeroArg = make_argument_constant(0);

			append_element(
				ir_lines,
				make_instruction_movl(
					argRight,
					eax
				)
			);

			IR_INSTRUCTION *cmpvszero = 
				make_instruction_cmp(zeroArg, eax);
			append_element(ir_lines, cmpvszero);

			IR_INSTRUCTION *notzero = make_instruction_jne(zeroden);
			append_element(ir_lines, notzero);

			ARGUMENT *exitARG1 = make_argument_constant(3);
			IR_INSTRUCTION *moveExitArg1 = 
				make_instruction_movl(exitARG1, ebx);
			append_element(ir_lines, moveExitArg1);

			ARGUMENT *exitARG2 = make_argument_constant(1);
			IR_INSTRUCTION *moveExitArg2 = 
				make_instruction_movl(exitARG2, eax);
			append_element(ir_lines, moveExitArg2);

			IR_INSTRUCTION *sysexit = make_instruction_intcode("0x80");
			append_element(ir_lines, sysexit);

			append_element(ir_lines, zerodenlabel);

			//HACK -------------------------

			IR_INSTRUCTION *hack1 = make_instruction_pushl(ebx);
			append_element(ir_lines, hack1);

			IR_INSTRUCTION *hack2 = make_instruction_movl(argRight, ebx);
			append_element(ir_lines, hack2);

			//------------------------------
			edxsave = make_instruction_pushl(edx); //Saving edx register
			append_element(ir_lines, edxsave);

			instr = make_instruction_xor(edx, edx); //Clear edx for DIV
			append_element(ir_lines, instr);

			IR_INSTRUCTION *instr1 = make_instruction_movl(argLeft, eax);
			append_element(ir_lines, instr1);

			IR_INSTRUCTION *instr2 = make_instruction_div(ebx);
			append_element(ir_lines, instr2);

			IR_INSTRUCTION *instr3 = make_instruction_movl(eax, argLeft);
			append_element(ir_lines, instr3);

			IR_INSTRUCTION *hack3 = make_instruction_popl(ebx);
			append_element(ir_lines, hack3);

			edxrestore = make_instruction_popl(edx);
			append_element(ir_lines, edxrestore); //Restoring edx register

			return argLeft;

		case EXPRES_EQ:
		case EXPRES_NEQ:
		case EXPRES_GREATER:
		case EXPRES_LESS:
		case EXPRES_LEQ:
		case EXPRES_GEQ:
			result = make_argument_tempregister(current_temporary++);
			tmp = getNextLabel();

			char *booltruelabel = calloc(MAXLABELSIZE,sizeof(char));
			char *boolendlabel = calloc(MAXLABELSIZE,sizeof(char));

			sprintf(booltruelabel, "booOPtrue%d", tmp);
			IR_INSTRUCTION *truelabel = make_instruction_label(booltruelabel);

			sprintf(boolendlabel, "boolOPend%d", tmp);
			IR_INSTRUCTION *endlabel = make_instruction_label(boolendlabel);


			instr = make_instruction_cmp( argRight, argLeft);
			append_element(ir_lines, instr);
	
			IR_INSTRUCTION *truejmp;

			switch(exp->kind){
				case EXPRES_EQ:
					truejmp = make_instruction_je(booltruelabel);
					break;

				case EXPRES_NEQ:
					truejmp = make_instruction_jne(booltruelabel);
					break;

				case EXPRES_GREATER:
					truejmp = make_instruction_jg(booltruelabel);
					break;

				case EXPRES_LESS:
					truejmp = make_instruction_jl(booltruelabel);
					break;

				case EXPRES_LEQ:
					truejmp = make_instruction_JLE(booltruelabel);
					break;

				case EXPRES_GEQ:
					truejmp = make_instruction_JGE(booltruelabel);
					break;

				default:
					return argRight;
			}

			append_element(ir_lines, truejmp); //false
			IR_INSTRUCTION *falsesave = make_instruction_movl(
			make_argument_constant(0), result);
			append_element(ir_lines, falsesave);


			IR_INSTRUCTION *endjmp = make_instruction_jmp(boolendlabel);
			append_element(ir_lines, endjmp);

			//true
			append_element(ir_lines, truelabel);

			IR_INSTRUCTION *truesave = make_instruction_movl(
				make_argument_constant(1), result);
			append_element(ir_lines, truesave);

			append_element(ir_lines, endlabel);
			return result;

		case EXPRES_AND: //Checked
			tmp = getNextLabel();

			char *andFalselabel = calloc(MAXLABELSIZE,sizeof(char));
			char *andEndlabel = calloc(MAXLABELSIZE,sizeof(char));

			sprintf(andFalselabel, "ANDfalse%d", tmp);
			IR_INSTRUCTION *andFalseinstr = make_instruction_label(
					andFalselabel);

			sprintf(andEndlabel, "ANDend%d", tmp);
			IR_INSTRUCTION *andEndinstr = make_instruction_label(andEndlabel);

			result = make_argument_tempregister(current_temporary++);

			ARGUMENT *cmpARG = make_argument_constant(1);


			IR_INSTRUCTION *cmp1 = make_instruction_cmp(cmpARG, argLeft);
			append_element(ir_lines, cmp1);
			

			IR_INSTRUCTION *jmpfalse = make_instruction_jne(andFalselabel);
			append_element(ir_lines, jmpfalse);

			IR_INSTRUCTION *cmp2 = make_instruction_cmp(cmpARG, argRight);
			append_element(ir_lines, cmp2);
	

			append_element(ir_lines, jmpfalse);

			IR_INSTRUCTION *trueand = 
			make_instruction_movl(make_argument_constant(1), result);
			append_element(ir_lines, trueand);

			IR_INSTRUCTION *andjmpend = make_instruction_jmp(andEndlabel);
			append_element(ir_lines, andjmpend);

			append_element(ir_lines, andFalseinstr);
			IR_INSTRUCTION *falseand = 
				make_instruction_movl(make_argument_constant(0), result);
			append_element(ir_lines, falseand);

			append_element(ir_lines, andEndinstr);
			append_element(ir_lines, make_instruction_popl(edx));
			break;
		case EXPRES_OR:
			tmp = getNextLabel();

			char *orOKlabel = calloc(MAXLABELSIZE,sizeof(char));
			char *orEndlabel = calloc(MAXLABELSIZE,sizeof(char));

			sprintf(orOKlabel, "ORtrue%d", tmp);
			IR_INSTRUCTION *oroklabel = make_instruction_label(orOKlabel);

			sprintf(orEndlabel, "ORend%d", tmp);
			IR_INSTRUCTION *orendlabel = make_instruction_label(orEndlabel);

			result = make_argument_tempregister(current_temporary++);

			ARGUMENT *cmporARG = make_argument_constant(1);

			IR_INSTRUCTION *cmpor1 = make_instruction_cmp(cmporARG, argLeft);
			append_element(ir_lines, cmpor1);

			IR_INSTRUCTION *jmpOK = make_instruction_je(orOKlabel);
			append_element(ir_lines, jmpOK);

			IR_INSTRUCTION *cmpor2 = make_instruction_cmp(cmporARG, argRight);
			append_element(ir_lines, cmpor2);
			append_element(ir_lines, jmpOK);

			IR_INSTRUCTION *orfalsesave = make_instruction_movl(
				make_argument_constant(0), result);
			append_element(ir_lines, orfalsesave);

			IR_INSTRUCTION *jmpend = make_instruction_jmp(orEndlabel);
			append_element(ir_lines, jmpend);
			append_element(ir_lines, oroklabel);

			IR_INSTRUCTION *ortruesave = make_instruction_movl(
				make_argument_constant(1), result);
			append_element(ir_lines, ortruesave);

			append_element(ir_lines, orendlabel);
			break;
	}

	return argLeft;

}

ARGUMENT *IR_builder_term ( TERM *term) {

	ARGUMENT *arg1;
	ARGUMENT *arg2;
	SYMBOL *symbol;
	int tmp;
	int params;
	char *endlabelstring;
	char *falselabel;

	switch(term->kind){
		case TERM_NUM:
			arg1 = make_argument_constant(term->value.intconst);
			//arg2 = make_argument_tempregister(current_temporary++);
			//instr = make_instruction_movl(arg1, arg2);
			//append_element(ir_lines, instr);
			return arg1;//arg2; //Return arg2 to keep track of temps

		case TERM_TRUE:
			arg1 = make_argument_constant(1);
			//arg2 = make_argument_tempregister(current_temporary++);
			//instr = make_instruction_movl(arg1, arg2);
			//append_element(ir_lines, instr);
			return arg1;//arg2; //Return arg2 to keep track of temps

		case TERM_NULL:
		case TERM_FALSE:
			arg1 = make_argument_constant(0);
			//arg2 = make_argument_tempregister(current_temporary++);
			//instr = make_instruction_movl(arg1, arg2);
			//append_element(ir_lines, instr);
			return arg1;//arg2; //Return arg2 to keep track of temps

		case TERM_PARENTESES: // paranteses just parses
			return IR_builder_expression(term->value.exp);

		case TERM_VAR:
			
			arg1 = IR_builder_variable(term->value.var);
			//arg2 = make_argument_tempregister(current_temporary++);
			//instr = make_instruction_movl(arg1, arg2);
			//append_element(ir_lines, instr);
			return arg1;//arg2; //Return arg2 to keep track of temps			

		case TERM_ACT_LIST:
			addStaticLink(term->symboltable->id);
			symbol = getSymbol(term->symboltable, 
				term->value.term_act_list.id);

			// push parameters on stack recursively
			IR_builder_act_list(term->value.term_act_list.actlist);

			arg2 = make_argument_label(symbol->uniquename);
			IR_INSTRUCTION *call = make_instruction_call(arg2);
			append_element(ir_lines, call);
			//function paramerters are useless after function call
			moveStackpointer(symbol->noArguments);

			//Handle return value as it can sit in eax
			ARGUMENT *returnarg = make_argument_tempregister(
				current_temporary++);
			IR_INSTRUCTION *savereturn = make_instruction_movl(
				eax, returnarg);
			append_element(ir_lines, savereturn);
			return returnarg; // by convention eax holds return values

		case TERM_NOT:
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
				make_instruction_label(falselabel)
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
				make_instruction_label(endlabelstring)
				);
			
			return arg1;

		case TERM_ABS:
			params = 0;
			char *pipeEnd = calloc(MAXLABELSIZE,sizeof(char));
			sprintf(pipeEnd, "pipeEnd%d", getNextLabel());
			IR_INSTRUCTION *pipeLabel = make_instruction_label(pipeEnd);

			arg1 = IR_builder_expression(term->value.exp);
			if(term->symboltype->type == SYMBOL_INT){
				//2 cases, positive numbers and negative numbers
				ARGUMENT *pipeCMPArg = make_argument_constant(0);

				IR_INSTRUCTION *pipeCMP = make_instruction_cmp(pipeCMPArg, 
					arg1);
				append_element(ir_lines, pipeCMP);
				IR_INSTRUCTION *pipeEndJmp = make_instruction_JGE(pipeEnd);
				append_element(ir_lines, pipeEndJmp);
				IR_INSTRUCTION *negl = make_instruction_negl(arg1);
				append_element(ir_lines, negl);
				append_element(ir_lines, pipeLabel);
				return arg1;
			}else if(term->symboltype->type == SYMBOL_ARRAY){
				char *id = term->value.exp->value.term->value.var->id;

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
						make_argument_indexing(
							make_argument_label(id), 
								// label to pointer to heap
							zeroElement // index
						),
						firstElement // result
					)
				);
				return firstElement;
			}
		default:
			break;
	}

	return NULL;
}

void IR_builder_act_list ( ACT_LIST *actlst) { 

	switch(actlst->kind){
		case ACT_LIST_EXPLIST:
			IR_builder_expression_list(actlst->exp_list);
			break;
		case ACT_LIST_EMPTY:
			break;
	}
}

// Since expression_list is used only by act list we can push function
// parameters from here
void IR_builder_expression_list ( EXP_LIST *explst) {

	switch(explst->kind){
		case EXP_LIST_EXP:
			append_element(
				ir_lines, make_instruction_pushl(
					IR_builder_expression(explst->exp)
					)
				); 
			break;
		case EXP_LIST_LIST:
		
			IR_builder_expression_list(explst->explist);
			append_element(
				ir_lines, make_instruction_pushl(
					IR_builder_expression(explst->exp)
					)
				);
			break;
	}
}

/* Adding allocation of local variables, this is by convention 
 *	a subtration of the stack pointer 
 */
IR_INSTRUCTION *localVariableAllocation(SYMBOLTABLE *currentScope) {
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

void callerSave(){

	IR_INSTRUCTION *instr1 = make_instruction_pushl(ecx);
	IR_INSTRUCTION *instr2 = make_instruction_pushl(edx);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);

}

void callerRestore(){

	IR_INSTRUCTION *instr1 = make_instruction_popl(edx);
	IR_INSTRUCTION *instr2 = make_instruction_popl(ecx);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);

}

void calleeStart(){

	IR_INSTRUCTION *instr1 = make_instruction_pushl(ebp);
	IR_INSTRUCTION *instr2 = make_instruction_movl(esp, ebp);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);

}

void calleeEnd(){

	IR_INSTRUCTION *instr1 = make_instruction_movl(ebp, esp);
	IR_INSTRUCTION *instr2 = make_instruction_popl(ebp);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);

}

void calleeSave(){

	IR_INSTRUCTION *instr1 = make_instruction_pushl(ebx);
	IR_INSTRUCTION *instr2 = make_instruction_pushl(esi);
	IR_INSTRUCTION *instr3 = make_instruction_pushl(edi);
	append_element(ir_lines, instr1);
	append_element(ir_lines, instr2);
	append_element(ir_lines, instr3);
}

void calleeRestore(){

	IR_INSTRUCTION *instr1 = make_instruction_popl(ebx);
	IR_INSTRUCTION *instr2 = make_instruction_popl(esi);
	IR_INSTRUCTION *instr3 = make_instruction_popl(edi);
	append_element(ir_lines, instr3);
	append_element(ir_lines, instr2);
	append_element(ir_lines, instr1);

}

void moveStackpointer(int i){

	ARGUMENT *arg = make_argument_constant(i * WORDSIZE);
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

		if(instr1->arg1 != NULL && instr1->arg1->kind == tempreg_arg){
			reg = get_register(count);
			save = temp;
			int cmp1 = instr1->arg1->tempid;

			while(temp != ir_lines){

				instr2 = (IR_INSTRUCTION *) temp->data;

				if(instr2->arg1 != NULL && instr2->arg1->kind == 
						tempreg_arg){
					if(instr2->arg1->tempid == cmp1){

						instr2->arg1 = reg;
					}
				}
				//Arrays
				if(instr2->arg1 != NULL && instr2->arg1->kind == 
						indexing_arg){
					if(instr2->arg1->index->tempid == cmp1){

						instr2->arg1->index = reg;
					}
				}

				if(instr2->arg2 != NULL && instr2->arg2->kind == 
						tempreg_arg){
					if(instr2->arg2->tempid == cmp1){

						instr2->arg2 = reg;
					}
				}

                //Arrays
                if(instr2->arg2 != NULL && instr2->arg2->kind == 
                		indexing_arg){
                        if(instr2->arg2->index->tempid == cmp1){

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
                if(instr2->arg1 != NULL && instr2->arg1->kind == 
                		indexing_arg){
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
                if(instr2->arg2 != NULL && instr2->arg2->kind == 
                		indexing_arg){
                        if(instr2->arg2->index->tempid == cmp2){

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
	
	
	if(get_length(data_lines) > 0){
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
				case staticlink_arg:
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
				case staticlink_arg:
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
				case staticlink_arg:
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
				case staticlink_arg:
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
