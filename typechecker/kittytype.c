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
	SYMBOLTABLE *scope = scopeSymbolTable(st);
	function->symboltable = scope;	
	/* The head does not belong to the next scope but the current
	 * so the head type must be put in current scopes symboltable
	 */
	collect_head(function->functionF.head, scope, st);
	collect_body(function->functionF.body, scope);
	function->symboltype = function->functionF.head->symboltype;
	
}

void collect_head (HEAD *header, SYMBOLTABLE *scope, SYMBOLTABLE *st){

	header->symboltable = st;
	SYMBOLTYPE *symboltype = NEW(SYMBOLTYPE);
	SYMBOL *symbol;
	symboltype->type = SYMBOL_FUNCTION;
	
	header->symboltype = symboltype;

	int noArguments = 0;
	noArguments = collect_par_decl_list(header->headH.pdeclist, scope);
	header->arguments = noArguments;
	scope->temps = noArguments;
	symbol = putSymbol(st, header->headH.id, 0, symboltype);
	symbol->parameters = header->headH.pdeclist;
	symbol->returntype = header->headH.returntype;
	symbol->noArguments = noArguments;
	collect_type(header->headH.returntype, st);

	symboltype->value.return_type = header->headH.returntype;

}

void collect_body (BODY *body, SYMBOLTABLE *st){

	body->symboltable = st;
	collect_decl_list(body->decl_list, st);
	collect_statement_list(body->statement_list, st);

}

SYMBOLTYPE *collect_type ( TYPE *type, SYMBOLTABLE *st){

	type->symboltable = st;
	SYMBOLTYPE *symboltype = NEW(SYMBOLTYPE);

	switch(type->kind){
		case id_TY_K:
			symboltype->type = SYMBOL_ID;
			type->symboltype = symboltype;
			return symboltype;

		case int_TY_K:
			symboltype->type = SYMBOL_INT;
			type->symboltype = symboltype;
			return symboltype;

		case bool_TY_K:
			symboltype->type = SYMBOL_BOOL;
			type->symboltype = symboltype;
			return symboltype;

		case arrayof_TY_K:
			symboltype->type = SYMBOL_ARRAY;
			symboltype->nextInArray = collect_type(type->value.type, st);
			type->symboltype = symboltype;
			type->symboltype->value.array = type->value.type;
			return symboltype;

		case recordof_TY_K:
			symboltype->type = SYMBOL_RECORD;
			type->symboltype = symboltype;
			collect_var_decl_list(type->value.var_decl_list, 
				scopeSymbolTable(st));
			symboltype->child = type->value.var_decl_list->symboltable;
			return symboltype;
	}

	return NULL; // something went wrong
}

int collect_par_decl_list ( PAR_DECL_LIST *pdecl, SYMBOLTABLE *st){

	pdecl->symboltable = st;
	int arguments = 0;
	switch(pdecl->kind){
		case varType_PDL_K:
			arguments += collect_var_decl_list(pdecl->value.var_decl_list, st);
			break;

		case empty_PDL_K:
			return 0;
			break;

	}
	return arguments;
}

int collect_var_decl_list ( VAR_DECL_LIST *vdecl, SYMBOLTABLE *st){
	vdecl->symboltable = st;
	int no = 1;
	switch(vdecl->kind){
		case comma_VDL_K:
			no += collect_var_decl_list(vdecl->value.commaVDL.var_decl_list, st);
			collect_var_type(vdecl->value.commaVDL.var_type, st);
			break;

		case var_VDL_typeK:
			collect_var_type(vdecl->value.var_type, st);
			break;
	}

	return no;
}

void collect_var_type ( VAR_TYPE *vtype, SYMBOLTABLE *st){
	vtype->symboltable = st;

	vtype->symbol = putSymbol(st, vtype->id, 0, vtype->type->symboltype);
	if(vtype->symbol == NULL){
		fprintf(stderr, "%s\n", "Duplicate entry in symboltable");
		exit(1);
	}

	collect_type(vtype->type, st);
	//dumpSymbolTable(vtype->type->value.var_decl_list->symboltable);
	//printf("vtype id %s\n", vtype->id);

	//printf("%s\n", vtype->type->value.idconst);
	/*SYMBOL *check = getSymbol(vtype->symboltable, vtype->type->value.idconst);
	// setting the type def SYMBOLTYPE
	if(check != NULL ){
		vtype->symbol->symboltype = check->symboltype;
	} else {
		vtype->symbol->realtype = vtype->type;
	}*/
	st->temps++;

}

void collect_decl_list ( DECL_LIST *dlst, SYMBOLTABLE *st ){

	dlst->symboltable = st;

	switch(dlst->kind){
		case compound_DL_K:
			collect_decl_list(dlst->value.compoundDL.decl_list, st);
			collect_declaration(dlst->value.compoundDL.declaration, st);
			break;

		case empty_DL_K:
			break;
	}
}


void collect_declaration ( DECLARATION *decl, SYMBOLTABLE *st ){

	decl->symboltable = st;
	SYMBOLTYPE *symboltype = NEW(SYMBOLTYPE);
	SYMBOL *test;

	switch(decl->kind){
		case typeassign_D_K:
			symboltype->type = SYMBOL_ID;
			collect_type(decl->value.typedeclID.type, st);

			symboltype = decl->value.typedeclID.type->symboltype;
			symboltype->value.declaration_type = decl->value.typedeclID.type;

			if((test = putSymbol(st, decl->value.typedeclID.id, 0, symboltype)) 
					== NULL ){
				fprintf(stderr, 
					"Error: cannot place new type in symbol table\n");
			}

			test->declarationtype = decl->value.typedeclID.type;

			break;

		case func_D_K:
			collect_function(decl->value.function, st);
			break;

		case var_D_K:
			collect_var_decl_list(decl->value.var_decl_list, st);
			break;
	}
}

void collect_statement_list ( STATEMENT_LIST *slst, SYMBOLTABLE *st ){

	slst->symboltable = st;

	switch(slst->kind){
		case statement_SL_K:
			collect_statement(slst->value.statement, st);
			break;

		case compound_SL_K:
			collect_statement_list(slst->value.compoundSL.statement_list, st);
			collect_statement(slst->value.compoundSL.statement, st);
			break;
	}
}

void collect_statement ( STATEMENT *stm, SYMBOLTABLE *st ){

	stm->symboltable = st;

	switch(stm->kind){
		case return_S_K:
			collect_expression(stm->value.returnS.exp, st);
			break;

		case print_S_K:
			collect_expression(stm->value.exp, st);
			break;

		case allocate_S_K:
			collect_variable(stm->value.allocateS.variable, st);
			collect_opt_length(stm->value.allocateS.opt_length, st);
			break;

		case assign_S_K:
			collect_variable(stm->value.assignS.variable, st);
			collect_expression(stm->value.assignS.exp, st);
			break;

		case ifbranch_S_K:
			collect_expression(stm->value.ifbranchS.exp, st);
			collect_statement(stm->value.ifbranchS.statement, st);
			collect_opt_else(stm->value.ifbranchS.opt_else, st);
			break;

		case while_S_K:
			collect_expression(stm->value.whileS.exp, st);
			collect_statement(stm->value.whileS.statement, st);
			break;

		case statement_list_S_K:
			collect_statement_list(stm->value.statement_list, st);
			break;
	}
}

void collect_opt_length ( OPT_LENGTH *oplen, SYMBOLTABLE *st ){

	oplen->symboltable = st;

	switch(oplen->kind){
		case lengthof_OL_K:
			collect_expression(oplen->value.exp, st);
			break;

		case empty_OL_K:
			break;
	}
}

void collect_opt_else ( OPT_ELSE *opel, SYMBOLTABLE *st ){

	opel->symboltable = st;

	switch(opel->kind){
		case else_OE_K:
			collect_statement(opel->value.statement, st);
			break;

		case empty_OE_K:
			break;
	}
}

void collect_variable ( VAR *var, SYMBOLTABLE *st ){

	var->symboltable = st;

	switch(var->kind){
		case id_V_K:
			break;

		case indexing_V_K:
			collect_variable(var->value.indexingV.variable, st);
			collect_expression(var->value.indexingV.exp, st);
			break;

		case dot_V_K:
			collect_variable(var->value.dotV.variable, st);
			break;
	}
}

void collect_expression ( EXPRES *exp, SYMBOLTABLE *st ){

	exp->symboltable = st;

	switch(exp->kind){
		case term_E_K:
			collect_term(exp->value.term, st);
			exp->symboltype = exp->value.term->symboltype;
			break;
		case plus_E_K:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case minus_E_K:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case times_E_K:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case divide_E_K:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case booleq_E_K:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case boolneq_E_K:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case boolgreater_E_K:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case boolless_E_K:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case boolleq_E_K:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case boolgeq_E_K:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case booland_E_K:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;

		case boolor_E_K:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;
	}

}

void collect_term ( TERM *term, SYMBOLTABLE *st ){

	term->symboltable = st;
	SYMBOLTYPE *symboltype = NEW(SYMBOLTYPE);

	switch(term->kind){
		case var_T_K:
			collect_variable(term->value.var, st);
			break;

		case actList_T_K:
			collect_act_list(term->value.actlistT.actlist, st);
			break;

		case termBang_T_K:
			collect_term(term->value.term, st);
			break;

		case expresParent_T_K:
			collect_expression(term->value.exp, st);
			break;

		case expresPipes_T_K:
			collect_expression(term->value.exp, st);
			break;

		case null_T_K:
			symboltype->type = SYMBOL_NULL;
			term->symboltype = symboltype;
			break;

		case boolTrue_T_K:
		case boolFalse_T_K:
			symboltype->type = SYMBOL_BOOL;
			term->symboltype = symboltype;
			break;

		case num_T_K:
			symboltype->type = SYMBOL_INT;
			term->symboltype = symboltype;
			break;
	}
}

void collect_act_list ( ACT_LIST *actlst, SYMBOLTABLE *st){

	actlst->symboltable = st;

	switch(actlst->kind){
		case explist_AL_K:
			collect_expression_list(actlst->value.exp_list, st);
			break;
		case empty_AL_K:
			break;
	}
}

void collect_expression_list ( EXP_LIST *explst, SYMBOLTABLE *st){

	explst->symboltable = st;

	switch(explst->kind){
		case exp_EL_K:
			collect_expression(explst->value.exp, st);
			break;
		case commalist_EL_K:
			collect_expression_list(explst->value.commaEL.explist, st);
			collect_expression(explst->value.commaEL.exp, st);
			break;
	}
}
