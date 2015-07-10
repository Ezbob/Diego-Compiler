#include "kittycollect.h"
#include "../parserscanner/kittytree.h"

#define FIRST_TABLE_ID 0
#define RESET_SCOPE_OFFSET (currentScopeOffset = 1)
#define NEXT_SCOPE_OFFSET (currentScopeOffset++)
#define GET_NEXT_FUNCTION_ID (functionOffset++)

int unknownTypesCount = 0; // decides whether more passes should be used.
						// more passes are used when unknownTypesCount > 0
static int functionOffset = 0;
static int currentScopeOffset = 1;

/*
 * Collecting symbols and setting symboltables in ast nodes
 */

void begin_collect(BODY *main) {
	fprintf(stderr, "Initializing type collection phase\n");
	collect_body(main, initSymbolTable(FIRST_TABLE_ID));
}

void collect_function ( FUNC *function, SYMBOL_TABLE *st ) {

	/*Variables only lives in function, so new scope*/
	SYMBOL_TABLE *scope = scopeSymbolTable(st, st->id);
	function->symboltable = scope;	
	/* The head does not belong to the next scope but the current
	 * so the head type must be put in current scopes symboltable
	 */

	collect_head(function->head, scope, st);
	collect_body(function->body, scope);
	function->symboltype = function->head->symboltype;
}

void collect_head (HEAD *header, SYMBOL_TABLE *inner, SYMBOL_TABLE *outer) {
	SYMBOL *symbol;
	
	header->symboltable = outer;
	header->symboltype = make_SYMBOL_TYPE( SYMBOL_FUNCTION );

	RESET_SCOPE_OFFSET;
	int noArguments = collect_par_decl_list(header->pdlist, inner);
	RESET_SCOPE_OFFSET;

	header->arguments = noArguments;
	inner->temps = noArguments;

	/* symbol for the function */
	symbol = putSymbol(outer, header->id, header->symboltype);
	symbol->functionParameters = header->pdlist;
	symbol->noParameters = noArguments;
	symbol->offset = GET_NEXT_FUNCTION_ID;

	header->symboltype->return_type = collect_type(header->returntype, outer);
	if ( header->symboltype->return_type == NULL ) {
		fprintf(stderr,"Error: Could not collect function return type\n");
		exit(1);
	}
}

void collect_body (BODY *body, SYMBOL_TABLE *st) {
	body->symboltable = st;
	collect_decl_list(body->decl_list, st);
	collect_statement_list(body->statement_list, st);
}

/* Note: here parentTypeName is for recursively defined types */
SYMBOL_TYPE *collect_type ( TYPE *type, SYMBOL_TABLE *st ) {

	type->symboltable = st;
	SYMBOL_TYPE *symboltype = NULL;
	SYMBOL* symbol = NULL;

	switch(type->kind){
		case TYPE_ID:
			if( (symbol = getSymbol(st, type->value.id)) != NULL &&
				symbol->symbolType->type != SYMBOL_UNKNOWN ) {
				symboltype = symbol->symbolType;
				type->symboltype = symboltype;
				return symboltype;
			}
			type->symboltype = make_SYMBOL_TYPE(SYMBOL_UNKNOWN);
			return type->symboltype; // set to unknown if we can't find it

		case TYPE_INT:
			symboltype = make_SYMBOL_TYPE(SYMBOL_INT);
			type->symboltype = symboltype;
			return symboltype;

		case TYPE_BOOL:
			symboltype = make_SYMBOL_TYPE(SYMBOL_BOOL);
			type->symboltype = symboltype;
			return symboltype;

		case TYPE_ARRAY:
			symboltype = make_SYMBOL_TYPE(SYMBOL_ARRAY);
			type->symboltype = symboltype;
			symboltype->nextArrayType = collect_type(type->value.type, st);
			return symboltype;

		case TYPE_RECORD:
			symboltype = make_SYMBOL_TYPE(SYMBOL_RECORD);
			type->symboltype = symboltype;
			symboltype->child = scopeSymbolTable(st, st->id);
			RESET_SCOPE_OFFSET;
			symboltype->arguments =	collect_var_decl_list(
				type->value.var_decl_list, symboltype->child,
				RECORD_MEMBER_SYMBOL);
			RESET_SCOPE_OFFSET;
			symboltype->recordMembers = type->value.var_decl_list;
			return symboltype;
	}

	return symboltype; // null if a error case
}

int collect_par_decl_list ( PAR_DECL_LIST *pdecl, SYMBOL_TABLE *st ) {

	pdecl->symboltable = st;
	int arguments = 0;
	switch(pdecl->kind){
		case PAR_DECL_LIST_LIST:
			arguments += collect_var_decl_list(pdecl->var_decl_list, st,
			PARAMETER_SYMBOL );
			break;

		case PAR_DECL_LIST_EMPTY:
			return 0;
	}
	return arguments;
}

//Collecting lists and passing on offsets to variables
int collect_var_decl_list ( VAR_DECL_LIST *vdecl, SYMBOL_TABLE *st,
							SYMBOL_KIND symbolKind ) {

	vdecl->symboltable = st;
	int numberOfVariables = 1;
	switch(vdecl->kind){
		case VAR_DECL_LIST_LIST:
			numberOfVariables += collect_var_decl_list(vdecl->var_decl_list,
													   st, symbolKind);
			collect_var_type(vdecl->var_type, st, symbolKind);
			break;

		case VAR_DECL_LIST_TYPE:
			collect_var_type(vdecl->var_type, st, symbolKind);
			break;
	}
	return numberOfVariables;
}

void collect_var_type ( VAR_TYPE *vtype, SYMBOL_TABLE *st,
						SYMBOL_KIND symbolKind ) {

	vtype->symboltable = st;
	SYMBOL_TYPE *symbolType = collect_type(vtype->type, st);

	if ( symbolType->type == SYMBOL_UNKNOWN ) {
		unknownTypesCount++;
	}

	if (symbolType != NULL) {
		vtype->symbol = putSymbol(st, vtype->id, symbolType);
		if(vtype->symbol == NULL) {
			fprintf(stderr, "Duplicate entry in symboltable\n");
			exit(1);
		}
		vtype->symbol->symbolKind = symbolKind;
		if( !( symbolType->type == SYMBOL_RECORD ||
		   symbolType->type == SYMBOL_ARRAY ) ||
				symbolKind == RECORD_MEMBER_SYMBOL ||
				symbolKind == PARAMETER_SYMBOL ) {
			vtype->symbol->offset = currentScopeOffset;
			NEXT_SCOPE_OFFSET;
		} else {
			vtype->symbol->offset = 0;
		}
	} else {
		fprintf(stderr, "Error at line %i: type of symbol not recognized \n"
			, vtype->lineno);
		exit(1);
	}
	
	st->temps++;
}

void collect_decl_list ( DECL_LIST *dlst, SYMBOL_TABLE *st ) {

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

void collect_declaration ( DECLARATION *decl, SYMBOL_TABLE *st ) {

	decl->symboltable = st;
	SYMBOL_TYPE *symboltype;
	SYMBOL *symbol;

	switch(decl->kind){
		case DECLARATION_ID:

			symboltype = collect_type(decl->value.declaration_id.type, st);

			if(symboltype->type == SYMBOL_UNKNOWN){
				unknownTypesCount++;
			}

			if ( ( symbol = putSymbol(st, decl->value.declaration_id.id,
									  symboltype) ) != NULL) {
				symbol->symbolKind = TYPE_DEFINE_SYMBOL;
			} else {
				fprintf(stderr, "Error at line %i: "
						"Could not assignment new type\n", decl->lineno);
				exit(1);
			}

			break;

		case DECLARATION_FUNC:
			RESET_SCOPE_OFFSET;
			collect_function(decl->value.function, st);
			RESET_SCOPE_OFFSET;
			break;

		case DECLARATION_VAR:
			collect_var_decl_list( decl->value.var_decl_list, st,
			LOCAL_VARIABLE_SYMBOL);
			break;
	}
}

void collect_statement_list ( STATEMENT_LIST *slst, SYMBOL_TABLE *st ) {

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

void collect_statement ( STATEMENT *stm, SYMBOL_TABLE *st ) {

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

		case STATEMENT_BREAK:
			break;

		case STATEMENT_CONTINUE:
			break;
	}
}

void collect_opt_length ( OPT_LENGTH *oplen, SYMBOL_TABLE *st ) {

	oplen->symboltable = st;

	switch(oplen->kind){
		case OPT_LENGTH_EXPRES:
			collect_expression(oplen->exp, st);
			break;

		case OPT_LENGTH_EMPTY:
			break;
	}
}

void collect_opt_else ( OPT_ELSE *opel, SYMBOL_TABLE *st ) {

	opel->symboltable = st;

	switch(opel->kind){
		case OPT_ELSE_STATEMENT:
			collect_statement(opel->statement, st);
			break;

		case OPT_ELSE_EMPTY:
			break;
	}
}

void collect_variable ( VAR *var, SYMBOL_TABLE *st ) {

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

void collect_expression ( EXPRES *exp, SYMBOL_TABLE *st ) {

	exp->symboltable = st;

	switch(exp->kind){
		case EXPRES_TERM:
			collect_term(exp->value.term, st);
			break;

		default:
			collect_expression(exp->value.sides.left, st);
			collect_expression(exp->value.sides.right, st);
			break;
	}
}

void collect_term ( TERM *term, SYMBOL_TABLE *st ) {

	term->symboltable = st;

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

		case TERM_UMINUS:
			collect_term(term->value.term, st);
			break;

		case TERM_PARENTESES:
			collect_expression(term->value.exp, st);
			break;

		case TERM_ABS:
			collect_expression(term->value.exp, st);
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
	}
}

void collect_act_list ( ACT_LIST *actlst, SYMBOL_TABLE *st ) {

	actlst->symboltable = st;

	switch(actlst->kind){
		case ACT_LIST_EXPLIST:
			collect_expression_list(actlst->exp_list, st);
			break;
		case ACT_LIST_EMPTY:
			break;
	}
}

void collect_expression_list ( EXP_LIST *explst, SYMBOL_TABLE *st ) {

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

// initializing all members of the SYMBOL_TYPE struct
SYMBOL_TYPE *make_SYMBOL_TYPE(TYPES_SUPPORTED type) {
	SYMBOL_TYPE *new_type;
	new_type = NEW(SYMBOL_TYPE);
	new_type->nextArrayType = NULL;
	new_type->recordMembers = NULL;
	new_type->return_type = NULL;
	new_type->child = NULL;
  	new_type->arguments = 0;
  	new_type->arrayDim = 0;
  	new_type->type = type;

	return new_type;
}
