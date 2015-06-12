#include "kittycheck.h"
#include "kittycollect.h"
#include <stdio.h>

#define DEBUG_TYPE(symboltype) printf("BOOL>>> %i\n",symboltype->type \
			 == SYMBOL_BOOL); printf("INT>>> %i\n",symboltype->type \
				== SYMBOL_INT); \
			printf("ID>>> %i\n", symboltype->type == SYMBOL_ID); \
			printf("ARRAY>>> %i\n", symboltype->type == SYMBOL_ARRAY); \
			printf("RECORD>>> %i\n", symboltype->type == SYMBOL_RECORD); \
			printf("NULL>>> %i\n", symboltype->type == SYMBOL_NULL); \
			printf("UNKNOWN>>> %i\n", symboltype->type == SYMBOL_UNKNOWN); \
			printf("FUNCTION>>> %i\n", symboltype->type == SYMBOL_FUNCTION);

/*
	Structs to have their symboltypes set:
		terms,
		expressions, 
		functions, (done in ktype)
		head, (done in ktype)
		type, (done in ktype)
		var
	These structs are part of the function subtree:
		terms via expressions
		expressions via statements, terms, expression_lists, variable, 
			opt_length
		functions via decl_lists 
		head via functions
		type via var_type, declaration, head
		var via statements, term
*/

static int parameterCounter = 0;

void check_error_report(const char* errorMsg, int lineno) {
	if (lineno < 0){
		fprintf(stderr, "Error: %s \n",errorMsg);
		exit(1);
	}
	fprintf(stderr, "Error at line %i: %s \n",lineno,errorMsg);
	exit(1);
}

void begin_check(BODY *main) {

	fprintf(stderr, "%s\n", "Initializing typecheck phase");
	check_body(main);
}

void check_function(FUNC *func) {

	check_head(func->head);
	check_body(func->body);
}

void check_head (HEAD *header) {

	check_par_decl_list(header->pdlist);
	check_type(header->returntype);
}

void check_body (BODY *body) {

	check_decl_list(body->decl_list);
	check_statement_list(body->statement_list);
}

void check_type (TYPE *type) {
	switch(type->kind){
		case TYPE_ID:
			break;
		case TYPE_INT:
			break;
		case TYPE_BOOL:
			break;
		case TYPE_ARRAY:
			check_type(type->value.type);
			break;
		case TYPE_RECORD:
			check_var_decl_list(type->value.var_decl_list);
			break;
	}
}

void check_var_decl_list (VAR_DECL_LIST *vdecl) {

	switch(vdecl->kind){
		case VAR_DECL_LIST_LIST:
			check_var_decl_list(vdecl->var_decl_list);
			check_var_type(vdecl->var_type);
			break;
		case VAR_DECL_LIST_TYPE:
			check_var_type(vdecl->var_type);
			break;
	}
}

void check_var_type ( VAR_TYPE *vtype){
	check_type(vtype->type);
}

void check_decl_list ( DECL_LIST *dlst){
	switch(dlst->kind){
		case DECL_LIST_LIST:
			check_decl_list(dlst->decl_list);
			check_declaration(dlst->declaration);
			break;
		case DECL_LIST_EMPTY:
			break;
	}
}


void check_declaration ( DECLARATION *decl){
	switch(decl->kind){
		case DECLARATION_ID:
			check_type(decl->value.declaration_id.type);
			break;
		case DECLARATION_FUNC:
			check_function(decl->value.function);
			break;
		case DECLARATION_VAR:
			check_var_decl_list(decl->value.var_decl_list);
			break;
	}
}

void check_statement_list ( STATEMENT_LIST *slst){

	switch(slst->kind){
		case STATEMENT_LIST_STATEMENT:
			check_statement(slst->statement);
			break;

		case STATEMENT_LIST_LIST:
			check_statement(slst->statement);
			check_statement_list(slst->statement_list);
			break;
	}
}

void check_statement ( STATEMENT *st){

	SYMBOLTYPE *rightHand;
	SYMBOLTYPE *leftHand;

	switch(st->kind){
		case STATEMENT_RETURN:
			check_expression(st->value.statement_return.exp);

			// checking return expression against function header return type
			// (set in weeder)
			if (st->value.statement_return.exp->symboltype->type != 
				st->value.statement_return.function->symboltype->return_type
				->type ){
				check_error_report(
					"type error: Return type does not match function", 
					st->lineno);				
			}

			break;

		case STATEMENT_WRITE:
			check_expression(st->value.exp);

			if(st->value.exp->symboltype->type == SYMBOL_ARRAY &&
				 st->value.exp->value.term->kind != TERM_ABS){
					check_error_report("Only length of arrays can be written", 
					st->lineno);
			}
			break;

		case STATEMENT_ALLOCATE:
			
			check_variable(st->value.statement_allocate.var);
			check_opt_length(st->value.statement_allocate.opt_length);
			
			leftHand = st->value.statement_allocate.var->symboltype;

			if ( !(leftHand->type == SYMBOL_RECORD || 
				leftHand->type == SYMBOL_ARRAY) ) {
				check_error_report(
					"Cannot allocate to specific variable type", 
					st->lineno);
			}

			if(st->value.statement_allocate.opt_length->kind == 
				OPT_LENGTH_EMPTY && leftHand->type != SYMBOL_RECORD) {
				check_error_report(
					"Only records can be allocated without length", 
					st->lineno);
			}
			break;

		case STATEMENT_ASSIGN:
			check_variable(st->value.statement_assign.var);
			check_expression(st->value.statement_assign.exp);

			SYMBOL *temp_sym;
			SYMBOLTABLE *tmpTable;

			leftHand = st->value.statement_assign.var->symboltype;
			rightHand = st->value.statement_assign.exp->symboltype;

			if ( leftHand->type == SYMBOL_RECORD && rightHand->type ==
				SYMBOL_RECORD ) {
				
				if( compare_record_members(leftHand,rightHand) != 1) {
					check_error_report("Invalid assignment, type mismatch",
						st->lineno);
				}

			} else if ( leftHand->type == SYMBOL_ARRAY && rightHand->type == 
				leftHand->type ) { 

				leftHand = get_base_array_type(leftHand);
				rightHand = get_base_array_type(rightHand);

				/* checking if the base type and dimension of the arrays
					are the same */

				if ( leftHand->type != rightHand->type ||
					leftHand->arrayDim != rightHand->arrayDim ) {

					check_error_report("Invalid assignment; type mismatch",
						st->lineno);
				}

			} else if ( leftHand->type == SYMBOL_ARRAY ) {

				leftHand = get_base_array_type(leftHand);

				if ( leftHand->type != rightHand->type ){
					check_error_report("Invalid assignment; type mismatch",
						st->lineno);	
				}
				
			} else if ( rightHand->type == SYMBOL_ARRAY ){

				rightHand = get_base_array_type(rightHand);

				if ( leftHand->type != rightHand->type ){
					check_error_report("Invalid assignment; type mismatch",
						st->lineno);	
				}

			} else { //Standard check

				if( rightHand->type != leftHand->type ) {
					check_error_report(
						"Invalid assignment; type mismatch", st->lineno);
				} 
			}
			break;

		case STATEMENT_IFBRANCH:
			check_expression(st->value.statement_ifbranch.exp);
			check_statement(st->value.statement_ifbranch.statement);
			check_opt_else(st->value.statement_ifbranch.opt_else);

			if(st->value.statement_ifbranch.exp->
			   symboltype->type != SYMBOL_BOOL) {
				check_error_report("Expected condition to evaluate"
					" to a boolean value",
				st->lineno);
			}
			break;

		case STATEMENT_WHILE:
			check_expression(st->value.statement_while.exp);
			check_statement(st->value.statement_while.statement);
			if(st->value.statement_while.exp->symboltype->type 
				!= SYMBOL_BOOL) {
				check_error_report("Expected condition to"
					" evaluate to a boolean value",
				st->lineno);
			}
			break;

		case STATEMENT_LISTS:
			check_statement_list(st->value.statement_list);
			break;

	}
}

void check_opt_length ( OPT_LENGTH *oplen){

	switch(oplen->kind){
		case OPT_LENGTH_EXPRES:
			check_expression(oplen->exp);
			if(oplen->exp->symboltype->type != SYMBOL_INT){
				check_error_report("Expected length to evaluate to a"
					"integer value", oplen->lineno);
			}
			break;
		case OPT_LENGTH_EMPTY:
			break;
	}
}

void check_opt_else ( OPT_ELSE *opel){

	switch(opel->kind){
		case OPT_ELSE_STATEMENT:
			check_statement(opel->statement);
			break;
		case OPT_ELSE_EMPTY:
			break;
	}
}

int check_variable ( VAR *var){
	SYMBOL *symbol;
	SYMBOLTABLE *innerScope;
	int depth = 1;

	switch(var->kind){
		case VAR_ID:
			if((symbol = getSymbol(var->symboltable, var->id)) != NULL){
				var->symboltype = symbol->symboltype;
			} else {
				check_error_report("Symbol not recognized",var->lineno);
			}
			break;

		case VAR_ARRAY:
			depth += check_variable(var->value.var_array.var);
			check_expression(var->value.var_array.exp);

			//Expression must be evaluated to a integer
			if(var->value.var_array.exp->symboltype->type != SYMBOL_INT) {
				check_error_report("Expression must evaluate to int", 
					var->lineno);
			}

			//Variable must be array type
			if(var->value.var_array.var->symboltype->type != SYMBOL_ARRAY) {
				check_error_report("Variable is not an array", var->lineno);
			}

			int arrayDim = get_array_dim(var->value.var_array.
						var->symboltype);

			if ( depth == arrayDim ) { // where at the basic type
				var->symboltype = get_base_array_type(var->value.var_array.
						var->symboltype);
			} else {
				var->symboltype = var->value.var_array.var->symboltype;	
			}
			break;

		case VAR_RECORD:
			check_variable(var->value.var_record.var);

			if(var->value.var_record.var->symboltype->type == SYMBOL_RECORD) {
				
				innerScope = var->value.var_record.var->symboltype->child;

				if((symbol = getSymbol(innerScope, var->value.var_record
					.id)) != NULL){
					var->symboltype = symbol->symboltype;
				} else {
					check_error_report("Symbol not recognized",var->lineno);
				}	
			} else {
				check_error_report("Expected record",var->lineno);
			}
			break;
	}

	return depth;
}


void check_expression ( EXPRES *exp){
	SYMBOLTYPE *rightHand;
	SYMBOLTYPE *leftHand;

	// if it's not a term it got two sides
	if (exp->kind != EXPRES_TERM){
		// getting base SYMBOLTYPE of arrays for both sides if it's a array

		check_expression(exp->value.sides.right);
		check_expression(exp->value.sides.left);

		rightHand = get_base_array_type(
			exp->value.sides.right->symboltype);
		leftHand = get_base_array_type(
			exp->value.sides.left->symboltype);
	}

	switch(exp->kind){
		case EXPRES_TERM:
			check_term(exp->value.term);
			exp->symboltype = exp->value.term->symboltype;
			break;

		case EXPRES_PLUS:
		case EXPRES_MINUS:
		case EXPRES_DIVIDE:
		case EXPRES_TIMES:
			if(rightHand->type != SYMBOL_INT && 
			   leftHand->type != SYMBOL_INT ) {
				check_error_report("Expected integer expression", 
						exp->lineno);
			} else {
				exp->symboltype = make_SYMBOLTYPE(SYMBOL_INT);
			}
			break;

		case EXPRES_EQ:
		case EXPRES_NEQ:
			// expressions where the sides are the same type
			if( (rightHand->type == SYMBOL_INT || 
			   		leftHand->type == SYMBOL_INT) ||

			   	(rightHand->type == SYMBOL_BOOL || 
			   		leftHand->type == SYMBOL_BOOL) ||

			   	(rightHand->type == SYMBOL_ARRAY || 
			   		leftHand->type == SYMBOL_ARRAY) ||

			   	(rightHand->type == SYMBOL_RECORD ||
			   		leftHand->type == SYMBOL_RECORD) ||

			   	(rightHand->type == SYMBOL_NULL ||
			   		leftHand->type == SYMBOL_NULL) ) {

				exp->symboltype = make_SYMBOLTYPE(SYMBOL_BOOL);
				break;
			}

			// array and null
			if(	(rightHand->type == SYMBOL_ARRAY || 
			   		leftHand->type == SYMBOL_NULL) ||
			   	(rightHand->type == SYMBOL_NULL || 
			   		leftHand->type == SYMBOL_ARRAY ) ) { 

                exp->symboltype = make_SYMBOLTYPE(SYMBOL_BOOL);
				break;
			}

			// records and null
			if(
				(rightHand->type == SYMBOL_RECORD ||
			   		leftHand->type == SYMBOL_NULL ) || 
			   	(rightHand->type == SYMBOL_NULL || 
			   		leftHand->type == SYMBOL_RECORD ) ) {

                exp->symboltype = make_SYMBOLTYPE(SYMBOL_BOOL);
				break;
			}

			check_error_report("Invalid comparison types", exp->lineno);
			break;

		case EXPRES_GREATER:
		case EXPRES_LESS:
		case EXPRES_LEQ:
		case EXPRES_GEQ:
			if(rightHand->type != SYMBOL_INT &&
			   leftHand->type != SYMBOL_INT ) {
				check_error_report("Expected integer expression", 
						exp->lineno);
			} else {
				exp->symboltype = make_SYMBOLTYPE(SYMBOL_BOOL);
			}
			
			break;

		case EXPRES_AND:
		case EXPRES_OR:
			if(rightHand->type != SYMBOL_BOOL && 
			   leftHand->type != SYMBOL_BOOL) { 
				check_error_report("Expected boolean expression", 
						exp->lineno);
			} else {
				exp->symboltype = make_SYMBOLTYPE(SYMBOL_BOOL);
			}
			break;
	}
}

int check_term ( TERM *term ) {
	SYMBOLTYPE *symbolT;
	SYMBOL *symbol;
	SYMBOLTABLE *tmpTable;

	int count = 0;

	switch(term->kind){
		case TERM_VAR:	
			check_variable(term->value.var);
			term->symboltype = term->value.var->symboltype;
			break;

		case TERM_ACT_LIST:
			
			check_act_list(term->value.term_act_list.actlist);
			symbol = getSymbol(term->symboltable, term->value.term_act_list.id);
			if(symbol == NULL || symbol->symboltype->type 
					!= SYMBOL_FUNCTION ){
				check_error_report(
					"Function not defined or not a function", term->lineno);
			}

			if((term->value.term_act_list.actlist->kind == ACT_LIST_EMPTY && 
			   symbol->parameters->kind != PAR_DECL_LIST_EMPTY) ||
				(term->value.term_act_list.actlist->kind != ACT_LIST_EMPTY && 
			   symbol->parameters->kind == PAR_DECL_LIST_EMPTY)) {

				check_error_report("Wrong no. of parameters", term->lineno);
			}

			EXP_LIST *callParameters = term->value.term_act_list.actlist->
				exp_list;
			VAR_DECL_LIST *functionParameters = symbol->parameters->
				var_decl_list;

			while( callParameters != NULL && functionParameters 
								!= NULL && count < symbol->noArguments){
				
				if(callParameters->kind == EXP_LIST_LIST &&
				   functionParameters->kind == VAR_DECL_LIST_LIST){

					if(callParameters->exp->symboltype->type !=
					   functionParameters->var_type->symbol
					   			->symboltype->type){
						check_error_report("Wrong parameter type", 
							term->lineno);
					}

				} else if(callParameters->kind == EXP_LIST_EXP &&
					      functionParameters->kind == VAR_DECL_LIST_TYPE){
					
					if(callParameters->exp->symboltype->type !=
					   functionParameters->var_type->symbol
					   			->symboltype->type)
						check_error_report("Wrong parameter type", 
							term->lineno);
				} 
				callParameters = callParameters->explist;
				functionParameters = 
					functionParameters->var_decl_list;
				
				count++;
			}

			if( count != symbol->noArguments ){
				check_error_report("Wrong no. of parameters", term->lineno);
			}

			symbolT = symbol->symboltype->return_type;
			term->symboltype = symbolT;
			break;

		case TERM_NOT:
			check_term(term->value.term);
			if(term->value.term->symboltype->type != SYMBOL_BOOL){
				check_error_report("Expected term bool", term->lineno);
			}
			term->symboltype = term->value.term->symboltype;
			break;

		case TERM_PARENTESES:
			check_expression(term->value.exp);
			term->symboltype = term->value.exp->symboltype;
			break;

		case TERM_ABS:
			check_expression(term->value.exp);
			if(!(term->value.exp->symboltype->type == SYMBOL_INT ||
				term->value.exp->symboltype->type == SYMBOL_ARRAY) ) {
				check_error_report("Expected integer or array", 
					term->lineno);
			}
			term->symboltype = term->value.exp->symboltype;
			break;

		case TERM_NULL:
			term->symboltype = make_SYMBOLTYPE(SYMBOL_NULL);
			break;

		case TERM_TRUE:
		case TERM_FALSE:
			term->symboltype = make_SYMBOLTYPE(SYMBOL_BOOL);
			break;

		case TERM_NUM:
			term->symboltype = make_SYMBOLTYPE(SYMBOL_INT);
			break;

		default:
			break;
	}
	return count;
}

void check_par_decl_list ( PAR_DECL_LIST *pdecl ) {
	switch(pdecl->kind){
		case PAR_DECL_LIST_LIST:
			check_var_decl_list(pdecl->var_decl_list);
			break;
		case PAR_DECL_LIST_EMPTY:
			break;
	}
}

void check_act_list ( ACT_LIST *actlst ) {
	parameterCounter = 0; // reset parameter since there could be 
	switch(actlst->kind){
		case ACT_LIST_EXPLIST:
			check_expression_list(actlst->exp_list);
			break;
		case ACT_LIST_EMPTY:
			break;
	}
}

void check_expression_list ( EXP_LIST *explst ) {
	switch(explst->kind){
		case EXP_LIST_EXP:
			check_expression(explst->exp);
			break;
		case EXP_LIST_LIST:
			parameterCounter++;
			check_expression_list(explst->explist);
			check_expression(explst->exp);
			break;
	}
}

/* Note: this also sets the array dim of the function parameter and the
	return value */
SYMBOLTYPE *get_base_array_type ( SYMBOLTYPE *type_of_array ) {

	SYMBOLTYPE *iterator = type_of_array;
	int arrayDim = 1;

	while( iterator->nextArrayType != NULL ) {
		arrayDim++;
		iterator = iterator->nextArrayType;
	}
	
	type_of_array->arrayDim = arrayDim;
	iterator->arrayDim = arrayDim;

	return iterator; // reached base type of array
}

int get_array_dim ( SYMBOLTYPE *type_of_array ) {

	SYMBOLTYPE *iterator = type_of_array;
	int arrayDim = 1;

	while( iterator->nextArrayType != NULL ) {
		arrayDim++;
		iterator = iterator->nextArrayType;
	}
	return arrayDim;
}

/* Basicly iterates through the members of two records and 
 * checks if any of the pairs of members are different. 
 * Returns 0 if the records are different, 1 if they match
 */
int compare_record_members(SYMBOLTYPE *leftHand, SYMBOLTYPE *rightHand) {

	VAR_DECL_LIST *leftRecordMembers = leftHand->recordMembers;
	VAR_DECL_LIST *rightRecordMembers = rightHand->recordMembers;
	int result = 1;

	if ( leftHand->arguments != rightHand->arguments ) {
		// number of members does not match
		result = 0;
		return result;
	} else if ( leftHand->arguments == 1 ) {
		// only member in both records 
		result = leftRecordMembers->var_type->type->symboltype->type ==
					rightRecordMembers->var_type->type->symboltype->type;
		return result;
	}

	while ( leftRecordMembers->var_decl_list != NULL || 
			rightRecordMembers->var_decl_list != NULL ) {

		if ( leftRecordMembers->var_type->type->symboltype->type !=
			rightRecordMembers->var_type->type->symboltype->type ) {
			result = 0;
			return result;
		}

		leftRecordMembers = leftRecordMembers->var_decl_list;
		rightRecordMembers = rightRecordMembers->var_decl_list;
	}

	if ( leftRecordMembers->var_type->type->symboltype->type !=
		rightRecordMembers->var_type->type->symboltype->type ) {
		result = 0;
		return result;
	}

	return result;
}