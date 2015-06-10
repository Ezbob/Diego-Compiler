#include "kittycheck.h"
#include "kittytype.h"
#include <stdio.h>

int counter;
static int parameterCounter = 0;

void check_error_report(const char* errorMsg, int lineno){
	if (lineno < 0){
		fprintf(stderr, "Error: %s \n",errorMsg);
		exit(1);
	}
	fprintf(stderr, "Error at line %i: %s \n",lineno,errorMsg);
	exit(1);
}

void begin_check(BODY *main){
	fprintf(stderr, "%s\n", "Initializing typecheck phase");
	check_body(main);
}

void check_function(FUNC *func) {
	check_head(func->head);
	check_body(func->body);
}

void check_head (HEAD *header){

	check_par_decl_list(header->pdlist);
	check_type(header->returntype);
}

void check_body (BODY *body){

	check_decl_list(body->decl_list);
	check_statement_list(body->statement_list);
}


void check_type ( TYPE *type){
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

void check_var_decl_list ( VAR_DECL_LIST *vdecl){

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
			if (st->value.statement_return.exp->symboltype->type != 
				st->value.statement_return.function->symboltype->return_type->type ){
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

			/* this is a segfault
			if( (st->value.allocateS.variable->symboltype->type 
					!= SYMBOL_ARRAY) ||
			    (st->value.allocateS.variable->symboltype->type 
			   		!= SYMBOL_RECORD) ){
				check_error_report("Only " 
					"arrays and records can be allocated", st->lineno);
			}*/
			
			if(st->value.statement_allocate.opt_length->kind == OPT_LENGTH_EMPTY &&
			   st->value.statement_allocate.var->symboltype->type 
			   		!= SYMBOL_RECORD) {
				check_error_report(
					"Only records can be allocated without length", 
					st->lineno);
			} else if (st->value.statement_allocate.opt_length->kind == OPT_LENGTH_EMPTY &&
			   			st->value.statement_allocate.var->symboltype->type 
			   			== SYMBOL_RECORD){
				SYMBOL *childSymbol;
				if((childSymbol = getSymbol(st->symboltable, 
					st->value.statement_allocate.var->id)) != NULL){
					//st->value.statement_allocate.opt_length->emptyLength = 
					//	childSymbol->symboltype->child->temps; TODO
				}
			}
			break;

		case STATEMENT_ASSIGN:
			check_variable(st->value.statement_assign.var);
			check_expression(st->value.statement_assign.exp);

			SYMBOL *temp_sym;
			SYMBOLTABLE *tmpTable;
			//Checking if assignments are OK with record types
			if(st->value.statement_assign.var->kind == VAR_RECORD){

				if((temp_sym = getSymbol(st->symboltable, st->value.statement_assign.
						var->value.var_record.var->id)) != NULL){
					tmpTable = temp_sym->symboltype->child;
				}

				if((temp_sym = getSymbol(tmpTable, st->value.statement_assign.
						var->id)) != NULL){

					if(temp_sym->symboltype->type != st->value.
							statement_assign.exp->symboltype->type){
						check_error_report("Invalid assignment", st->lineno);
					} 
				}
				/*int i = 0;
				int index = 0;
				while(i < HASH_SIZE){
					if(tmpTable->table[i] != NULL){
						tmpTable->table[i]->offset = index;
						index++;
					}
					i++;
				} This spaghetti code seems fixed in collector */	
			} else { //Standard check
				temp_sym = getSymbol(st->symboltable, st->value.statement_assign.
					var->id);

				if(temp_sym != NULL){
					rightHand = get_base_array_type(temp_sym->symboltype);
					leftHand = get_base_array_type(st->
											value.statement_assign.exp->symboltype);

					if( rightHand->type != leftHand->type){
						check_error_report("Invalid assignment", st->lineno);
					} 
				}
			}

			break;

		case STATEMENT_IFBRANCH:
			check_expression(st->value.statement_ifbranch.exp);
			check_statement(st->value.statement_ifbranch.statement);
			check_opt_else(st->value.statement_ifbranch.opt_else);
			if(st->value.statement_ifbranch.exp->
			   symboltype->type != SYMBOL_BOOL) {
				check_error_report("Expected BOOL expression", st->lineno);
			}

			break;

		case STATEMENT_WHILE:
			check_expression(st->value.statement_while.exp);
			check_statement(st->value.statement_while.statement);
			if(st->value.statement_while.exp->symboltype->type != SYMBOL_BOOL){
				check_error_report("Invalid condition for Loop", st->lineno);
			}
			break;

		case STATEMENT_LISTS:
			check_statement_list(st->value.statement_list);
			break;

	}
}
//Laengden på en allocate skal vaere en int
void check_opt_length ( OPT_LENGTH *oplen){
	switch(oplen->kind){
		case OPT_LENGTH_EXPRES:
			check_expression(oplen->exp);
			if(oplen->exp->symboltype->type != SYMBOL_INT){
				fprintf(stderr, "Allocate has to be of length int");
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

void check_variable ( VAR *var){
	SYMBOL *symbol;

	switch(var->kind){
		case VAR_ID:
			if((symbol = getSymbol(var->symboltable, var->id)) != NULL){
				var->symboltype = symbol->symboltype;
			} else{
				check_error_report("Undefined symbol", var->lineno);
			}
			break;

		case VAR_ARRAY:
			check_variable(var->value.var_array.var);
			check_expression(var->value.var_array.exp);

			//Expression must be evaluated to a integer
			if(var->value.var_array.exp->symboltype->type != SYMBOL_INT){
				check_error_report("Expression must evaluate to int", 
					var->lineno);
			}

			//Variable must be array type

			if(var->value.var_array.var->symboltype->type 
					!= SYMBOL_ARRAY){
				check_error_report("Variable is not an array", var->lineno);
			}

			var->symboltype = var->value.var_array.var->
				symboltype->array->symboltype;
			break;

		/*TODO*/
		case VAR_RECORD:
			check_variable(var->value.var_record.var);
			break;
	}
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
			if(rightHand->type != SYMBOL_INT || 
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
			if(rightHand->type != SYMBOL_INT ||
			   leftHand->type != SYMBOL_INT ) {
				check_error_report("Expected integer expression", 
						exp->lineno);
			} else {
				exp->symboltype = make_SYMBOLTYPE(SYMBOL_BOOL);
			}
			
			break;

		case EXPRES_AND:
		case EXPRES_OR:
			if(rightHand->type != SYMBOL_BOOL || 
			   leftHand->type != SYMBOL_BOOL) { 
				check_error_report("Expected boolean expression", 
						exp->lineno);
			} else {
				exp->symboltype = make_SYMBOLTYPE(SYMBOL_BOOL);
			}
			break;
	}
}

int check_term ( TERM *term){
	SYMBOLTYPE *symbolT;
	SYMBOL *symbol;
	SYMBOLTABLE *tmpTable;

	int count = 0;

	switch(term->kind){
		case TERM_VAR:
	
			check_variable(term->value.var);
			if(term->value.var->kind == VAR_RECORD) {
				tmpTable = getSymbol(term->symboltable, term->value.var->
					value.var_record.var->id)->symboltype->child;
				symbol = getSymbol(tmpTable, term->value.var->value.var_record.id);
				term->symboltype = symbol->symboltype;
			} else {
				symbol = getSymbol(term->symboltable, term->value.var->id);
				if(symbol != NULL) {
					term->symboltype = symbol->symboltype;
				} else {
					term->symboltype = term->value.var->symboltype;
				}
			}

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
			if(term->value.term->symboltype->type == SYMBOL_BOOL){
				term->symboltype = term->value.term->symboltype;
			} else {
				check_error_report("Expected term bool", term->lineno);
			}
			break;

		case TERM_PARENTESES:
			check_expression(term->value.exp);
			term->symboltype = term->value.exp->symboltype;
			break;

		case TERM_ABS:
			check_expression(term->value.exp);
			if(term->value.exp->value.term->symboltype->type == SYMBOL_BOOL ||
			   term->value.exp->value.term->symboltype->type == SYMBOL_ID ||
			   term->value.exp->value.term->symboltype->type == SYMBOL_RECORD ||
			   term->value.exp->value.term->symboltype->type == SYMBOL_FUNCTION){
				check_error_report("Expected term int", term->lineno);
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

void check_par_decl_list ( PAR_DECL_LIST *pdecl){
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

SYMBOLTYPE *get_base_array_type(SYMBOLTYPE *type_of_array){

	SYMBOLTYPE *iterator = type_of_array;

	while( iterator->nextArrayType != NULL )
		iterator = iterator->nextArrayType;
	
	return iterator; // reached base type of array
}
