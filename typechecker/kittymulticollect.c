#include "kittymulticollect.h"

extern int unknownTypesCount;

void begin_multi_collect ( BODY * main ) {

	if ( unknownTypesCount > 0 ) {
		fprintf(stderr, "Initializing multiple type collection passes phase\n");
	
		for ( int i = 0; i < MAXIMUM_PASSES; i++ ) {
			multi_collect_body ( main );
		}

		fprintf(stderr, "%i passes completed\n", MAXIMUM_PASSES + 1);
	}
}

void multi_collect_body ( BODY *body ) {
	multi_collect_decl_list(body->decl_list);
}

void multi_collect_head ( HEAD *head ) {
	multi_collect_type(head->returntype);
	multi_collect_par_decl_list(head->pdlist);
}

void multi_collect_function ( FUNC *function ) {
	multi_collect_head(function->head);
	multi_collect_body(function->body);
}

void multi_collect_type ( TYPE *type ) {

	// idea: only user defined types can be unknown since otherwise
	// the type would be known at first pass
	SYMBOL *symbol;

	switch(type->kind) {
		case TYPE_ID:
			if ( type->symbolTable->recordParentScope != NULL ) {
				// recordParentScope only set when using records
				// this case gets priority so recursive definition in records
				// gets resolved
				symbol = getSymbol(type->symbolTable->recordParentScope,
								   type->value.id);
			} else {
				symbol = getSymbol(type->symbolTable, type->value.id);
			}

			if (symbol != NULL && symbol->symbolType->type != SYMBOL_UNKNOWN
				&& type->symbolType->type == SYMBOL_UNKNOWN ) {
				type->symbolType = symbol->symbolType;
			}
			break;
		case TYPE_RECORD:
			multi_collect_var_decl_list(type->value.var_decl_list);
			break;
		case TYPE_ARRAY:
			multi_collect_type(type->value.type);
		default:
			break;
	}

}

void multi_collect_par_decl_list ( PAR_DECL_LIST *par_decl_list ) {

	switch(par_decl_list->kind) {
		case PAR_DECL_LIST_LIST:
			multi_collect_var_decl_list(par_decl_list->var_decl_list);
			break;
		case PAR_DECL_LIST_EMPTY:
			break;
	}
}

void multi_collect_var_decl_list ( VAR_DECL_LIST *var_decl_list ) {

	switch(var_decl_list->kind) {
		case VAR_DECL_LIST_LIST:
			multi_collect_var_decl_list(var_decl_list->var_decl_list);
			multi_collect_var_type(var_decl_list->var_type);
			break;
		case VAR_DECL_LIST_TYPE:
			multi_collect_var_type(var_decl_list->var_type);
			break;
	}	
}

void multi_collect_var_type ( VAR_TYPE *var_type ) {
	multi_collect_type(var_type->type);
	SYMBOL_TYPE *symbolType = var_type->type->symbolType;
	SYMBOL *symbol;

	if ( ( symbol = getSymbol(var_type->symbolTable, var_type->id) )
		!= NULL && symbol->symbolType->type == SYMBOL_UNKNOWN
		&& symbolType->type != SYMBOL_UNKNOWN ) {
		symbol->symbolType = symbolType;
		unknownTypesCount--;
	}
}

void multi_collect_decl_list ( DECL_LIST *decl_list ) {

	switch ( decl_list->kind ) {
		case DECL_LIST_LIST:
			multi_collect_decl_list(decl_list->decl_list);
			multi_collect_declaration(decl_list->declaration);
			break;
		case DECL_LIST_EMPTY:
			break;
	}
}

void multi_collect_declaration ( DECLARATION *declaration ) {
	SYMBOL *symbol;
	SYMBOL_TYPE *symbolType;

	switch(declaration->kind) {
		case DECLARATION_ID:
			multi_collect_type(declaration->value.declaration_id.type);
			symbolType = declaration->value.declaration_id.type->symbolType;

			if( ( symbol = getSymbol(declaration->symbolTable,
									declaration->value.declaration_id.id) )
				!= NULL && symbol->symbolType->type == SYMBOL_UNKNOWN
				&& symbolType->type != SYMBOL_UNKNOWN) {

				unknownTypesCount--;
				symbol->symbolType = symbolType;
			}
			break;
		case DECLARATION_FUNC:
			multi_collect_function(declaration->value.function);
			break;
		case DECLARATION_VAR:
			multi_collect_var_decl_list(declaration->value.var_decl_list);
			break;
	}
}


