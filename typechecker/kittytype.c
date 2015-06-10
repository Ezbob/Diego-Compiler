#include "kittytype.h"
#include "../parserscanner/kittytree.h"
#include <stdio.h>

/*
 * Collecting types
 */
extern SYMBOLTABLE *loltable;

void collect(BODY *main, SYMBOLTABLE *symboltable){
	fprintf(stderr, "Initializing Type collection\n");
	//Start collecting
	collect_body(main, symboltable);



}

void collect_function ( FUNC *function, SYMBOLTABLE *st) {

	/*Variables only lives in function, so new scope*/
	SYMBOLTABLE *scope = scopeSymbolTable(st, st->id);
	function->symboltable = scope;	
	/* The head does not belong to the next scope but the current
	 * so the head type must be put in current scopes symboltable
	 */
	collect_head(function->head, scope, st);
	collect_body(function->body, scope);
	function->symboltype = function->head->symboltype;
	
}

void collect_head (HEAD *header, SYMBOLTABLE *scope, SYMBOLTABLE *st){

	header->symboltable = st;
	SYMBOLTYPE *symboltype = make_SYMBOLTYPE(SYMBOL_FUNCTION);
	SYMBOL *symbol;
	
	header->symboltype = symboltype;

	int noArguments = 0;
	noArguments = collect_par_decl_list(header->pdlist, scope);
	header->arguments = noArguments;
	scope->temps = noArguments;
	symbol = putSymbol(st, header->id, 0, symboltype);
	symbol->parameters = header->pdlist;
	symbol->returntype = header->returntype;
	symbol->noArguments = noArguments;

	// return_type is now a SYMBOLTYPE struct
	symboltype->return_type = collect_type(header->returntype, st);
}

void collect_body (BODY *body, SYMBOLTABLE *st){

	body->symboltable = st;
	collect_decl_list(body->decl_list, st);
	collect_statement_list(body->statement_list, st);
}

SYMBOLTYPE *collect_type ( TYPE *type, SYMBOLTABLE *st){

	type->symboltable = st;
	SYMBOLTYPE *symboltype = NULL;

	switch(type->kind){
		case TYPE_ID:
			symboltype = make_SYMBOLTYPE(SYMBOL_ID);
			type->symboltype = symboltype;
			return symboltype;

		case TYPE_INT:
			symboltype = make_SYMBOLTYPE(SYMBOL_INT);
			type->symboltype = symboltype;
			return symboltype;

		case TYPE_BOOL:
			symboltype = make_SYMBOLTYPE(SYMBOL_BOOL);
			type->symboltype = symboltype;
			return symboltype;

		case TYPE_ARRAY:
			symboltype = make_SYMBOLTYPE(SYMBOL_ARRAY);
			type->symboltype = symboltype;
			symboltype->nextArrayType = collect_type(type->value.type, st);
			type->symboltype->array = type->value.type;
			return symboltype; 


		case TYPE_RECORD:
			symboltype = make_SYMBOLTYPE(SYMBOL_RECORD);
			type->symboltype = symboltype;
			collect_var_decl_list(type->value.var_decl_list, 
											scopeSymbolTable(st, st->id));

			symboltype->child = type->value.var_decl_list->symboltable;
			return symboltype;
	}

	return symboltype; // null if a error case

}

int collect_par_decl_list ( PAR_DECL_LIST *pdecl, SYMBOLTABLE *st){

	pdecl->symboltable = st;
	int arguments = 0;
	switch(pdecl->kind){
		case PAR_DECL_LIST_LIST:
			arguments += collect_var_decl_list(pdecl->var_decl_list, st);
			break;

		case PAR_DECL_LIST_EMPTY:
			return 0;
			break;

	}
	return arguments;
}

//Collecting lists and passing on offsets to variables
int collect_var_decl_list ( VAR_DECL_LIST *vdecl, SYMBOLTABLE *st){
	vdecl->symboltable = st;
	int no = 1;
	switch(vdecl->kind){
		case VAR_DECL_LIST_LIST:
			no += collect_var_decl_list(vdecl->var_decl_list, st);
			collect_var_type(vdecl->var_type, st, no);
			break;

		case VAR_DECL_LIST_TYPE:
			collect_var_type(vdecl->var_type, st, no);
			break;
	}

	return no;
}

void collect_var_type ( VAR_TYPE *vtype, SYMBOLTABLE *st, int offset){
	vtype->symboltable = st;

	SYMBOLTYPE *symboltype;

	symboltype = collect_type(vtype->type, st);
	
	if(symboltype != NULL){
		vtype->symbol = putSymbol(st, vtype->id, 0, symboltype);
		if(vtype->symbol == NULL){
			fprintf(stderr, "%s\n", "Duplicate entry in symboltable");
			exit(1);
		}
		vtype->symbol->offset = offset;
	} else {
		fprintf(stderr, "Error at line %i: type of symbol not recognized \n"
			, vtype->lineno);
		exit(1);
	}
	
	/*SYMBOL *check;
	if(vtype->type->kind == TYPE_ID && 
		(check = getSymbol(vtype->symboltable, vtype->type->value.id)) 
			!= NULL){
			printf("%s\n", "GOT HERE");
		vtype->symbol->symboltype = check->symboltype;
	} else {
		vtype->symbol->realtype = vtype->type;
	}*/
	st->temps++;
}

void collect_decl_list ( DECL_LIST *dlst, SYMBOLTABLE *st ){

	dlst->symboltable = st;

	switch(dlst->kind){
		case DECL_LIST_LIST:
			collect_decl_list(dlst->decl_list, st);
			collect_declaration(dlst->declaration, st);
			break;

		case DECL_LIST_EMPTY:
			break;
	}
}


void collect_declaration ( DECLARATION *decl, SYMBOLTABLE *st ){

	decl->symboltable = st;
	SYMBOLTYPE *symboltype;
	SYMBOL *test;

	switch(decl->kind){
		case DECLARATION_ID:
			symboltype = make_SYMBOLTYPE(SYMBOL_ID);

			collect_type(decl->value.declaration_id.type, st);

			//Since symboltypes seems out might be subject to scrapping
			//symboltype = decl->value.declaration_id.type->symboltype;
			//symboltype->declaration_type = decl->value.declaration_id.type;


			if((test = putSymbol(st, decl->value.declaration_id.id, 0, 
					symboltype)) == NULL ){
				fprintf(stderr, "Error: in collecting new type assign\n");
			}
			
			test->declarationtype = decl->value.declaration_id.type; //???

			break;

		case DECLARATION_FUNC:
			collect_function(decl->value.function, st);
			break;

		case DECLARATION_VAR:
			collect_var_decl_list(decl->value.var_decl_list, st);
			break;
	}
}

void collect_statement_list ( STATEMENT_LIST *slst, SYMBOLTABLE *st ){

	slst->symboltable = st;

	switch(slst->kind){
		case STATEMENT_LIST_STATEMENT:
			collect_statement(slst->statement, st);
			break;

		case STATEMENT_LIST_LIST:
			collect_statement_list(slst->statement_list, st);
			collect_statement(slst->statement, st);
			break;
	}
}

void collect_statement ( STATEMENT *stm, SYMBOLTABLE *st ){

	stm->symboltable = st;

	switch(stm->kind){
		case STATEMENT_RETURN:
			collect_expression(stm->value.statement_return.exp, st);
			break;

		case STATEMENT_WRITE:
			collect_expression(stm->value.exp, st);
			break;

		case STATEMENT_ALLOCATE:
			collect_variable(stm->value.statement_allocate.var, st);
			collect_opt_length(stm->value.statement_allocate.opt_length, st);
			break;

		case STATEMENT_ASSIGN:
			collect_variable(stm->value.statement_assign.var, st);
			collect_expression(stm->value.statement_assign.exp, st);
			break;

		case STATEMENT_IFBRANCH:
			collect_expression(stm->value.statement_ifbranch.exp, st);
			collect_statement(stm->value.statement_ifbranch.statement, st);
			collect_opt_else(stm->value.statement_ifbranch.opt_else, st);
			break;

		case STATEMENT_WHILE:
			collect_expression(stm->value.statement_while.exp, st);
			collect_statement(stm->value.statement_while.statement, st);
			break;

		case STATEMENT_LISTS:
			collect_statement_list(stm->value.statement_list, st);
			break;
	}
}

void collect_opt_length ( OPT_LENGTH *oplen, SYMBOLTABLE *st ){

	oplen->symboltable = st;

	switch(oplen->kind){
		case OPT_LENGTH_EXPRES:
			collect_expression(oplen->exp, st);
			break;

		case OPT_LENGTH_EMPTY:
			break;
	}
}

void collect_opt_else ( OPT_ELSE *opel, SYMBOLTABLE *st ){

	opel->symboltable = st;

	switch(opel->kind){
		case OPT_ELSE_STATEMENT:
			collect_statement(opel->statement, st);
			break;

		case OPT_ELSE_EMPTY:
			break;
	}
}

void collect_variable ( VAR *var, SYMBOLTABLE *st ){

	var->symboltable = st;
	switch(var->kind){
		case VAR_ID:
			break;

		case VAR_ARRAY:
			collect_variable(var->value.var_array.var, st);
			collect_expression(var->value.var_array.exp, st);
			break;

		case VAR_RECORD:
			collect_variable(var->value.var_record.var, st);
			break;
	}
}

void collect_expression ( EXPRES *exp, SYMBOLTABLE *st ){

	exp->symboltable = st;

	switch(exp->kind){
		case EXPRES_TERM:
			collect_term(exp->value.term, st);
			exp->symboltype = exp->value.term->symboltype;
			break;
		case EXPRES_PLUS:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case EXPRES_MINUS:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case EXPRES_TIMES:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case EXPRES_DIVIDE:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case EXPRES_EQ:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case EXPRES_NEQ:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case EXPRES_GREATER:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case EXPRES_LESS:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case EXPRES_LEQ:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case EXPRES_GEQ:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case EXPRES_AND:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case EXPRES_OR:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;
	}

}

void collect_term ( TERM *term, SYMBOLTABLE *st ){

	term->symboltable = st;
	SYMBOLTYPE *symboltype;

	switch(term->kind){
		case TERM_VAR:
			collect_variable(term->value.var, st);
			break;

		case TERM_ACT_LIST:
			collect_act_list(term->value.term_act_list.actlist, st);
			break;

		case TERM_NOT:
			collect_term(term->value.term, st);
			break;

		case TERM_PARENTESES:
			collect_expression(term->value.exp, st);
			break;

		case TERM_ABS:
			collect_expression(term->value.exp, st);
			break;

		case TERM_NULL:
			symboltype = make_SYMBOLTYPE(SYMBOL_NULL);
			term->symboltype = symboltype;
			break;

		case TERM_TRUE:
		case TERM_FALSE:
			symboltype = make_SYMBOLTYPE(SYMBOL_BOOL);
			term->symboltype = symboltype;
			break;

		case TERM_NUM:
			symboltype = make_SYMBOLTYPE(SYMBOL_INT);
			term->symboltype = symboltype;
			break;
	}
}

void collect_act_list ( ACT_LIST *actlst, SYMBOLTABLE *st){

	actlst->symboltable = st;

	switch(actlst->kind){
		case ACT_LIST_EXPLIST:
			collect_expression_list(actlst->exp_list, st);
			break;
		case ACT_LIST_EMPTY:
			break;
	}
}

void collect_expression_list ( EXP_LIST *explst, SYMBOLTABLE *st){

	explst->symboltable = st;

	switch(explst->kind){
		case EXP_LIST_EXP:
			collect_expression(explst->exp, st);
			break;
		case EXP_LIST_LIST:
			collect_expression_list(explst->explist, st);
			collect_expression(explst->exp, st);
			break;
	}
}

// initializing all members of the SYMBOLTYPE struct
SYMBOLTYPE *make_SYMBOLTYPE(TYPES_SUPPORTED type){
	SYMBOLTYPE *new_type;
	new_type = NEW(SYMBOLTYPE);

	new_type->nextArrayType = NULL;
	new_type->return_type = NULL;
	new_type->child = NULL;
	new_type->array = NULL;
  	new_type->declaration_type = NULL;
  	new_type->func = NULL;
  	new_type->parameters = NULL;
  	new_type->arguments = 0;
  	new_type->type = type;

	return new_type;
}
