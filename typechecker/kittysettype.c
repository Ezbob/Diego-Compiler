#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbol/symbol.h"
#include "parserscanner/kittytree.h"
#include "kittytype.h"
#include "kittysettype.h"

void begin_set( BODY *body ){

	set_body(body);
}

void set_function ( FUNC *function ){

	set_head(function->functionF.head);
	set_body(function->functionF.body);

}

void set_head ( HEAD *header ){

	set_par_decl_list(header->headH.pdeclist);
	set_type(header->headH.returntype);

}
	
void set_body ( BODY *body ) {

	set_decl_list(body->decl_list);

}

void set_par_decl_list ( PAR_DECL_LIST *pdecl ) {

	switch(pdecl->kind){
		case varType_PDL_K:
			set_var_decl_list(pdecl->value.var_decl_list);
			break;
		case empty_PDL_K:
			break;
	}
}

void set_var_decl_list ( VAR_DECL_LIST *vdecl ) {

	switch(vdecl->kind){
		case comma_VDL_K:
			set_var_decl_list(vdecl->value.commaVDL.var_decl_list);
			set_var_type(vdecl->value.commaVDL.var_type);
			break;
		case var_VDL_typeK:
			set_var_type(vdecl->value.var_type);
			break;
	}
}
void set_var_type ( VAR_TYPE *vtype ) {
	set_type(vtype->type);
	vtype->symbol->symboltype = vtype->type->symboltype;

}

void set_decl_list ( DECL_LIST *dlst ) {

	switch(dlst->kind){
		case compound_DL_K:
			set_decl_list(dlst->value.compoundDL.decl_list);
			set_declaration(dlst->value.compoundDL.declaration);
			break;
		case empty_DL_K:
			break;
	}	
}

void set_declaration ( DECLARATION *decl ) {

	switch(decl->kind){
		case typeassign_D_K:
			set_type(decl->value.typedeclID.type);
			break;
		case func_D_K:
			set_function(decl->value.function);
			break;
		case var_D_K:
			set_var_decl_list(decl->value.var_decl_list);
			break;
	}
}
void set_type ( TYPE *type ) {

	SYMBOL *tmp;

	switch(type->kind){
		case id_TY_K:
			tmp = getSymbol(type->symboltable, type->value.idconst);

			if(tmp->symboltype->type == SYMBOL_INT){
				type->kind = int_TY_K;
				type->symboltype->type = SYMBOL_INT;
				type->symboltype->visited = 1;
			}

			if(tmp->symboltype->type == SYMBOL_BOOL){
				type->kind = bool_TY_K;
				type->symboltype->type = SYMBOL_BOOL;
				type->symboltype->visited = 1;
			}

			if(tmp->symboltype->type == SYMBOL_ARRAY){
				type->kind = arrayof_TY_K;
				type->symboltype->type = SYMBOL_ARRAY;
				type->symboltype->visited = 1;
			}

			if(tmp->symboltype->type == SYMBOL_RECORD){
				type->kind = recordof_TY_K;
				type->symboltype->type = SYMBOL_RECORD;
				type->symboltype->visited = 1;
			}
			break;
		case int_TY_K:
			break;
		case bool_TY_K:
			break;
		case arrayof_TY_K:
			break;
		case recordof_TY_K:
			break;
	}
}
