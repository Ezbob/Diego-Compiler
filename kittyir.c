#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "kittyir.h"
#include "irInstructions.h"

static int current_label = 0;
static int function_label = 0;
static int number_of_scopes = 0;

#define GET_NEXT_LABEL_ID (current_label++)
#define GET_NEXT_FUNCTION_ID (function_label++)
#define NEW_SCOPE (number_of_scopes++)

static linked_list *ir_lines; // plug IR code in here
static linked_list *data_lines; // for allocates


ARGUMENT *eax, *ebx, *ecx, *edx, *edi, *esi, *ebp, *esp;

void init_registers() {
	eax = make_argument_register(r_eax, "eax");
	ebx = make_argument_register(r_ebx, "ebx");
	ecx = make_argument_register(r_ecx, "ecx");
	edx = make_argument_register(r_edx, "edx");
	edi = make_argument_register(r_edi, "edi");
	esi = make_argument_register(r_esi, "esi");
	ebp = make_argument_register(r_ebp, "ebp");
	esp = make_argument_register(r_esp, "esp");
}

void init_heap() {
	 if ( get_length(data_lines ) == 0 ) {

		 append_element(ir_lines, make_instruction_movl(
				 make_argument_label("$heap"),
				 make_argument_label("(heapNext)"))
		 );
	 }
}

void add_Static_Link( int id ) {

	append_element(ir_lines, make_instruction_pushl(ebx));

	append_element(ir_lines, make_instruction_movl(
			make_argument_constant(id), ebx));

	append_element(ir_lines, 
		make_instruction_movl(ebp,
			make_argument_indexing( 
				make_argument_label("staticLinks"), NULL , ebx)));

	append_element(ir_lines, make_instruction_popl(ebx));
}

linked_list *IR_build( BODY *program ) {
	fprintf(stderr, "Initializing intermediate code generation phase\n");
	ir_lines = initialize_list();
	data_lines = initialize_list();
	init_registers();

	// adding text section for completion
	append_element(ir_lines, make_instruction_directive(".text"));

	IR_builder_decl_list(program->decl_list);

	// make ".globl main" directive
	append_element(ir_lines, make_instruction_directive(".globl main"));

	// make "main:" label line
	append_element(ir_lines, make_instruction_label("main"));

	NEW_SCOPE;

	callee_start();
	callee_save();

	local_variable_allocation(program->symboltable);

	caller_save();
	IR_builder_statement_list(program->statement_list);

	callee_restore();
	caller_restore();
	callee_end();

	append_element(ir_lines, make_instruction_movl(
		make_argument_constant(0), eax));

	program->symboltable->localVars = 0; // resetting local variables counter
	append_element(ir_lines, make_instruction_ret());

	build_data_section();

	return ir_lines;
}

void IR_builder_function(FUNC *func) {

	int functionId = GET_NEXT_FUNCTION_ID;
	char *functionStartLabel = NEW_LABEL;
	char *functionEndLabel = NEW_LABEL;

	SYMBOL *symbol = getSymbol(func->symboltable, func->head->id);

	sprintf(functionStartLabel, "func%d", functionId);
	sprintf(functionEndLabel,"endFunc%d", functionId);

	strcpy(symbol->uniqueName, functionStartLabel);

	sprintf(functionStartLabel, "%s", functionStartLabel);
	NEW_SCOPE;

	// move the handling of the declaration list here instead of the body to
	// avoid nested function getting generated inside each others 
	IR_builder_decl_list(func->body->decl_list);

	// start function label
	append_element(ir_lines, make_instruction_label(functionStartLabel));

	IR_builder_body(func->body);

	// end of function label
	append_element(ir_lines, make_instruction_label(functionEndLabel));

	callee_restore();
	caller_restore();
	callee_end();
	func->symboltable->localVars = 0; // reset local variables in scope 

	append_element(ir_lines, make_instruction_ret());

}

void IR_builder_body (BODY *body) {
	callee_start(); // shift in stackframe
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
	int numberOfRecordMembers;

	char *trueWhileString;
	char *endLabelString;
	char *elseLabel;
	char *printLabel;
	char *falseLabel;
	char *trueLabel;

	IR_INSTRUCTION *pushForm;
	ARGUMENT *variable;

	switch(st->kind){

		case STATEMENT_RETURN:	
			IR_builder_expression(st->value.exp);
			append_element(ir_lines, make_instruction_popl(eax));
			break;

		case STATEMENT_WRITE:
			caller_save();
			IR_builder_expression(st->value.exp);
			append_element(ir_lines, make_instruction_popl(eax));
				// result from expression
			switch(st->value.exp->symboltype->type){
				case SYMBOL_BOOL:
					tempLabelCounter = GET_NEXT_LABEL_ID;

					falseLabel = NEW_LABEL;
					trueLabel = NEW_LABEL;
					printLabel = NEW_LABEL;

					sprintf(falseLabel,"bfalse%d", tempLabelCounter);
					sprintf(trueLabel, "btrue%d", tempLabelCounter);
					sprintf(printLabel,"printbool%d", tempLabelCounter);

					// compare boolean value to true
					append_element(ir_lines, make_instruction_cmp(
							make_argument_constant(1), eax));

					// true has to be printed?
					append_element(ir_lines,
								   make_instruction_jne(falseLabel));

					// making a push to the stack with result of expression
					append_element(ir_lines, make_instruction_pushl(eax));

					// true case here
					append_element(ir_lines, make_instruction_pushl(
							make_argument_label("$formTRUE")));

					// jump to printf call
					append_element(ir_lines,
								   make_instruction_jmp(printLabel));

					// false label section
					append_element(ir_lines,
								   make_instruction_label(falseLabel));

					// making a push to the stack with result of expression
					append_element(ir_lines, make_instruction_pushl(eax));

					// false case here
					append_element(ir_lines, make_instruction_pushl(
							make_argument_label("$formFALSE")));

					// printing section
					append_element(ir_lines, make_instruction_label(
							printLabel));
					break;

				case SYMBOL_INT:
				case SYMBOL_NULL:

					append_element(ir_lines, make_instruction_pushl(eax));

					if (st->value.exp->value.term->kind == TERM_NULL) {
						pushForm = make_instruction_pushl(
								make_argument_label("$formNULL"));
					} else {
						pushForm = make_instruction_pushl(
								make_argument_label("$formNUM"));
					}

					append_element(ir_lines, pushForm);
					break;

				case SYMBOL_ARRAY:
					append_element(ir_lines, make_instruction_pushl(eax));

					append_element(ir_lines, make_instruction_pushl(
							make_argument_label("$formNUM")));
					break;
				default:
					break;
			}
			// call to print
			append_element( ir_lines, make_instruction_call(
					make_argument_label("printf")));

			add_to_stack_pointer(2); // clean up
			caller_restore();
			break;

		case STATEMENT_ASSIGN:
			IR_builder_expression(st->value.statement_assign.exp);
			variable = IR_builder_variable(st->value.statement_assign.var);

			append_element(ir_lines, make_instruction_popl(ebx));
				// exp

			append_element(ir_lines, make_instruction_movl(ebx, variable));
			break;

		case STATEMENT_IFBRANCH:
			// generate code for boolean expression(s)
			IR_builder_expression(st->value.statement_ifbranch.exp);

			append_element(ir_lines, make_instruction_popl(eax));

			elseLabel = NEW_LABEL;
			endLabelString = NEW_LABEL;

			int labelNo = GET_NEXT_LABEL_ID;

			sprintf(elseLabel, "else%d", labelNo);
			sprintf(endLabelString, "endIf%d", labelNo);

			//Comparison with "true" boolean value
			append_element(ir_lines, make_instruction_cmp(
					make_argument_constant(1), eax));

			if(st->value.statement_ifbranch.opt_else->kind != OPT_ELSE_EMPTY){
				// if not equal goto else part
				append_element(ir_lines, make_instruction_jne(elseLabel));
			} else {
				// if not equal goto end-of-if
				append_element(ir_lines,
							   make_instruction_jne(endLabelString));
			}

			IR_builder_statement(st->value.statement_ifbranch.statement);
				// build statements in if-case

			if(st->value.statement_ifbranch.opt_else->kind != OPT_ELSE_EMPTY){

				// we have to jump over
				//else when if-case is true
				append_element(ir_lines,
							   make_instruction_jmp(endLabelString));

				// make else-label
				append_element(ir_lines, make_instruction_label(elseLabel));

				IR_builder_statement( // build else statements
					st->value.statement_ifbranch.opt_else->statement);
			}

			// end-of-if label
			append_element(ir_lines, make_instruction_label(endLabelString));
			break;

		case STATEMENT_ALLOCATE:

			switch(st->value.statement_allocate.var->symboltype->type){

				case SYMBOL_ARRAY:
					// assume that the checker has checked if "length of" is
					// present
					init_heap();

					// put check out of memory here

					variable = IR_builder_variable(
							st->value.statement_allocate.var);

					append_element(ir_lines, make_instruction_leal(
							variable,eax));

					// allocate space to array
					append_element(ir_lines, make_instruction_movl(
							make_argument_label("$heapNext"),
							eax));

					// xored to get zero, aka the first index
					append_element(ir_lines, make_instruction_xor(ebx, ebx));

					IR_builder_opt_length(st->value.statement_allocate
												  .opt_length);
					append_element(ir_lines, make_instruction_popl(ecx));

					// move the array size to the first index
					append_element(ir_lines, make_instruction_movl(ecx,
							make_argument_indexing(NULL, eax, ebx)));

					append_element(ir_lines,
								   make_instruction_incl(ecx));

					// getting array size in bytes
					append_element(ir_lines, make_instruction_imul(
							make_argument_constant(WORD_SIZE), ecx));

					// update the heap free pointer
					append_element(ir_lines, make_instruction_addl(ecx,
							make_argument_label("(heapNext)")));

					append_element(data_lines, st);
					break;

				case SYMBOL_RECORD:
					init_heap();

					variable = IR_builder_variable(st->value.
							statement_allocate.var);
					append_element(ir_lines, make_instruction_leal(variable,
																   eax));

					append_element(ir_lines, make_instruction_movl(
							make_argument_label("$heapNext"), eax));

					numberOfRecordMembers = st->value.statement_allocate.
							var->symboltype->arguments;

					// we need number of members in record

					append_element(ir_lines, make_instruction_movl(
							make_argument_constant(numberOfRecordMembers)
							, ebx));

					append_element(ir_lines, make_instruction_imul(
							make_argument_constant(WORD_SIZE), ebx));

					// add to the next pointer
					append_element(ir_lines, make_instruction_addl(ebx,
							make_argument_label("(heapNext)")));

					append_element(data_lines,st);
					break;
				default:
					break;
				}
			break;

		case STATEMENT_WHILE:

			tempLabelCounter = GET_NEXT_LABEL_ID;

			trueWhileString = NEW_LABEL;
			endLabelString = NEW_LABEL;

			sprintf(trueWhileString, "whileStart%d", current_label);
			sprintf(endLabelString, "whileEnd%d", current_label);

			// while-start label insert
			append_element(ir_lines, make_instruction_label(trueWhileString));

			// evaluating expressions
			IR_builder_expression(st->value.statement_while.exp);
			append_element(ir_lines, make_instruction_popl(eax));

			//Compare evaluated expression with true
			append_element(ir_lines, make_instruction_cmp(
					make_argument_constant(1), eax));

			// jump to end if while condition is false
			append_element(ir_lines, make_instruction_jne(endLabelString));

			// generate code for statements
			IR_builder_statement(st->value.statement_while.statement);

			// repeating statement jump
			append_element(ir_lines, make_instruction_jmp(trueWhileString));

			// insertion of while-end
			append_element(ir_lines, make_instruction_label(endLabelString));
			break;

		case STATEMENT_LISTS:
			IR_builder_statement_list(st->value.statement_list);
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
				offsetValue = (symbol->offset + 1) * WORD_SIZE;

				if ( symbol->symbolType->type == SYMBOL_ARRAY ||
						symbol->symbolType->type == SYMBOL_RECORD ) {

					// They're on the heap so we just use labels
					result = make_argument_label(var->id);

				} else if (symbol->tableId != var->symboltable->id) {
					// basically, if variable is not in current,
					// use static link


						// plus one because return address

					append_element(ir_lines, make_instruction_movl(
							make_argument_constant(symbol->tableId), ecx));

					append_element(ir_lines, make_instruction_movl(
							make_argument_indexing( make_argument_label(
									"staticLinks"), NULL, ecx ), ebx ));

					if ( symbol->symbolKind == LOCAL_VARIABLE_SYMBOL ) {
						result = make_argument_static( -1 * offsetValue );
					} else {
						result = make_argument_static( offsetValue );
					}

				} else {
					if ( symbol->symbolKind == LOCAL_VARIABLE_SYMBOL ) {
						result = make_argument_address( -1 * offsetValue );

					} else {
						result = make_argument_address( offsetValue );
					}

				}
			}

			return result;

		case VAR_ARRAY:
			base = IR_builder_variable(var->value.var_array.var);
			IR_builder_expression(var->value.var_array.exp);

			append_element(ir_lines, make_instruction_movl(base, esi));
				// exp
			append_element(ir_lines, make_instruction_popl(edi));
				// var

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
				offset = make_argument_constant(symbol->offset);
				// member index in the record as argument
			}
			append_element(ir_lines, make_instruction_movl(base,esi));

			append_element(ir_lines, make_instruction_movl(offset, edi));

				// returns much the same as arrays
			return make_argument_indexing(NULL, esi, edi);
	}
	return NULL;
}

void IR_builder_expression ( EXPRES *exp ) {

	int tempLabelCounter = 0;
	char *notZeroDenominator;
	ARGUMENT *truth;

	if ( exp->kind != EXPRES_TERM ) {
		IR_builder_expression(exp->value.sides.left);
		IR_builder_expression(exp->value.sides.right);
	}

	switch(exp->kind){
		case EXPRES_TERM: 
			IR_builder_term(exp->value.term);
			break;

		case EXPRES_PLUS:
			append_element(ir_lines, make_instruction_popl(ebx));
				// rhs
			append_element(ir_lines, make_instruction_popl(ecx));
				// lhs
 			append_element(ir_lines, make_instruction_addl(ebx, ecx));

			append_element(ir_lines, make_instruction_pushl(ecx));
			break;

		case EXPRES_MINUS:
			append_element(ir_lines, make_instruction_popl(ebx));
			// rhs
			append_element(ir_lines, make_instruction_popl(ecx));
			// lhs
			append_element(ir_lines, make_instruction_subl(ebx, ecx));
			append_element(ir_lines, make_instruction_pushl(ecx));
			break;

		case EXPRES_TIMES:
			append_element(ir_lines, make_instruction_popl(ebx));
			// rhs
			append_element(ir_lines, make_instruction_popl(ecx));
			// lhs
			append_element(ir_lines, make_instruction_imul(ebx, ecx));
			append_element(ir_lines, make_instruction_pushl(ecx));
			break;

		case EXPRES_DIVIDE:

			notZeroDenominator = NEW_LABEL;
			sprintf(notZeroDenominator, "NotZeroDen%d", GET_NEXT_LABEL_ID);

			append_element(ir_lines, make_instruction_popl(ebx));
			// rhs
			append_element(ir_lines, make_instruction_popl(eax));
			// lhs

			append_element(ir_lines, make_instruction_cmp(
					make_argument_constant(0), ebx));
			append_element(ir_lines, make_instruction_jne(
					notZeroDenominator));
				// denominator has to be check if zero

			build_exception("0x80");

			append_element(ir_lines, make_instruction_label(
					notZeroDenominator));

			append_element(ir_lines, make_instruction_pushl(edx));
				// Saving edx register; contains modulo after division

			append_element(ir_lines, make_instruction_xor(edx, edx));
				// Clear edx for modulo

			append_element(ir_lines, make_instruction_div(ebx));
				// divide eax with eax to get result in eax

			append_element(ir_lines, make_instruction_popl(edx));
			// restore the saved registers

			append_element(ir_lines, make_instruction_pushl(eax));
				// result: "quotient" on the stack
			break;

		case EXPRES_EQ:
		case EXPRES_NEQ:
		case EXPRES_GREATER:
		case EXPRES_LESS:
		case EXPRES_LEQ:
		case EXPRES_GEQ:
			tempLabelCounter = GET_NEXT_LABEL_ID;

			char *boolTrueLabel = NEW_LABEL;
			char *boolEndLabel = NEW_LABEL;

			sprintf(boolTrueLabel, "booOPtrue%d", tempLabelCounter);
			IR_INSTRUCTION *trueLabel = make_instruction_label(boolTrueLabel);

			sprintf(boolEndLabel, "boolOPend%d", tempLabelCounter);
			IR_INSTRUCTION *endLabel = make_instruction_label(boolEndLabel);

			append_element(ir_lines, make_instruction_popl(ebx));
				// rhs
			append_element(ir_lines, make_instruction_popl(ecx));
				// lhs

			append_element(ir_lines, make_instruction_cmp( ebx, ecx ));
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
					break;
			}

			append_element(ir_lines, trueJump);
				// the jump to true instruction

			append_element(ir_lines, make_instruction_pushl(
					make_argument_constant(0)));
				// the false case
			append_element(ir_lines, make_instruction_jmp(boolEndLabel));

			append_element(ir_lines, trueLabel);

			append_element(ir_lines, make_instruction_pushl(
					make_argument_constant(1)));
				// the true case

			append_element(ir_lines, endLabel);
			break;

		case EXPRES_AND:
			tempLabelCounter = GET_NEXT_LABEL_ID;

			char *andFalseLabel = NEW_LABEL;
			char *andEndLabel = NEW_LABEL;
			sprintf(andFalseLabel, "ANDfalse%d", tempLabelCounter);
			sprintf(andEndLabel, "ANDend%d", tempLabelCounter);

			truth = make_argument_constant(1);
			IR_INSTRUCTION *jumpToFalse = make_instruction_jne(andFalseLabel);

			append_element(ir_lines, make_instruction_popl(ebx));
			// rhs
			append_element(ir_lines, make_instruction_popl(ecx));
			// lhs

			append_element(ir_lines, make_instruction_cmp(truth, ecx));
			append_element(ir_lines, jumpToFalse);
			append_element(ir_lines, make_instruction_cmp(truth, ebx));
			append_element(ir_lines, jumpToFalse);
				// check if both arguments evaluates to true

			append_element(ir_lines,make_instruction_pushl(truth));
			append_element(ir_lines, make_instruction_jmp(andEndLabel));
				// in case both arguments are true

			append_element(ir_lines, make_instruction_label(andFalseLabel));
			append_element(ir_lines, make_instruction_pushl(
					make_argument_constant(0)));
				// in case one of the arguments are false

			append_element(ir_lines, make_instruction_label(andEndLabel));
			break;
		case EXPRES_OR:
			tempLabelCounter = GET_NEXT_LABEL_ID;

			char *orTrueLabel = NEW_LABEL;
			char *orEndLabel = NEW_LABEL;
			sprintf(orTrueLabel, "ORtrue%d", tempLabelCounter);
			sprintf(orEndLabel, "ORend%d", tempLabelCounter);

			truth = make_argument_constant(1);

			IR_INSTRUCTION *jumpToTrue = make_instruction_je(orTrueLabel);

			append_element(ir_lines, make_instruction_popl(ebx));
			// rhs
			append_element(ir_lines, make_instruction_popl(ecx));
			// lhs

			append_element(ir_lines, make_instruction_popl(ecx));

			append_element(ir_lines, make_instruction_cmp(truth, ecx));
			append_element(ir_lines, jumpToTrue);
			append_element(ir_lines,make_instruction_cmp(truth, ebx));
			append_element(ir_lines, jumpToTrue);
				// like in "and" we compare both arguments but jumps to true
				// case instead of false case

			append_element(ir_lines, make_instruction_pushl(
					make_argument_constant(0)));
				// false case
			append_element(ir_lines, make_instruction_jmp(orEndLabel));

			append_element(ir_lines, make_instruction_label(orTrueLabel));
			append_element(ir_lines, make_instruction_pushl(truth));
				// true case

			append_element(ir_lines, make_instruction_label(orEndLabel));
			break;
	}

}

void IR_builder_term ( TERM *term) {

	SYMBOL *symbol;
	ARGUMENT *variable;
	char *positiveNumberLabel;

	switch(term->kind){
		case TERM_NUM:
			append_element(ir_lines, make_instruction_pushl(
					make_argument_constant(term->value.intconst)));
			break;

		case TERM_TRUE:
			append_element(ir_lines, make_instruction_pushl(
					make_argument_constant(1) ));
			break;

		case TERM_NULL:
		case TERM_FALSE:
			append_element(ir_lines, make_instruction_pushl(
					make_argument_constant(0) ));
			break;

		case TERM_PARENTESES: // parentheses just parses
			IR_builder_expression(term->value.exp);
			break;

		case TERM_VAR:
			variable = IR_builder_variable(term->value.var);
			append_element(ir_lines, make_instruction_pushl(variable));
			break;

		case TERM_ACT_LIST:
			add_Static_Link(term->symboltable->id);

			symbol = getSymbol(term->symboltable, term->value.
					term_act_list.id);

			// push functionParameters on stack recursively
			IR_builder_act_list(term->value.term_act_list.actlist);

			append_element(ir_lines, make_instruction_call(
					make_argument_label(symbol->uniqueName)));

			// stack clean up for function functionParameters
			add_to_stack_pointer(symbol->noParameters);

			append_element(ir_lines, make_instruction_pushl(eax));
			break;

		case TERM_NOT:

			append_element(ir_lines,make_instruction_popl(ebx));

			append_element(ir_lines,make_instruction_xor(
					make_argument_constant(1), ebx));
			// xor the term expression with true (1) to get the negated value

			append_element(ir_lines,make_instruction_pushl(ebx));
			break;

		case TERM_ABS:
			IR_builder_expression(term->value.exp);

			if ( term->symboltype->type == SYMBOL_INT ) {
				append_element(ir_lines, make_instruction_popl(ebx));

				positiveNumberLabel = NEW_LABEL;
				sprintf(positiveNumberLabel, "posNum%i", GET_NEXT_LABEL_ID);

				append_element(ir_lines,make_instruction_cmp(
						make_argument_constant(0), ebx));
				append_element(ir_lines,make_instruction_JGE(
						positiveNumberLabel));
				// jump to end if number is positive

				append_element(ir_lines, make_instruction_negl(ebx));
				// false negates the argument as two's complement

				append_element(ir_lines, make_instruction_pushl(ebx));

				append_element(ir_lines,
							   make_instruction_label(positiveNumberLabel));

				append_element(ir_lines, make_instruction_pushl(ebx));

			} else if ( term->symboltype->type == SYMBOL_ARRAY ) {

				append_element(ir_lines, make_instruction_popl(eax));
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

/*
 * Handy code sippet for throwing exceptions
 */
void build_exception(char *intCode) {
	append_element(ir_lines, make_instruction_movl(
			make_argument_constant(3), ebx));

	append_element(ir_lines, make_instruction_movl(
			make_argument_constant(1), eax));

	append_element(ir_lines, make_instruction_intcode(intCode));
}

void caller_save(){

	append_element(ir_lines, make_instruction_pushl(ecx));
	append_element(ir_lines, make_instruction_pushl(edx));

}

void caller_restore(){

	append_element(ir_lines, make_instruction_popl(edx));
	append_element(ir_lines, make_instruction_popl(ecx));

}

void callee_start(){

	append_element(ir_lines, make_instruction_pushl(ebp));
	append_element(ir_lines, make_instruction_movl(esp, ebp));

}

void callee_end(){

	append_element(ir_lines, make_instruction_movl(ebp, esp));
	append_element(ir_lines, make_instruction_popl(ebp));

}

void callee_save(){

	append_element(ir_lines, make_instruction_pushl(ebx));
	append_element(ir_lines, make_instruction_pushl(esi));
	append_element(ir_lines, make_instruction_pushl(edi));
}

void callee_restore(){

	append_element(ir_lines, make_instruction_popl(edi));
	append_element(ir_lines, make_instruction_popl(esi));
	append_element(ir_lines, make_instruction_popl(ebx));

}

void add_to_stack_pointer(int i){

	append_element(ir_lines, make_instruction_addl(
			make_argument_constant(i * WORD_SIZE), esp));

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
	append_element(ir_lines, make_instruction_space(
			make_argument_label("heap"),
			make_argument_label("4194304")
			));

	append_element(ir_lines, make_instruction_space(
			make_argument_label("heapNext"),
			make_argument_label("4")
			)
		);

	// make static link pointer (pointer to a array of static links)
	char *static_display_size = calloc(MAX_LABEL_SIZE, sizeof(char));
	sprintf(static_display_size,"%d", WORD_SIZE * number_of_scopes );

	// make static link pointer (pointer to a array of static links)
	append_element(ir_lines, make_instruction_space(
			make_argument_label("staticLinks"),
			make_argument_label(static_display_size)));


	if ( get_length(data_lines) > 0 ) {
		// make pointers to records / arrays in heap

		linked_list *temp;
		temp = data_lines->next;

		while ( temp != data_lines ) { // making label pointers for allocated 
										// items
			STATEMENT *st = (STATEMENT *) temp->data;

			if ( st->value.statement_allocate.var->kind == VAR_ID ) {
				append_element(ir_lines, make_instruction_space(
						make_argument_label(st->value.statement_allocate.
								var->id), make_argument_label("4")));
			}

			temp = temp->next;
		}
		
		terminate_list(&data_lines);
	}
}
