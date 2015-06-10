#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kittysettype.h"

char test[HASH_SIZE];

void begin_set( BODY *body ){

	set_body(body);
}

void set_function ( FUNC *function ){

	set_head(function->head);
	set_body(function->body);

}

void set_head ( HEAD *header ){

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
	strcpy(test, vtype->id);
	set_type(vtype->type);
	vtype->symbol->symboltype = vtype->type->symboltype;

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
//Types only have symbol types???
void set_type ( TYPE *type ) {

	SYMBOL *tmp;

	printf("%p\n", (void *) type->symboltype->declaration_type);
	dumpSymbolTable(type->symboltable);
	switch(type->kind){
		case TYPE_ID:
			tmp = getSymbol(type->symboltable, type->value.id);
			printf("%s\n", tmp->name);


			switch(type->kind){
				case TYPE_ID:

				while (tmp != NULL && tmp->visited != 1){

						if(tmp->visited == 1){
							fprintf(stderr, "%s\n", "Recursive defined type");
							exit(1);
						}

						if(strcmp(type->value.id, test) == 0){
							fprintf(stderr, "%s\n", "Recursive defined type");
							exit(1);
						}

						printf("%s\n", "ID");
						tmp = getSymbol(type->symboltable, tmp->declarationtype->value.id);
						printf("%s\n", "TEST 4");
						tmp->visited = 1;
				}
				break;

				case TYPE_INT:
					printf("%s\n", "INT");
					type->kind = TYPE_INT;
					type->symboltype->type = SYMBOL_INT;
					break;

				case TYPE_BOOL:
					printf("%s\n", "BOOL");
					type->kind = TYPE_BOOL;
					type->symboltype->type = SYMBOL_BOOL;			
					break;
				case TYPE_ARRAY:
					printf("%s\n", "ARRAY");
					type->kind = TYPE_ARRAY;
					type->symboltype->type = SYMBOL_ARRAY;						
					break;

				case TYPE_RECORD:
					printf("%s\n", "RECORD");
					type->kind = TYPE_RECORD;
					type->symboltype->type = SYMBOL_RECORD;
					type->value.var_decl_list = tmp->declarationtype->value.var_decl_list;
					break;

				default:
					printf("%s\n", "DEFAULT");
					break;
			}


				//printf("%p\n", (void *) tmp->declarationtype->value.id);
				//printf("%s\n", tmp->declarationtype->value.id);
				//tmp = getSymbol(type->symboltable, tmp->declarationtype->value.id);



			break;
		case TYPE_INT:
			break;
		case TYPE_BOOL:
			break;
		case TYPE_ARRAY:
			break;
		case TYPE_RECORD:
			//set_var_decl_list(type->value.var_decl_list);
			//type->value.var_decl_list = tmp->declarationtype->value.var_decl_list;
			break;
	}
}
