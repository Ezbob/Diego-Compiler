#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kittysettype.h"

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

void begin_set( BODY *body ) {

	set_body(body);
}

void set_function ( FUNC *function ) {

	set_head(function->head);
	set_body(function->body);
}

void set_head ( HEAD *header ) {

	set_par_decl_list(header->pdlist);
	set_type(header->returntype);
}
	
void set_body ( BODY *body ) {

	set_decl_list(body->decl_list);
}

void set_par_decl_list ( PAR_DECL_LIST *pdecl ) {

	switch(pdecl->kind){
		case PAR_DECL_LIST_LIST:
			set_var_decl_list(pdecl->var_decl_list);
			break;
		case PAR_DECL_LIST_EMPTY:
			break;
	}
}

void set_var_decl_list ( VAR_DECL_LIST *vdecl ) {

	switch(vdecl->kind){
		case VAR_DECL_LIST_LIST:
			set_var_decl_list(vdecl->var_decl_list);
			set_var_type(vdecl->var_type);
			break;
		case VAR_DECL_LIST_TYPE:
			set_var_type(vdecl->var_type);
			break;
	}
}

void set_var_type ( VAR_TYPE *vtype ) {
	
	set_type(vtype->type);
	//vtype->symbol->symboltype = vtype->type->symboltype; ?? this is done in 
	// kittytype
}

void set_decl_list ( DECL_LIST *dlst ) {

	switch(dlst->kind){
		case DECL_LIST_LIST:
			set_decl_list(dlst->decl_list);
			set_declaration(dlst->declaration);
			break;
		case DECL_LIST_EMPTY:
			break;
	}	
}

void set_declaration ( DECLARATION *decl ) {

	switch(decl->kind){
		case DECLARATION_ID:
			set_type(decl->value.declaration_id.type);
			break;
		case DECLARATION_FUNC:
			set_function(decl->value.function);
			break;
		case DECLARATION_VAR:
			set_var_decl_list(decl->value.var_decl_list);
			break;
	}
}

void set_type ( TYPE *type ) {

	SYMBOL *tmp;

	switch(type->kind){
		case TYPE_ID:
			tmp = getSymbol(type->symboltable, type->value.id);
			while (tmp != NULL && tmp->visited != 1) {

					if(tmp->visited == 1){
						fprintf(stderr, "Recursive defined type\n");
						exit(1);
					}

					//if(strcmp(type->value.id, test) == 0){
					//	fprintf(stderr, "%s\n", "Recursive defined type");
					//	exit(1);
					//}

					tmp = getSymbol(type->symboltable, 
						tmp->declarationtype->value.id);
				
					tmp->visited = 1;
				}
			/*switch(type->kind){ 
				// this is the same switch which will yield the same result
				case TYPE_ID:
					while (tmp != NULL && tmp->visited != 1) {

						if(tmp->visited == 1){
							fprintf(stderr, "Recursive defined type\n");
							exit(1);
						}

						//if(strcmp(type->value.id, test) == 0){
						//	fprintf(stderr, "%s\n", "Recursive defined type");
						//	exit(1);
						//}

						tmp = getSymbol(type->symboltable, 
							tmp->declarationtype->value.id);
					
						tmp->visited = 1;
					}
					break;

				case TYPE_INT:
					type->kind = TYPE_INT;
					type->symboltype->type = SYMBOL_INT;
					break;

				case TYPE_BOOL:
					type->kind = TYPE_BOOL;
					type->symboltype->type = SYMBOL_BOOL;			
					break;
				case TYPE_ARRAY:
					type->kind = TYPE_ARRAY;
					type->symboltype->type = SYMBOL_ARRAY;						
					break;

				case TYPE_RECORD:
					type->kind = TYPE_RECORD;
					type->symboltype->type = SYMBOL_RECORD;
					type->value.var_decl_list = 
						tmp->declarationtype->value.var_decl_list;
					break;

				default:
					break;
			}
			*/
			break;
		case TYPE_INT:
			break;
		case TYPE_BOOL:
			break;
		case TYPE_ARRAY:
			break;
		case TYPE_RECORD:
			break;
	}
}
