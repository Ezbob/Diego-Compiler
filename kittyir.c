#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "kittyir.h"
#include "irInstructions.h"
#include "parserscanner/kittytree.h"
#include "typechecker/stack.h"

static int current_label = 0;
static int has_errors = 0;
static int has_prints = 0;

extern linked_list *ir_lines; // plug IR code in here
static linked_list *data_lines; // for allocates, we use variables
static stackT *function_stack;

ARGUMENT *eax, *ebx, *ecx, *edx, *edi, *esi, *ebp, *esp;
ARGUMENT *one, *zero;
ARGUMENT *heapAddress, *heapFreePointer;
ARGUMENT *printFormInt, *printFormNull, *printFormTrue, *printFormFalse;
ARGUMENT *wordSizePointer;
ARGUMENT *printfLabel;

IR_INSTRUCTION *pushEax, *pushEbx, *pushEcx, *pushEdx, *pushEdi, *pushEsi,
		*pushEbp, *pushEsp;
IR_INSTRUCTION *popEax, *popEbx, *popEcx, *popEdx, *popEdi, *popEsi, *popEbp,
		*popEsp;

/*
 * Initialization of common arguments
 */
void init_argument_constants() {
	init_registers();
	one = make_argument_constant(1);
	zero = make_argument_constant(0);
	heapAddress = make_argument_label("$heap.");
	heapFreePointer = make_argument_label("heap.NEXT");
	printFormFalse = make_argument_label("$form.FALSE");
	printFormTrue = make_argument_label("$form.TRUE");
	printFormNull = make_argument_label("$form.NULL");
	printFormInt = make_argument_label("$form.NUM");
	wordSizePointer = make_argument_plain_constant(WORD_SIZE);
	printfLabel = make_argument_label("printf");
}

void init_stack_instructions() {
	pushEax = make_instruction_pushl(eax);
	pushEbx = make_instruction_pushl(ebx);
	pushEcx = make_instruction_pushl(ecx);
	pushEdx = make_instruction_pushl(edx);
	pushEdi = make_instruction_pushl(edi);
	pushEsi = make_instruction_pushl(esi);
	pushEbp = make_instruction_pushl(ebp);
	pushEsp = make_instruction_pushl(esp);

	popEax = make_instruction_popl(eax);
	popEbx = make_instruction_popl(ebx);
	popEcx = make_instruction_popl(ecx);
	popEdx = make_instruction_popl(edx);
	popEdi = make_instruction_popl(edi);
	popEsi = make_instruction_popl(esi);
	popEbp = make_instruction_popl(ebp);
	popEsp = make_instruction_popl(esp);
}

void init_registers() {
	eax = make_argument_register("eax");
	ebx = make_argument_register("ebx");
	ecx = make_argument_register("ecx");
	edx = make_argument_register("edx");
	edi = make_argument_register("edi");
	esi = make_argument_register("esi");
	ebp = make_argument_register("ebp");
	esp = make_argument_register("esp");
}

void init_heap() {
    if ( get_length(data_lines) > 0 ) {
        append_element(ir_lines, make_instruction_movl(heapAddress,
                                                       heapFreePointer));
    }
}

void IR_build( BODY *program ) {
	fprintf(stderr, "Initializing intermediate code generation phase\n");
	ir_lines = initialize_list();
	data_lines = initialize_list();
	function_stack = stackInit();
	init_argument_constants();
	init_stack_instructions();

	// adding text section for completion
	append_element(ir_lines, make_instruction_directive(".text"));

	IR_builder_decl_list(program->decl_list);

	// make ".globl main" directive
	append_element(ir_lines, make_instruction_directive(".globl main"));

	// make "main:" label line
	append_element(ir_lines, make_instruction_label("main"));

	function_prolog(program->symboltable);

    init_heap();

	IR_builder_statement_list(program->statement_list);

	append_element(ir_lines, make_instruction_movl(zero, eax));
	// return signal zero: all good
	function_epilog();

	program->symboltable->localVars = 0; // resetting local variables counter
	append_element(ir_lines, make_instruction_ret());

	build_data_section();

	stackDestroy(function_stack);
}

void IR_builder_function(FUNC *func) {
	funcStackPush(function_stack, func);
	// we can now refer to the current function

	// move the handling of the declaration list here instead of the body to
	// avoid nested function getting generated inside each others 
	IR_builder_decl_list(func->body->decl_list);

	// header makes the function label
	IR_builder_head(func->head);

	IR_builder_body(func->body);

	func->symboltable->localVars = 0; // reset local variables in scope

	funcStackPop(function_stack);
		// leaving the function
}

void IR_builder_head(HEAD *head) {
	char *functionStartLabel;
	SYMBOL *symbol = getSymbol(head->symboltable, head->id);
	GET_FUNCTION_LABEL(functionStartLabel, symbol->name, symbol->offset);

	append_element(ir_lines, make_instruction_label(functionStartLabel));
}

void IR_builder_body (BODY *body) {
	function_prolog(body->symboltable);
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
	// for int and bool we allocate place in stack.
	// else we maybe have to heap it
	switch (vtype->type->kind) {
		case TYPE_INT:
			if ( vtype->symbol->symbolKind == LOCAL_VARIABLE_SYMBOL ) {
				vtype->symboltable->localVars += WORD_SIZE;
			}
			break;
		case TYPE_BOOL:
			if ( vtype->symbol->symbolKind == LOCAL_VARIABLE_SYMBOL ) {
				vtype->symboltable->localVars += WORD_SIZE;
			}
			break;
		default:
            append_element(data_lines, vtype);
			break;
	}
 }

void IR_builder_decl_list ( DECL_LIST *dlst ) {
	switch(dlst->kind) {
		case DECL_LIST_LIST:
			IR_builder_decl_list(dlst->decl_list);
			IR_builder_declaration(dlst->declaration);
			break;
		case DECL_LIST_EMPTY:
			break;
	}
}

void IR_builder_declaration ( DECLARATION *decl ) {
	switch(decl->kind) {
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
	int labelIdCounter = 0;
	int numberOfRecordMembers;

	char *endLabel;
	char *elseLabel;
	char *printLabel;
	char *falseLabel;
	char *trueLabel;

	IR_INSTRUCTION *pushForm;
	ARGUMENT *variable;

	switch(st->kind){

		case STATEMENT_RETURN:	
			IR_builder_expression(st->value.statement_return.exp);
			append_element(ir_lines, popEax);
			function_epilog();
			append_element(ir_lines, make_instruction_ret());
			break;

		case STATEMENT_WRITE:
            has_prints = 1;
			caller_save();
			IR_builder_expression(st->value.exp);
			append_element(ir_lines, popEax);
				// result from expression
			switch(st->value.exp->symboltype->type){
				case SYMBOL_BOOL:
					labelIdCounter = GET_NEXT_LABEL_ID;

                    GET_FLOW_CONTROL_LABEL(falseLabel, "printFalse",
                                           labelIdCounter);
                    GET_FLOW_CONTROL_LABEL(trueLabel, "printTrue",
                                           labelIdCounter);
                    GET_FLOW_CONTROL_LABEL(printLabel,"printBool",
                                           labelIdCounter);

					// compare boolean value to true
					append_element(ir_lines, make_instruction_cmp(one, eax));

					// true has to be printed?
					append_element(ir_lines,
								   make_instruction_jne(falseLabel));

					// making a push to the stack with result of expression
					append_element(ir_lines, pushEax);

					// true case here
					append_element(ir_lines, make_instruction_pushl(
							printFormTrue));

					// jump to printf call
					append_element(ir_lines,
								   make_instruction_jmp(printLabel));

					// false label section
					append_element(ir_lines,
								   make_instruction_label(falseLabel));

					// making a push to the stack with result of expression
					append_element(ir_lines, pushEax);

					// false case here
					append_element(ir_lines, make_instruction_pushl(
							printFormFalse));

					// printing section
					append_element(ir_lines, make_instruction_label(
							printLabel));
					break;

				case SYMBOL_INT:
				case SYMBOL_NULL:
					append_element(ir_lines, pushEax);

					if (st->value.exp->value.term->kind == TERM_NULL) {
						pushForm = make_instruction_pushl(printFormNull);
					} else {
						pushForm = make_instruction_pushl(printFormInt);
					}

					append_element(ir_lines, pushForm);
					break;

				case SYMBOL_ARRAY:
					append_element(ir_lines, pushEax);

					append_element(ir_lines, make_instruction_pushl(
							printFormInt));
					break;
				default:
					break;
			}
			// call to print
			append_element( ir_lines, make_instruction_call(printfLabel));

			add_to_stack_pointer(2); // clean up
			caller_restore();
			break;

        case STATEMENT_ADDASSIGN:
        case STATEMENT_SUBASSIGN:
        case STATEMENT_MULASSIGN:
        case STATEMENT_DIVASSIGN:
        case STATEMENT_MODASSIGN:
		case STATEMENT_ASSIGN:
			IR_builder_expression(st->value.statement_assign.exp);
			variable = IR_builder_variable(st->value.statement_assign.var);

			append_element(ir_lines, popEbx);
				// expression
            switch (st->kind) {
                case STATEMENT_ADDASSIGN:
                    append_element(ir_lines, make_instruction_movl(variable,
                                                                   eax));
                    append_element(ir_lines, make_instruction_addl(ebx, eax));
                    append_element(ir_lines, make_instruction_movl(eax,
                                                                   variable));
                    break;
                case STATEMENT_SUBASSIGN:
                    append_element(ir_lines, make_instruction_movl(variable,
                                                                   eax));
                    append_element(ir_lines, make_instruction_subl(ebx, eax));
                    append_element(ir_lines, make_instruction_movl(eax,
                                                                   variable));
                    break;
                case STATEMENT_MULASSIGN:
                    append_element(ir_lines, make_instruction_movl(variable,
                                                                   eax));
                    append_element(ir_lines, make_instruction_imul(ebx, eax));
                    append_element(ir_lines, make_instruction_movl(eax,
                                                                   variable));
                    break;
                case STATEMENT_DIVASSIGN:
                case STATEMENT_MODASSIGN:
                    append_element(ir_lines, make_instruction_movl(variable,
                                                                   eax));
                    division_by_zero_runtime_check(st->lineno, ebx);

                    append_element(ir_lines, make_instruction_xor(edx, edx));
                    // Clear edx for modulo

                    append_element(ir_lines, make_instruction_div(ebx));
                    // divide eax with eax to get result in eax

                    if ( st->kind == STATEMENT_DIVASSIGN ) {
                        append_element(ir_lines,
                                       make_instruction_movl(eax, variable));
                        // result: quotient on the stack
                    } else {
                        append_element(ir_lines,
                                       make_instruction_movl(edx, variable));
                        // else: remainder on stack
                    }
                    break;
                case STATEMENT_ASSIGN:
                    append_element(ir_lines, make_instruction_movl(ebx,
                                                                   variable));
                    break;
                default:
                    break;
            }

			break;

		case STATEMENT_IFBRANCH:
			// generate code for boolean expression(s)
			IR_builder_expression(st->value.statement_ifbranch.exp);

			append_element(ir_lines, popEax);

			labelIdCounter = GET_NEXT_LABEL_ID;

            GET_FLOW_CONTROL_LABEL(elseLabel, "else", labelIdCounter);
            GET_FLOW_CONTROL_LABEL(endLabel, "endIf", labelIdCounter);

			//Comparison with "true" boolean value
			append_element(ir_lines, make_instruction_cmp(one, eax));

			if(st->value.statement_ifbranch.opt_else->kind != OPT_ELSE_EMPTY){
				// if not equal goto else part
				append_element(ir_lines, make_instruction_jne(elseLabel));
			} else {
				// if not equal goto end-of-if
				append_element(ir_lines,
							   make_instruction_jne(endLabel));
			}

			IR_builder_statement(st->value.statement_ifbranch.statement);
				// build statements in if-case

			if(st->value.statement_ifbranch.opt_else->kind != OPT_ELSE_EMPTY){

				// we have to jump over
				//else when if-case is true
				append_element(ir_lines,
							   make_instruction_jmp(endLabel));

				// make else-label
				append_element(ir_lines, make_instruction_label(elseLabel));

				IR_builder_statement( // build else statements
					st->value.statement_ifbranch.opt_else->statement);
			}

			// end-of-if label
			append_element(ir_lines, make_instruction_label(endLabel));
			break;

		case STATEMENT_ALLOCATE:
			switch(st->value.statement_allocate.var->symboltype->type){

				case SYMBOL_ARRAY:
					// assume that the checker has checked if "length of" is
					// present

					variable = IR_builder_variable(
							st->value.statement_allocate.var);

					append_element(ir_lines, make_instruction_movl(
							heapFreePointer, eax));

					append_element(ir_lines,
							   make_instruction_movl(eax, variable));

					// xored to get zero, aka the first index
					append_element(ir_lines,
								   make_instruction_xor(ebx, ebx));

					IR_builder_opt_length(st->value.statement_allocate
												  .opt_length);
					append_element(ir_lines, popEcx);

					// edi and esi is free here
					negative_array_size_check(st->lineno, ecx);

					// move the array size to the first index
					append_element(ir_lines, make_instruction_movl(ecx,
								make_argument_indexing(NULL, eax, ebx)));

					// array length + 1 since we use space
					append_element(ir_lines,
								   make_instruction_incl(ecx));

					// getting array size in bytes
					append_element(ir_lines, make_instruction_imul(
							make_argument_constant(WORD_SIZE), ecx));

					// eax is free, ebx is free
					out_of_memory_runtime_check(st->lineno, ecx);

					// update the heap free pointer
					append_element(ir_lines, make_instruction_addl(ecx,
							heapFreePointer));

					break;

				case SYMBOL_RECORD:

					variable = IR_builder_variable(st->value.
							statement_allocate.var);

					// copy heapFree pointer address to variable
					append_element(ir_lines, make_instruction_movl(
							heapFreePointer, eax));
					append_element(ir_lines,
								   make_instruction_movl(eax, variable));

					numberOfRecordMembers = st->value.statement_allocate.
							var->symboltype->arguments;

					// we need number of members in record
					append_element(ir_lines, make_instruction_movl(
							make_argument_constant(numberOfRecordMembers)
							, ecx));

					append_element(ir_lines, make_instruction_imul(
							make_argument_constant(WORD_SIZE), ecx));

					// eax and ebx is free
					out_of_memory_runtime_check(st->lineno, ecx);

					// add to the next pointer
					append_element(ir_lines, make_instruction_addl(ecx,
							heapFreePointer));

					break;
				default:
					break;
				}
			break;

		case STATEMENT_WHILE:
			labelIdCounter = GET_NEXT_LABEL_ID;

            GET_FLOW_CONTROL_LABEL(st->value.statement_while.start_label, 
                                   "whileStart", labelIdCounter);
            GET_FLOW_CONTROL_LABEL(st->value.statement_while.end_label,
                                   "whileEnd", labelIdCounter);
            
			// while-start label insert
			append_element(ir_lines, make_instruction_label(
                    st->value.statement_while.start_label));

			// evaluating expressions
			IR_builder_expression(st->value.statement_while.exp);
			append_element(ir_lines, popEax);

			//Compare evaluated expression with true
			append_element(ir_lines, make_instruction_cmp(one, eax));

			// jump to end if while condition is false
			append_element(ir_lines, make_instruction_jne(st->value.
                    statement_while.end_label));

			// generate code for statements
			IR_builder_statement(st->value.statement_while.statement);

			// repeating statement jump
			append_element(ir_lines, make_instruction_jmp(
                    st->value.statement_while.start_label));

			// insertion of while-end
			append_element(ir_lines, make_instruction_label(
                    st->value.statement_while.end_label));
			break;

		case STATEMENT_LISTS:
			IR_builder_statement_list(st->value.statement_list);
			break;

		case STATEMENT_BREAK:
			append_element(ir_lines, make_instruction_jmp(
				st->currentLoop->value.statement_while.end_label));
			break;

		case STATEMENT_CONTINUE:
			append_element(ir_lines, make_instruction_jmp(
				st->currentLoop->value.statement_while.start_label));
			break;
		default:
			break;
	}
} 

void IR_builder_opt_length ( OPT_LENGTH *opt_length ) {
	IR_builder_expression(opt_length->exp);
}

ARGUMENT *IR_builder_variable (VAR *var) {

	int offsetValue;
	int callScopeId;
	int definedScopeId;

	SYMBOL *symbol;
	ARGUMENT *result;
	ARGUMENT *offset;
	ARGUMENT *base;
	SYMBOL_TABLE *childTable;
	
	switch ( var->kind ) {
		case VAR_ID:
			symbol = getSymbol(var->symboltable, var->id);
			result = NULL;

			if( symbol != NULL ) {
				offsetValue = (symbol->offset) * WORD_SIZE;

				if ( ( symbol->symbolType->type == SYMBOL_ARRAY ||
						symbol->symbolType->type == SYMBOL_RECORD ) &&
						!symbol->symbolKind == PARAMETER_SYMBOL ) {

					// They're on the heap so we just use labels
					result = make_argument_label(var->id);

				} else if (symbol->tableId < var->symboltable->id) {
					// basically, if variable is not in current,
					// use static link

					callScopeId = var->symboltable->id;
					definedScopeId = symbol->tableId;

					append_element(ir_lines, make_instruction_movl(
							make_argument_address(8, ebp), eax));
					callScopeId--;

					while ( definedScopeId != callScopeId) {
						// get previous base pointer by iteration
						append_element(ir_lines, make_instruction_movl(
								make_argument_address(8, eax), eax));
						callScopeId--;
					}

					if ( symbol->symbolKind == LOCAL_VARIABLE_SYMBOL ) {
						result = make_argument_address( -1 * offsetValue,
						eax );
					} else {
						offsetValue = offsetValue + (2 * WORD_SIZE);
						result = make_argument_address( offsetValue, eax );
						// beware of the return address on the stack
					}

				} else {
					if ( symbol->symbolKind == LOCAL_VARIABLE_SYMBOL ) {
						result = make_argument_address( -1 * offsetValue,
														ebp );

					} else {
						offsetValue = offsetValue + (2 * WORD_SIZE);
						result = make_argument_address( offsetValue, ebp );
						// beware of the return address on the stack
					}

				}
			}

			return result;

		case VAR_ARRAY:
			base = IR_builder_variable(var->value.var_array.var);
			IR_builder_expression(var->value.var_array.exp);

			append_element(ir_lines, make_instruction_movl(base, esi));
			null_pointer_runtime_check(var->lineno, esi);

			append_element(ir_lines, popEdi);
				// exp
			out_of_bounds_runtime_check(var->lineno, esi, edi);

			append_element(ir_lines, make_instruction_incl(edi));
				// increment since we use the first element as the size

			// return the indexing into the array
			return make_argument_indexing(NULL, esi, edi);

		case VAR_RECORD:
			base = IR_builder_variable(var->value.var_record.var);



			childTable = var->value.var_record.var->symboltype->child;
				// This must be the child table

			if( ( symbol = getSymbol(childTable, var->value.var_record.id) )
			   != NULL) {
				offset = make_argument_constant(symbol->offset - 1);
				// member index in the record as argument
				// member index is zero indiced
			}
			append_element(ir_lines, make_instruction_movl(base,esi));
			null_pointer_runtime_check(var->lineno, esi);

			append_element(ir_lines, make_instruction_movl(offset, edi));

			// returns much the same as arrays
			return make_argument_indexing(NULL, esi, edi);
	}
	return NULL;
}

void IR_builder_expression ( EXPRES *exp ) {

    char *trueLabel;
    char *falseLabel;
    char *endLabel;
	int labelIdCounter = 0;
	ARGUMENT *truth;

	switch(exp->kind){
		case EXPRES_TERM: 
			IR_builder_term(exp->value.term);
			break;

		case EXPRES_PLUS:
		case EXPRES_MINUS:
		case EXPRES_TIMES:
			IR_builder_expression(exp->value.sides.left);
			IR_builder_expression(exp->value.sides.right);
			append_element(ir_lines, popEbx);
				// rhs
			append_element(ir_lines, popEcx);
				// lhs
			switch (exp->kind) {
				case EXPRES_PLUS:
					append_element(ir_lines, make_instruction_addl(ebx, ecx));
					break;
				case EXPRES_MINUS:
					append_element(ir_lines, make_instruction_subl(ebx, ecx));
					break;
				case EXPRES_TIMES:
					append_element(ir_lines, make_instruction_imul(ebx, ecx));
					break;
				default:
					break;
			}
			append_element(ir_lines, pushEcx);
			break;

		case EXPRES_DIVIDE:
        case EXPRES_MODULO:
			IR_builder_expression(exp->value.sides.left);
			IR_builder_expression(exp->value.sides.right);
			append_element(ir_lines, popEbx);
			// rhs
			append_element(ir_lines, popEax);
			// lhs

			division_by_zero_runtime_check(exp->lineno, ebx);

			append_element(ir_lines, make_instruction_xor(edx, edx));
				// Clear edx for modulo

			append_element(ir_lines, make_instruction_div(ebx));
				// divide eax with eax to get result in eax
            if ( exp->kind == EXPRES_DIVIDE ) {
                append_element(ir_lines, pushEax);
                // result: quotient on the stack
            } else {
                append_element(ir_lines, pushEdx);
                // else: remainder on stack
            }
			break;

		case EXPRES_EQ:
		case EXPRES_NEQ:
		case EXPRES_GREATER:
		case EXPRES_LESS:
		case EXPRES_LEQ:
		case EXPRES_GEQ:
			IR_builder_expression(exp->value.sides.left);
			IR_builder_expression(exp->value.sides.right);
			labelIdCounter = GET_NEXT_LABEL_ID;

            GET_FLOW_CONTROL_LABEL(trueLabel, "boolOPtrue", labelIdCounter);
            GET_FLOW_CONTROL_LABEL(endLabel, "boolOPend", labelIdCounter);

			append_element(ir_lines, popEbx);
				// rhs
			append_element(ir_lines, popEcx);
				// lhs

			append_element(ir_lines, make_instruction_cmp( ebx, ecx ));
				// compare both sides
	
			IR_INSTRUCTION *trueJump;

			switch(exp->kind){
				case EXPRES_EQ:
					trueJump = make_instruction_je(trueLabel);
					break;

				case EXPRES_NEQ:
					trueJump = make_instruction_jne(trueLabel);
					break;

				case EXPRES_GREATER:
					trueJump = make_instruction_jg(trueLabel);
					break;

				case EXPRES_LESS:
					trueJump = make_instruction_jl(trueLabel);
					break;

				case EXPRES_LEQ:
					trueJump = make_instruction_JLE(trueLabel);
					break;

				case EXPRES_GEQ:
					trueJump = make_instruction_JGE(trueLabel);
					break;
				default:
					break;
			}

			append_element(ir_lines, trueJump);
				// the jump to true instruction

			append_element(ir_lines, make_instruction_pushl(zero));
				// the false case
			append_element(ir_lines, make_instruction_jmp(endLabel));

			append_element(ir_lines, make_instruction_label(trueLabel));

			append_element(ir_lines, make_instruction_pushl(one));
				// the true case

			append_element(ir_lines, make_instruction_label(endLabel));
			break;

		case EXPRES_AND:
			labelIdCounter = GET_NEXT_LABEL_ID;

            GET_FLOW_CONTROL_LABEL(falseLabel,"ANDfalse",labelIdCounter);
			GET_FLOW_CONTROL_LABEL(endLabel,"ANDend",labelIdCounter);

			truth = one;

			IR_INSTRUCTION *jumpToFalse = make_instruction_jne(falseLabel);

			// lazy evaluation on the false case
			IR_builder_expression(exp->value.sides.right);
			append_element(ir_lines, popEbx);
			append_element(ir_lines, make_instruction_cmp(truth, ebx));
			append_element(ir_lines, jumpToFalse);
			// rhs

			IR_builder_expression(exp->value.sides.left);
			append_element(ir_lines, popEbx);
			append_element(ir_lines, make_instruction_cmp(truth, ebx));
			append_element(ir_lines, jumpToFalse);
			// lhs

			append_element(ir_lines,make_instruction_pushl(truth));
			append_element(ir_lines, make_instruction_jmp(endLabel));
				// in case both arguments are true

			append_element(ir_lines, make_instruction_label(falseLabel));
			append_element(ir_lines, make_instruction_pushl(zero));
				// in case one of the arguments are false

			append_element(ir_lines, make_instruction_label(endLabel));
			break;
		case EXPRES_OR:
			labelIdCounter = GET_NEXT_LABEL_ID;

            GET_FLOW_CONTROL_LABEL(trueLabel, "ORtrue", labelIdCounter);
            GET_FLOW_CONTROL_LABEL(endLabel, "ORend", labelIdCounter);

			truth = one;

			IR_INSTRUCTION *jumpToTrue = make_instruction_je(trueLabel);

			// Note: using lazy evaluation here
			IR_builder_expression(exp->value.sides.left);
			append_element(ir_lines, popEbx);
			append_element(ir_lines, make_instruction_cmp(truth, ebx));
			append_element(ir_lines, jumpToTrue);
			// LHS

			IR_builder_expression(exp->value.sides.right);
			append_element(ir_lines, popEbx);
			append_element(ir_lines,make_instruction_cmp(truth, ebx));
			append_element(ir_lines, jumpToTrue);
			// RHS

			append_element(ir_lines, make_instruction_pushl(zero));
				// false case
			append_element(ir_lines, make_instruction_jmp(endLabel));

			append_element(ir_lines, make_instruction_label(trueLabel));
			append_element(ir_lines, make_instruction_pushl(truth));
				// true case

			append_element(ir_lines, make_instruction_label(endLabel));
			break;
	}

}

void IR_builder_term ( TERM *term ) {

	SYMBOL *symbol;
	ARGUMENT *variable;
	char *positiveNumberLabel;
	char *functionLabel;
	int callScopeId;
	int definedScopeId;

	switch(term->kind){
		case TERM_NUM:
			append_element(ir_lines, make_instruction_pushl(
					make_argument_constant(term->value.intconst)));
			break;

		case TERM_TRUE:
			append_element(ir_lines, make_instruction_pushl(one));
			break;

		case TERM_NULL:
		case TERM_FALSE:
			append_element(ir_lines, make_instruction_pushl(zero));
			break;

		case TERM_PARENTESES: // parentheses just parses
			IR_builder_expression(term->value.exp);
			break;

		case TERM_VAR:
			variable = IR_builder_variable(term->value.var);
			append_element(ir_lines, make_instruction_pushl(variable));
			break;

		case TERM_ACT_LIST:
			symbol = getSymbol(term->symboltable, term->value.term_act_list
					.id);
			GET_FUNCTION_LABEL(functionLabel,symbol->name,symbol->offset);

			callScopeId = term->symboltable->id;
			definedScopeId = symbol->tableId;

			// push functionParameters on stack recursively
			IR_builder_act_list(term->value.term_act_list.actlist);

			if ( !stackIsEmpty(function_stack) &&
					strcmp(funcStackPeek(function_stack)->head->id,
						   term->value.term_act_list.id) == 0 ) {
				// recursion here just have to push the same base
				// pointer again

				append_element(ir_lines, make_instruction_pushl(
						make_argument_address(8, ebp)));
					// pass the previous base pointer
			} else if( callScopeId > definedScopeId ) {
				// we have to to fetch the right base pointer e.g.: the base
				// pointer of the scope where the function is defined
				append_element(ir_lines, make_instruction_movl(
						make_argument_address(8, ebp), eax));
				callScopeId--;

				while ( definedScopeId != callScopeId) {
					// get previous base pointer by iteration
					append_element(ir_lines, make_instruction_movl(
							make_argument_address(8,eax), eax));
					callScopeId--;
				}

				append_element(ir_lines, pushEax);
			} else if ( callScopeId == definedScopeId ) {
				// top level, we just push the base pointer on the stack
				append_element(ir_lines, pushEbp);
			}

			append_element(ir_lines, make_instruction_call(
					make_argument_label(functionLabel)));

			// stack clean up for function functionParameters plus 1 for
			// static link
			add_to_stack_pointer(symbol->noParameters + 1);

			append_element(ir_lines, pushEax);
			break;

		case TERM_NOT:
			IR_builder_term(term->value.term);
			append_element(ir_lines,popEbx);

			append_element(ir_lines,make_instruction_xor(one, ebx));
			// xor the term expression with true (1) to get the negated value

			append_element(ir_lines,pushEbx);
			break;

		case TERM_UMINUS:
			IR_builder_term(term->value.term);
			append_element(ir_lines,popEbx);

			append_element(ir_lines, make_instruction_negl(ebx));
			// two's complement negation

			append_element(ir_lines, pushEbx);
			break;

		case TERM_ABS:
			IR_builder_expression(term->value.exp);

			if ( term->value.exp->symboltype->type == SYMBOL_INT ) {
				append_element(ir_lines, popEbx);

                GET_FLOW_CONTROL_LABEL(positiveNumberLabel, "posNum",
                                       GET_NEXT_LABEL_ID);

				append_element(ir_lines,make_instruction_cmp(zero, ebx));
				append_element(ir_lines,make_instruction_JGE(
						positiveNumberLabel));
				// jump to end if number is positive

				append_element(ir_lines, make_instruction_negl(ebx));
				// false negates the argument as two's complement

				append_element(ir_lines, pushEbx);

				append_element(ir_lines,
							   make_instruction_label(positiveNumberLabel));

				append_element(ir_lines, pushEbx);

			} else if ( term->value.exp->symboltype->type == SYMBOL_ARRAY ) {

				append_element(ir_lines, popEax);
				// may be variable
				append_element( ir_lines, make_instruction_xor(ecx, ecx));

				append_element( ir_lines, make_instruction_pushl(
						make_argument_indexing(NULL, eax, ecx)));
				// gets the first element of the array where the size is
				// stored
			}
			break;

		default:
			break;
	}
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
			IR_builder_expression(expList->exp);
			break;
		case EXP_LIST_LIST:
			IR_builder_expression(expList->exp);
			IR_builder_expression_list(expList->explist);
			break;
	}
}

/* Adding allocation of local variables, this is by convention 
 *	a subtraction of the stack pointer
 */
IR_INSTRUCTION *local_variable_allocation(SYMBOL_TABLE *currentScope) {

	if (currentScope->localVars > 0){
		IR_INSTRUCTION *instr = make_instruction_subl(
				make_argument_constant(currentScope->localVars), esp);
		append_element(ir_lines, instr);
		return instr; 
		// reset counter at end of function
	}
	return NULL;
}

void negative_array_size_check(int lineno, ARGUMENT *arraySize) {

    char *notNegativeSize;
    GET_FLOW_CONTROL_LABEL(notNegativeSize, "notNegSize", GET_NEXT_LABEL_ID);

	// assume that arraySize is in register
	append_element(ir_lines, make_instruction_cmp(zero, arraySize));
	append_element(ir_lines, make_instruction_JGE(notNegativeSize));

	halt_for_error("$error.NEGSIZE", RUNTIME_ERROR_NEGSIZE, lineno);

	append_element(ir_lines, make_instruction_label(notNegativeSize));
}

void out_of_bounds_runtime_check( int lineno, ARGUMENT* variable,
								  ARGUMENT *index ) {
    int sharedId = GET_NEXT_LABEL_ID;

	char *notOutOfBoundsLabel;
	char *outOfBoundsLabel;
    GET_FLOW_CONTROL_LABEL(notOutOfBoundsLabel,"notOutBounds", sharedId);
    GET_FLOW_CONTROL_LABEL(outOfBoundsLabel,"outBounds", sharedId);

	// index < 0
	append_element(ir_lines, make_instruction_cmp(zero, index));
	append_element(ir_lines, make_instruction_jl(outOfBoundsLabel));

	// get length of the array
	append_element(ir_lines, make_instruction_xor(ecx, ecx));
	append_element(ir_lines, make_instruction_movl(
			make_argument_indexing(NULL, variable, ecx), ecx));

	// index >= length of array
	append_element(ir_lines, make_instruction_cmp(ecx, index));
	append_element(ir_lines, make_instruction_JGE(outOfBoundsLabel));

	// all check passed
	append_element(ir_lines, make_instruction_jmp(notOutOfBoundsLabel));

	append_element(ir_lines, make_instruction_label(outOfBoundsLabel));

	halt_for_error("$error.OUTBOUNDS", RUNTIME_ERROR_OUTBBOUNDS, lineno);

	append_element(ir_lines, make_instruction_label(notOutOfBoundsLabel));
}

void out_of_memory_runtime_check( int lineno, ARGUMENT *increase ) {

	char *notOutOfMemoryLabel;
    GET_FLOW_CONTROL_LABEL(notOutOfMemoryLabel,"notOutMem",
                           GET_NEXT_LABEL_ID);

	append_element(ir_lines, make_instruction_movl(heapAddress, edx));
	append_element(ir_lines, make_instruction_addl(
			make_argument_constant(MAX_HEAP_SIZE), edx));

	append_element(ir_lines, make_instruction_movl(heapFreePointer,eax));

	append_element(ir_lines, make_instruction_addl(increase, eax));

	append_element(ir_lines, make_instruction_cmp(eax, edx));

	append_element(ir_lines, make_instruction_jg(notOutOfMemoryLabel));

	halt_for_error("$error.OUTMEM", RUNTIME_ERROR_OUTMEM, lineno);

	append_element(ir_lines, make_instruction_label(notOutOfMemoryLabel));
}

void null_pointer_runtime_check( int lineno, ARGUMENT *variable ) {

	char *notNullLabel;
    GET_FLOW_CONTROL_LABEL(notNullLabel, "notNull", GET_NEXT_LABEL_ID);

	append_element(ir_lines, make_instruction_cmp(zero, variable));
	append_element(ir_lines, make_instruction_jne(notNullLabel));

	halt_for_error("$error.NULL", RUNTIME_ERROR_NULL, lineno);

	append_element(ir_lines, make_instruction_label(notNullLabel));

}

void division_by_zero_runtime_check( int lineno, ARGUMENT *denominator ) {

	char *notZeroDenominator;
    GET_FLOW_CONTROL_LABEL(notZeroDenominator, "notZeroDiv",
                           GET_NEXT_LABEL_ID);

	append_element(ir_lines, make_instruction_cmp(zero, denominator));
	append_element(ir_lines, make_instruction_jne(
			notZeroDenominator));
	// denominator has to be check if zero

	halt_for_error("$error.DIVZERO", RUNTIME_ERROR_DIVZERO, lineno);

	append_element(ir_lines, make_instruction_label(
			notZeroDenominator));
}

/*
 * prints a error message and stops the runtime process
 * errorMessage should match a error label in the data section
 */
void halt_for_error(char *errorMessageCode, int signalCode, int lineno) {

    // all error messages use this function
    has_errors = 1;

	append_element(ir_lines,make_instruction_pushl(
			make_argument_constant(lineno)));

	append_element(ir_lines,make_instruction_pushl(
			make_argument_label(errorMessageCode)));

	append_element(ir_lines, make_instruction_call(printfLabel));
	add_to_stack_pointer(2);

	exit_assembler(signalCode);
}

/*
 * Works much like "exit()" in C.
 */
void exit_assembler(int signalCode) {
	append_element(ir_lines, make_instruction_movl(
			make_argument_constant(signalCode), ebx));

	append_element(ir_lines, make_instruction_movl(one, eax));

	append_element(ir_lines, make_instruction_intcode("0x80"));
}

void caller_save(){

	append_element(ir_lines, pushEcx);
	append_element(ir_lines, pushEdx);

}

void caller_restore(){

	append_element(ir_lines, popEdx);
	append_element(ir_lines, popEcx);

}

void callee_start(){

	append_element(ir_lines, pushEbp);
	append_element(ir_lines, make_instruction_movl(esp, ebp));

}

void callee_end(){

	append_element(ir_lines, make_instruction_movl(ebp, esp));
	append_element(ir_lines, popEbp);

}

void callee_save(){

	append_element(ir_lines, pushEbx);
	append_element(ir_lines, pushEsi);
	append_element(ir_lines, pushEdi);
}

void callee_restore(){

	append_element(ir_lines, popEdi);
	append_element(ir_lines, popEsi);
	append_element(ir_lines, popEbx);

}

void function_prolog(SYMBOL_TABLE *currentScope) {
	callee_start();
	callee_save();
	local_variable_allocation(currentScope);
	caller_save();

}

void function_epilog() {
	callee_restore();
	caller_restore();
	callee_end();
}

void add_to_stack_pointer(int i){

	append_element(ir_lines, make_instruction_addl(
			make_argument_constant(i * WORD_SIZE), esp));

}

void add_print_forms(){
	append_element(ir_lines, make_instruction_directive(
			"form.NUM: \n\t.string \"%d\\n\" "));

	append_element(ir_lines, make_instruction_directive(
			"form.TRUE: \n\t.string \"TRUE\\n\" "));

	append_element(ir_lines, make_instruction_directive(
			"form.FALSE: \n\t.string \"FALSE\\n\" "));

	append_element(ir_lines, make_instruction_directive(
			"form.NULL: \n\t.string \"NULL\\n\" "));
}

void add_error_forms(){
	append_element(ir_lines, make_instruction_directive(
			"error.DIVZERO: \n\t.string \"Error at line %i: "
					"Division by zero\\n\" ")
	);

	append_element(ir_lines, make_instruction_directive(
			"error.OUTMEM: \n\t.string \"Error at line %i: "
					"Cannot allocate; out of memory\\n\" ")
	);

	append_element(ir_lines, make_instruction_directive(
			"error.NULL: \n\t.string \"Error at line %i: "
					"Null pointer exception\\n\" ")
	);

	append_element(ir_lines, make_instruction_directive(
			"error.OUTBOUNDS: \n\t.string \"Error at line %i: "
					"Index out of bounds\\n\" ")
	);

	append_element(ir_lines, make_instruction_directive(
			"error.NEGSIZE: \n\t.string \"Error at line %i: "
					"Cannot allocate arrays of negative size\\n\" ")
	);
}

int is_already_defined(linked_list *definedList, char *labelName) {

	linked_list *iterator = definedList->next;

	while (iterator != definedList) {

		if( strcmp(labelName, (char*)iterator->data) == 0 ){
			return 1;
		}
		iterator = iterator->next;
	}
	return 0;
}

// builds the data section at the end of the file
// cannot build in top because data_lines is not filled
void build_data_section() {

    int total_heap_data = get_length(data_lines);

    if (total_heap_data > 0 || has_errors || has_prints ) {
        append_element(ir_lines, make_instruction_directive(".data"));
    }

    if ( has_prints ) {
        add_print_forms();
    }

    if ( has_errors ) {
        add_error_forms();
    }

	if ( total_heap_data > 0 ) {

        // if there is allocation to the heap or a function is declared
        // (need heap for the static link)
        append_element(ir_lines, make_instruction_space(
                make_argument_label("heap."),
                make_argument_plain_constant(MAX_HEAP_SIZE)));

        append_element(ir_lines, make_instruction_space(
                heapFreePointer, wordSizePointer));

		// make pointers to records / arrays in heap
		linked_list *temp;
		temp = data_lines->next;

		linked_list *definedTypes = initialize_list();

		while ( temp != data_lines ) { // making label pointers for allocated 
										// items
			VAR_TYPE *var_type = (VAR_TYPE *) temp->data;
			SYMBOL *symbol = getSymbol(var_type->symboltable, var_type->id);

			if ( symbol != NULL && (symbol->symbolType->type == SYMBOL_ARRAY
					 || symbol->symbolType->type == SYMBOL_RECORD) ) {

				if ( !is_already_defined(definedTypes, var_type->id) ) {
					// check if the type is already defined
					append_element(ir_lines, make_instruction_space(
							make_argument_label(var_type->id),
							wordSizePointer));

					append_element(definedTypes, var_type->id);
				}
			}

			temp = temp->next;
		}
		
		terminate_list(&data_lines);
	}
}
