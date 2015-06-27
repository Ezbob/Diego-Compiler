#include "kittycheck.h"
#include "kittycollect.h"
#include "../parserscanner/kittytree.h"
#include <stdio.h>

void check_error_report(const char* errorMsg, int lineno) {
	if (lineno < 0){
		fprintf(stderr, "Error: %s \n",errorMsg);
		exit(1);
	}
	fprintf(stderr, "Error at line %i: %s \n",lineno,errorMsg);
	exit(1);
}

void begin_check ( BODY *main ) {

	fprintf(stderr, "%s\n", "Initializing typecheck phase");
	check_body(main);
}

void check_function ( FUNC *function ) {

	check_head(function->head);
	check_body(function->body);
}

void check_head ( HEAD *header ) {

	check_par_decl_list(header->pdlist);
	check_type(header->returntype);
}

void check_body ( BODY *body ) {

	check_decl_list(body->decl_list);
	check_statement_list(body->statement_list);
}

void check_type ( TYPE *type ) {
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

void check_var_decl_list ( VAR_DECL_LIST *var_decl_list ) {
	switch(var_decl_list->kind){
		case VAR_DECL_LIST_LIST:
			check_var_decl_list(var_decl_list->var_decl_list);
			check_var_type(var_decl_list->var_type);
			break;
		case VAR_DECL_LIST_TYPE:
			check_var_type(var_decl_list->var_type);
			break;
	}
}

void check_var_type ( VAR_TYPE *var_type){
	check_type(var_type->type);

	if ( var_type->symbol->symbolType->type == SYMBOL_UNKNOWN ) {
		check_error_report("Unknown type; cannot infer type",
						   var_type->lineno);
	}
}

void check_decl_list ( DECL_LIST *decl_list){
	switch(decl_list->kind){
		case DECL_LIST_LIST:
			check_decl_list(decl_list->decl_list);
			check_declaration(decl_list->declaration);
			break;
		case DECL_LIST_EMPTY:
			break;
	}
}


void check_declaration ( DECLARATION *declaration ) {
	switch(declaration->kind){
		case DECLARATION_ID:
			check_type(declaration->value.declaration_id.type);
			break;
		case DECLARATION_FUNC:
			check_function(declaration->value.function);
			break;
		case DECLARATION_VAR:
			check_var_decl_list(declaration->value.var_decl_list);
			break;
	}
}

void check_statement_list ( STATEMENT_LIST *statement_list){
	switch(statement_list->kind){
		case STATEMENT_LIST_STATEMENT:
			check_statement(statement_list->statement);
			break;

		case STATEMENT_LIST_LIST:
			check_statement_list(statement_list->statement_list);
			check_statement(statement_list->statement);
			break;
	}
}

void check_statement ( STATEMENT *statement){

	SYMBOL_TYPE *rightHand;
	SYMBOL_TYPE *leftHand;
	SYMBOL_TYPE *returnType;

	switch(statement->kind) {
		case STATEMENT_RETURN:
			check_expression(statement->value.statement_return.exp);

			// Note: return type set in weeder
			leftHand = statement->value.statement_return.exp->symboltype;
			returnType = statement->value.statement_return.
							function->symboltype->return_type;

			if (leftHand->type != returnType->type
				&& ((returnType->type == SYMBOL_RECORD || returnType->type ==
														  SYMBOL_ARRAY) &&
					leftHand->type != SYMBOL_NULL)) {
				check_error_report(
						"type error: Return type does not match function",
						statement->lineno);
			}
			break;

		case STATEMENT_WRITE:
			check_expression(statement->value.exp);

			if (statement->value.exp->symboltype->type == SYMBOL_ARRAY &&
				statement->value.exp->value.term->kind != TERM_ABS) {
				check_error_report("Only length of arrays can be written",
								   statement->lineno);
			}
			break;

		case STATEMENT_ALLOCATE:
			check_variable(statement->value.statement_allocate.var);
			check_opt_length(statement->value.statement_allocate.opt_length);

			leftHand = statement->value.statement_allocate.var->symboltype;

			if (!(leftHand->type == SYMBOL_RECORD ||
				  leftHand->type == SYMBOL_ARRAY)) {
				check_error_report(
						"Cannot allocate to specific variable type",
						statement->lineno);
			}

			if (statement->value.statement_allocate.opt_length->kind ==
				OPT_LENGTH_EMPTY && leftHand->type != SYMBOL_RECORD) {
				check_error_report(
						"Only records can be allocated without length",
						statement->lineno);
			}
			break;

		case STATEMENT_ASSIGN:
			check_variable(statement->value.statement_assign.var);
			check_expression(statement->value.statement_assign.exp);

			leftHand = statement->value.statement_assign.var->symboltype;
			rightHand = statement->value.statement_assign.exp->symboltype;

			if (leftHand->type == SYMBOL_RECORD && rightHand->type ==
												   SYMBOL_RECORD) {

				if (compare_record_as_sets(leftHand, rightHand) != 1) {
					check_error_report("Invalid assignment, type mismatch",
									   statement->lineno);
				}

			} else if (leftHand->type == SYMBOL_ARRAY && rightHand->type ==
														 leftHand->type) {

				leftHand = get_base_array_type(leftHand);
				rightHand = get_base_array_type(rightHand);

				/* checking if the base type and dimension of the arrays
					are the same */

				if (leftHand->type != rightHand->type ||
					leftHand->arrayDim != rightHand->arrayDim) {
					check_error_report("Invalid assignment; type mismatch",
									   statement->lineno);
				}

			} else if (leftHand->type == SYMBOL_ARRAY) {

				leftHand = get_base_array_type(leftHand);

				if (leftHand->type != rightHand->type &&
					rightHand->type != SYMBOL_NULL) {
					check_error_report("Invalid assignment; type mismatch",
									   statement->lineno);
				}

			} else if (rightHand->type == SYMBOL_ARRAY) {

				rightHand = get_base_array_type(rightHand);

				if (leftHand->type != rightHand->type) {
					check_error_report("Invalid assignment; type mismatch",
									   statement->lineno);
				}

			} else if (leftHand->type == SYMBOL_RECORD) {

				if (rightHand->type != SYMBOL_NULL) {
					check_error_report(
							"Invalid assignment; type mismatch",
							statement->lineno);
				}

			} else { //Standard check
				if( rightHand->type != leftHand->type ) {
					check_error_report(
						"Invalid assignment; type mismatch",
						statement->lineno);
				} 
			}
			break;

		case STATEMENT_IFBRANCH:
			check_expression(statement->value.statement_ifbranch.exp);
			check_statement(statement->value.statement_ifbranch.statement);
			check_opt_else(statement->value.statement_ifbranch.opt_else);

			if(statement->value.statement_ifbranch.exp->
			   symboltype->type != SYMBOL_BOOL) {
				check_error_report("Expected condition to evaluate"
					" to a boolean value",
				statement->lineno);
			}
			break;

		case STATEMENT_WHILE:
			check_expression(statement->value.statement_while.exp);
			check_statement(statement->value.statement_while.statement);
			if(statement->value.statement_while.exp->symboltype->type
				!= SYMBOL_BOOL) {
				check_error_report("Expected condition to"
					" evaluate to a boolean value",
				statement->lineno);
			}
			break;

		case STATEMENT_LISTS:
			check_statement_list(statement->value.statement_list);
			break;

	}
}

void check_opt_length ( OPT_LENGTH *oplen){

	switch(oplen->kind){
		case OPT_LENGTH_EXPRES:
			check_expression(oplen->exp);
			if(oplen->exp->symboltype->type != SYMBOL_INT){
				check_error_report("Expected length to evaluate to a"
					" integer value", oplen->lineno);
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
	SYMBOL_TABLE *innerScope;
	int depth = 1;

	switch(var->kind){
		case VAR_ID:
			if( (symbol = getSymbol(var->symboltable, var->id)) != NULL ) {
				var->symboltype = symbol->symbolType;
				if ( symbol->symbolKind == TYPE_DEFINE_SYMBOL ) {
					check_error_report("Symbol is a type",var->lineno);
				}
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
					var->symboltype = symbol->symbolType;
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
	SYMBOL_TYPE *rightHand;
	SYMBOL_TYPE *leftHand;

	// if it's not a term it got two sides
	if (exp->kind != EXPRES_TERM){
		// getting base SYMBOL_TYPE of arrays for both sides if it's a array

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
				exp->symboltype = make_SYMBOL_TYPE(SYMBOL_INT);
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

				exp->symboltype = make_SYMBOL_TYPE(SYMBOL_BOOL);
				break;
			}

			// array and null
			if(	(rightHand->type == SYMBOL_ARRAY || 
			   		leftHand->type == SYMBOL_NULL) ||
			   	(rightHand->type == SYMBOL_NULL || 
			   		leftHand->type == SYMBOL_ARRAY ) ) { 

                exp->symboltype = make_SYMBOL_TYPE(SYMBOL_BOOL);
				break;
			}

			// records and null
			if(
				(rightHand->type == SYMBOL_RECORD ||
			   		leftHand->type == SYMBOL_NULL ) || 
			   	(rightHand->type == SYMBOL_NULL || 
			   		leftHand->type == SYMBOL_RECORD ) ) {

                exp->symboltype = make_SYMBOL_TYPE(SYMBOL_BOOL);
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
				exp->symboltype = make_SYMBOL_TYPE(SYMBOL_BOOL);
			}
			
			break;

		case EXPRES_AND:
		case EXPRES_OR:
			if(rightHand->type != SYMBOL_BOOL && 
			   leftHand->type != SYMBOL_BOOL) { 
				check_error_report("Expected boolean expression", 
						exp->lineno);
			} else {
				exp->symboltype = make_SYMBOL_TYPE(SYMBOL_BOOL);
			}
			break;
	}
}

void check_term ( TERM *term ) {
	SYMBOL_TYPE *symbolT;
	SYMBOL *symbol;
	int actualParameterCount;
	int numberOfParametersNeeded;

	switch(term->kind){
		case TERM_VAR:	
			check_variable(term->value.var);
			term->symboltype = term->value.var->symboltype;
			break;

		case TERM_ACT_LIST:
			
			actualParameterCount = check_act_list(term->value.
							term_act_list.actlist);
			symbol = getSymbol(term->symboltable, 
						term->value.term_act_list.id);

			if(symbol == NULL || symbol->symbolType->type
					!= SYMBOL_FUNCTION ){
				check_error_report(
					"Function not defined or not a function", term->lineno);
			}

			numberOfParametersNeeded = symbol->noParameters;

			if (actualParameterCount < numberOfParametersNeeded ) {
				check_error_report("Too few arguments for function", 
					term->lineno);
			} else if(actualParameterCount > numberOfParametersNeeded ) {
				check_error_report("Too many arguments for function", 
					term->lineno);
			} else if (actualParameterCount > 0 && 
					numberOfParametersNeeded == 0) {
				check_error_report("Function does not take any arguments", 
					term->lineno);
			}

			EXP_LIST *callParameters = term->value.term_act_list.actlist->
				exp_list;
			VAR_DECL_LIST *functionParameters = symbol->functionParameters->
				var_decl_list;

			function_parameter_compaire(callParameters,functionParameters);

			symbolT = symbol->symbolType->return_type;
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
			term->symboltype = make_SYMBOL_TYPE(SYMBOL_NULL);
			break;

		case TERM_TRUE:
		case TERM_FALSE:
			term->symboltype = make_SYMBOL_TYPE(SYMBOL_BOOL);
			break;

		case TERM_NUM:
			term->symboltype = make_SYMBOL_TYPE(SYMBOL_INT);
			break;

		default:
			break;
	}
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

int check_act_list ( ACT_LIST *actlst ) {
	int parameterCounter = 0; 

	switch(actlst->kind){
		case ACT_LIST_EXPLIST:
			parameterCounter = check_expression_list(actlst->exp_list);
			break;
		case ACT_LIST_EMPTY:
			break;
	}

	return parameterCounter;
}

int check_expression_list ( EXP_LIST *explst ) {
	int parameterCounter = 1;
	switch(explst->kind){
		case EXP_LIST_EXP:
			check_expression(explst->exp);
			break;
		case EXP_LIST_LIST:
			parameterCounter += check_expression_list(explst->explist);
			check_expression(explst->exp);
			break;
	}
	return parameterCounter;
}

/* Note: this also sets the array dim of the function parameter and the
	return value */
SYMBOL_TYPE *get_base_array_type ( SYMBOL_TYPE *type_of_array ) {

	SYMBOL_TYPE *iterator = type_of_array;
	int arrayDim = 1;

	while( iterator->nextArrayType != NULL ) {
		arrayDim++;
		iterator = iterator->nextArrayType;
	}
	
	type_of_array->arrayDim = arrayDim;
	iterator->arrayDim = arrayDim;

	return iterator; // reached base type of array
}

int get_array_dim ( SYMBOL_TYPE *type_of_array ) {

	SYMBOL_TYPE *iterator = type_of_array;
	int arrayDim = 1;

	while( iterator->nextArrayType != NULL ) {
		arrayDim++;
		iterator = iterator->nextArrayType;
	}
	return arrayDim;
}

/* Counts the type of the members of both records, and checks whether these
 * are the same. This makes the order of the members unimportant thus records
 * are treated as sets.
 * Returns 0 if the records are different, 1 if they match
 */
int compare_record_as_sets(SYMBOL_TYPE *leftHand, SYMBOL_TYPE *rightHand) {

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

	// idear: counting up the diffrent types
	int typeCount[2][NUMBER_OF_TYPES_SUPPORTED];
	SYMBOL_TYPE *currentSymbolType;

	// init the count matrix/uniqueName
	for(int i = 0; i < NUMBER_OF_TYPES_SUPPORTED; i++){
		typeCount[0][i] = 0;
		typeCount[1][i] = 0;
	}

	// counting both sides for var_decl_list
	while ( leftRecordMembers->kind == VAR_DECL_LIST_LIST ) {
		for ( int i = 0; i < 2; i++ ) {
			if ( i == 0 ) {
				currentSymbolType = leftRecordMembers->
						var_type->type->symboltype;
			} else {
				currentSymbolType = rightRecordMembers->
						var_type->type->symboltype;
			}
			switch ( currentSymbolType->type ) {
				case SYMBOL_FUNCTION:
					typeCount[i][0]++;
					break; 
				case SYMBOL_INT:
					typeCount[i][1]++;
					break; 
				case SYMBOL_BOOL:
					typeCount[i][2]++;
					break; 
				case SYMBOL_ID:
					typeCount[i][3]++;
					break;
				case SYMBOL_RECORD:
					typeCount[i][4]++;
					break; 
				case SYMBOL_ARRAY:
					typeCount[i][5]++;
					break; 
				case SYMBOL_NULL:
					typeCount[i][6]++;
					break;
				case SYMBOL_UNKNOWN:
					typeCount[i][7]++;
					break;
			}
		}

		leftRecordMembers = leftRecordMembers->var_decl_list;
		rightRecordMembers = rightRecordMembers->var_decl_list;
	}

	// we have to count one last time because var_type is the last element in
	// the list
	for ( int i = 0; i < 2; i++ ) {
		if ( i == 0 ) {
			currentSymbolType = leftRecordMembers->
					var_type->type->symboltype;
		} else {
			currentSymbolType = rightRecordMembers->
					var_type->type->symboltype;
		}
		switch ( currentSymbolType->type ) {
			case SYMBOL_FUNCTION:
				typeCount[i][0]++;
				break; 
    		case SYMBOL_INT:
    			typeCount[i][1]++;
    			break; 
    		case SYMBOL_BOOL:
    			typeCount[i][2]++;
    			break; 
    		case SYMBOL_ID:
    			typeCount[i][3]++;
    			break;
    		case SYMBOL_RECORD:
    			typeCount[i][4]++;
    			break; 
    		case SYMBOL_ARRAY:
    			typeCount[i][5]++;
    			break; 
    		case SYMBOL_NULL:
    			typeCount[i][6]++;
    			break;
    		case SYMBOL_UNKNOWN:
    			typeCount[i][7]++;
    			break;
		}
	}

	for ( int i = 0; i < NUMBER_OF_TYPES_SUPPORTED; i++ ) {
		if ( typeCount[0][i] != typeCount[1][i] ) {
			// basicly the count of type i does not match
			result = 0;
			return result;
		}
	}

	return result;
}

void function_parameter_compaire ( EXP_LIST *callParameters,
	VAR_DECL_LIST *functionParameters ) {

	while ( callParameters != NULL && functionParameters != NULL) {
		
		if( callParameters->kind == EXP_LIST_LIST &&
		   functionParameters->kind == VAR_DECL_LIST_LIST) {

			if( callParameters->exp->symboltype->type !=
			   functionParameters->var_type->symbol
			   			->symbolType->type ) {
				check_error_report("Wrong parameter type", 
					callParameters->lineno);
			}

		} else if( callParameters->kind == EXP_LIST_EXP &&
			      functionParameters->kind == VAR_DECL_LIST_TYPE ) {
			
			if( callParameters->exp->symboltype->type !=
			   functionParameters->var_type->symbol
			   			->symbolType->type){
				check_error_report("Wrong parameter type", 
					callParameters->lineno);
			}
		} 
		callParameters = callParameters->explist;
		functionParameters = functionParameters->var_decl_list;
	}
}
