#include "kittycheck.h"
#include "kittytype.h"
#include "../symbol/symbol.h"
#include <stdio.h>

int counter;

void check_error_report(const char* errorMsg, int lineno){
	if (lineno < 0){
		fprintf(stderr, "Error: %s \n",errorMsg);
		exit(1);
	}
	fprintf(stderr, "Error at line %i: %s \n",lineno,errorMsg);
	exit(1);
}

void begin_check(BODY *main){

	check_body(main);
}

void check_function(FUNC *func) {

	check_head(func->functionF.head);
	check_body(func->functionF.body);

}

void check_head (HEAD *header){

	check_par_decl_list(header->headH.pdeclist);
	check_type(header->headH.returntype);
}

void check_body (BODY *body){

	check_decl_list(body->decl_list);
	check_statement_list(body->statement_list);
}


void check_type ( TYPE *type){
	switch(type->kind){
		case id_TY_K:
			break;
		case int_TY_K:
			break;
		case bool_TY_K:
			break;
		case arrayof_TY_K:
			check_type(type->value.type);
			break;
		case recordof_TY_K:
			check_var_decl_list(type->value.var_decl_list);
			break;
	}
}

void check_par_decl_list ( PAR_DECL_LIST *pdecl){
	switch(pdecl->kind){
		case varType_PDL_K:
			check_var_decl_list(pdecl->value.var_decl_list);
			break;
		case empty_PDL_K:
			break;

	}
}

void check_var_decl_list ( VAR_DECL_LIST *vdecl){

	switch(vdecl->kind){
		case comma_VDL_K:
			check_var_decl_list(vdecl->value.commaVDL.var_decl_list);
			check_var_type(vdecl->value.commaVDL.var_type);
			break;
		case var_VDL_typeK:
			check_var_type(vdecl->value.var_type);
			break;
	}
}

void check_var_type ( VAR_TYPE *vtype){
	check_type(vtype->type);
}

void check_decl_list ( DECL_LIST *dlst){
	switch(dlst->kind){
		case compound_DL_K:
			check_decl_list(dlst->value.compoundDL.decl_list);
			check_declaration(dlst->value.compoundDL.declaration);
			break;
		case empty_DL_K:
			break;
	}
}


void check_declaration ( DECLARATION *decl){
	switch(decl->kind){
		case typeassign_D_K:
			check_type(decl->value.typedeclID.type);

			break;
		case func_D_K:
			check_function(decl->value.function);
			break;
		case var_D_K:

			check_var_decl_list(decl->value.var_decl_list);
			break;
	}
}

void check_statement_list ( STATEMENT_LIST *slst){
	switch(slst->kind){
		case statement_SL_K:
			check_statement(slst->value.statement);
			break;
		case compound_SL_K:
			check_statement(slst->value.compoundSL.statement);
			if(slst->value.compoundSL.statement_list == NULL){
				break;
			}
			check_statement_list(slst->value.compoundSL.statement_list);
			break;
	}
}

void check_statement ( STATEMENT *st){

	switch(st->kind){
		case return_S_K:
			check_expression(st->value.returnS.exp);
			SYMBOL *symbol = NEW(SYMBOL);

			if(st->value.returnS.exp->value.term->kind == var_T_K){
				if((symbol = getSymbol(st->symboltable, st->value.returnS.exp->value.term->value.var->value.id)) != NULL){

				if(symbol->symboltype->type != st->value.returnS.function->symboltype->value.return_type->
														   symboltype->type){
					check_error_report("Return type does not match function", 
																	st->lineno);
				}
			   }
			}	else if (st->value.returnS.exp->symboltype->type !=
			   st->value.returnS.function->symboltype->value.return_type->
														   symboltype->type) {
					check_error_report("Return type does not match function", 
																st->lineno);
			}

			break;

		case print_S_K:
			check_expression(st->value.exp);
			if(st->value.exp->symboltype->type == SYMBOL_ARRAY &&
				 st->value.exp->value.term->kind != expresPipes_T_K){
					check_error_report("Only length of arrays can be written", 
																															st->lineno);
			}
			break;

		case allocate_S_K:
			check_variable(st->value.allocateS.variable);
			check_opt_length(st->value.allocateS.opt_length);

			if(st->value.allocateS.variable->symboltype->type != SYMBOL_ARRAY ){
				check_error_report("Only arrays can be allocated", st->lineno);
			}

			break;

		case assign_S_K:
			check_variable(st->value.assignS.variable);
			check_expression(st->value.assignS.exp);

			SYMBOL *temp_sym;

			if((temp_sym = getSymbol(st->symboltable, st->value.assignS.variable->value.id)) != NULL){
				if(temp_sym->symboltype->type != st->value.assignS.exp->symboltype->type){
					check_error_report("Invalid assignment", st->lineno);
				}
			}

			break;

		case ifbranch_S_K:
			check_expression(st->value.ifbranchS.exp);
			check_statement(st->value.ifbranchS.statement);
			check_opt_else(st->value.ifbranchS.opt_else);
			if(st->value.ifbranchS.exp->
			   symboltype->type != SYMBOL_BOOL) {
				check_error_report("Expected BOOL expression", st->lineno);
			}

			break;

		case while_S_K:
			check_expression(st->value.whileS.exp);
			check_statement(st->value.whileS.statement);

			if(st->value.whileS.exp->symboltype->type != SYMBOL_BOOL){
				check_error_report("Invalid condition for Loop", st->lineno);
			}
			break;

		case statement_list_S_K:
			check_statement_list(st->value.statement_list);
			break;
	}
}
//Læængden på en allocate skal være en int
void check_opt_length ( OPT_LENGTH *oplen){
	switch(oplen->kind){
		case lengthof_OL_K:
			check_expression(oplen->value.exp);
			if(oplen->value.exp->symboltype->type != SYMBOL_INT){
				fprintf(stderr, "Allocate has to be of length int");
			}
			break;
		case empty_OL_K:
			break;
	}
}

void check_opt_else ( OPT_ELSE *opel){

	switch(opel->kind){
		case else_OE_K:
			check_statement(opel->value.statement);
			break;
		case empty_OE_K:
			break;
	}
}

void check_variable ( VAR *var){

	SYMBOL *symbol;

	switch(var->kind){
		case id_V_K:
			if((symbol = getSymbol(var->symboltable, var->value.id)) != NULL){
				var->symboltype = symbol->symboltype;
			} else{
				check_error_report("Undefined symbol", var->lineno);
			}

			break;
		/*TODO*/
		case indexing_V_K:
			check_variable(var->value.indexingV.variable);
			check_expression(var->value.indexingV.exp);
			break;
		case dot_V_K:
			check_variable(var->value.dotV.variable);
			break;
	}
}

void check_expression ( EXPRES *exp){

	SYMBOLTYPE *symbolT = NEW(SYMBOLTYPE);
	SYMBOL *check;

	switch(exp->kind){
		case term_E_K:
			check_term(exp->value.term);
			/*if(exp->value.term->kind == var_T_K){
				check = getSymbol(exp->symboltable, exp->value.term->value.var->value.id);
				exp->symboltype = check->symboltype;
			} else {*/
				symbolT = exp->value.term->symboltype;
				exp->symboltype = symbolT;
			//}
			break;

		case plus_E_K:
			check_expression(exp->value.sides.left);
			check_expression(exp->value.sides.right);

			if(exp->value.sides.right->symboltype->type != SYMBOL_INT || 
			   exp->value.sides.left->symboltype->type != SYMBOL_INT ) { 
				check_error_report("Expected exp int", exp->lineno);
			} else {
				symbolT->type = SYMBOL_INT;
				exp->symboltype = symbolT;
			}
			break;

		case minus_E_K:
			check_expression(exp->value.sides.left);
			check_expression(exp->value.sides.right);


			if(exp->value.sides.right->symboltype->type != SYMBOL_INT || 
			   exp->value.sides.left->symboltype->type != SYMBOL_INT ) { 
				check_error_report("Expected exp int", exp->lineno);
			} else {
				symbolT->type = SYMBOL_INT;
				exp->symboltype = symbolT;
			}
			break;

		case times_E_K:
			check_expression(exp->value.sides.right);
			check_expression(exp->value.sides.left);

			if(exp->value.sides.left->symboltype->type != SYMBOL_INT || 
			   exp->value.sides.right->symboltype->type != SYMBOL_INT ) { 
				check_error_report("Expected exp int", exp->lineno);
			} else {
				symbolT->type = SYMBOL_INT;
				exp->symboltype = symbolT;
			}
			break;

		case divide_E_K:
			check_expression(exp->value.sides.left);
			check_expression(exp->value.sides.right);

			if(exp->value.sides.right->symboltype->type != SYMBOL_INT || 
			   exp->value.sides.left->symboltype->type != SYMBOL_INT ) { 
				check_error_report("Expected exp int", exp->lineno);
			} else {
				symbolT->type = SYMBOL_INT;
				exp->symboltype = symbolT;
			}
			break;

		//array og record må være null, bool skal være med bool, int skal være med int 
		case booleq_E_K:
		case boolneq_E_K:
			check_expression(exp->value.sides.left);
			check_expression(exp->value.sides.right);
			if(exp->value.sides.right->symboltype->type == SYMBOL_INT || 
			   exp->value.sides.left->symboltype->type == SYMBOL_INT ) { 
				symbolT->type = SYMBOL_BOOL;
				exp->symboltype = symbolT;
				break;
			}
			if(exp->value.sides.right->symboltype->type == SYMBOL_BOOL || 
			   exp->value.sides.left->symboltype->type == SYMBOL_BOOL ) { 
				symbolT->type = SYMBOL_BOOL;
                                exp->symboltype = symbolT;
				break;
			}
			if(exp->value.sides.right->symboltype->type == SYMBOL_ARRAY || 
			   exp->value.sides.left->symboltype->type == SYMBOL_NULL ) { 
				symbolT->type = SYMBOL_BOOL;
                                exp->symboltype = symbolT;
				break;
			}
			if(exp->value.sides.right->symboltype->type == SYMBOL_NULL || 
			   exp->value.sides.left->symboltype->type == SYMBOL_ARRAY ) { 
				symbolT->type = SYMBOL_BOOL;
                                exp->symboltype = symbolT;
				break;
			}
			if(exp->value.sides.right->symboltype->type == SYMBOL_ARRAY || 
			   exp->value.sides.left->symboltype->type == SYMBOL_ARRAY ) { 
				symbolT->type = SYMBOL_BOOL;
                                exp->symboltype = symbolT;
				break;
			}

			if(exp->value.sides.right->symboltype->type == SYMBOL_RECORD ||
			   exp->value.sides.left->symboltype->type == SYMBOL_NULL ) {
				symbolT->type = SYMBOL_BOOL;
                                exp->symboltype = symbolT;
				break;
			}

			if(exp->value.sides.right->symboltype->type == SYMBOL_NULL ||
			   exp->value.sides.left->symboltype->type == SYMBOL_RECORD ) {
				symbolT->type = SYMBOL_BOOL;
                                exp->symboltype = symbolT;
				break;
			}

			if(exp->value.sides.right->symboltype->type == SYMBOL_RECORD ||
			   exp->value.sides.left->symboltype->type == SYMBOL_RECORD ) {
				symbolT->type = SYMBOL_BOOL;
                                exp->symboltype = symbolT;
				break;
			}

			check_error_report("Invalid comparison types", exp->lineno);
			break;


		case boolgreater_E_K:
			check_expression(exp->value.sides.left);
			check_expression(exp->value.sides.right);

			if(exp->value.sides.right->symboltype->type != SYMBOL_INT ||
			   exp->value.sides.left->symboltype->type != SYMBOL_INT ) {
				check_error_report("Expected exp int", exp->lineno);
			} else {
				symbolT->type = SYMBOL_BOOL;
                                exp->symboltype = symbolT;
			}
			break;

		case boolless_E_K:
			check_expression(exp->value.sides.left);
			check_expression(exp->value.sides.right);

			if(exp->value.sides.right->symboltype->type != SYMBOL_INT ||
			   exp->value.sides.left->symboltype->type != SYMBOL_INT ) {
				check_error_report("Expected exp int", exp->lineno);
			} else {
				symbolT->type = SYMBOL_BOOL;
				exp->symboltype = symbolT;
			}
			break;

		case boolleq_E_K:
			check_expression(exp->value.sides.left);
			check_expression(exp->value.sides.right);

			if(exp->value.sides.right->symboltype->type != SYMBOL_INT || 
			   exp->value.sides.left->symboltype->type != SYMBOL_INT ) { 
				check_error_report("Expected exp int", exp->lineno);
			} else {
				symbolT->type = SYMBOL_BOOL;
                                exp->symboltype = symbolT;
			}
			break;

		case boolgeq_E_K:
			check_expression(exp->value.sides.left);
			check_expression(exp->value.sides.right);

			if(exp->value.sides.right->symboltype->type != SYMBOL_INT || 
			   exp->value.sides.left->symboltype->type != SYMBOL_INT ) { 
				check_error_report("Expected exp int", exp->lineno);
			} else {
				symbolT->type = SYMBOL_BOOL;
                                exp->symboltype = symbolT;
			}
			break;

		case booland_E_K:
			check_expression(exp->value.sides.left);
			check_expression(exp->value.sides.right);
			if(exp->value.sides.right->symboltype->type != SYMBOL_BOOL || 
			   exp->value.sides.left->symboltype->type != SYMBOL_BOOL) { 
				check_error_report("Expected exp bool", exp->lineno);
			} else {
				symbolT->type = SYMBOL_BOOL;
                                exp->symboltype = symbolT;
			}
			break;
		case boolor_E_K:
			check_expression(exp->value.sides.left);
			check_expression(exp->value.sides.right);
			if(exp->value.sides.right->symboltype->type != SYMBOL_BOOL || 
			   exp->value.sides.left->symboltype->type != SYMBOL_BOOL) { 
				check_error_report("Expected exp bool", exp->lineno);
			} else {
				symbolT->type = SYMBOL_BOOL;
				exp->symboltype = symbolT;
			}
			break;
	}
}

int check_term ( TERM *term){
	SYMBOLTYPE *symbolT = NEW(SYMBOLTYPE);
	SYMBOL *symbol;// = NEW(SYMBOL);

	int count = 0;

	switch(term->kind){
		case var_T_K:
			check_variable(term->value.var);
			symbol = getSymbol(term->symboltable, term->value.var->value.id);
			term->symboltype = symbol->symboltype;
			break;

		case actList_T_K:
			check_act_list(term->value.actlistT.actlist);
			symbol = getSymbol(term->symboltable, term->value.actlistT.id);
			if(symbol == NULL || symbol->symboltype->type != SYMBOL_FUNCTION ){
				check_error_report("Function not defined or not a function", term->lineno);
			}

			if((term->value.actlistT.actlist->kind == empty_AL_K && 
			   symbol->parameters != empty_PDL_K) ||
				(term->value.actlistT.actlist->kind != empty_AL_K && 
			   symbol->parameters == empty_PDL_K)) {

				check_error_report("Wrong no. of parameters", term->lineno);
			}

			EXP_LIST *callParameters = term->value.actlistT.actlist->value.exp_list;
			VAR_DECL_LIST *functionParameters = symbol->parameters->value.var_decl_list;


			while( callParameters != NULL && functionParameters != NULL && count < symbol->noArguments){
				
				if(callParameters->kind == commalist_EL_K &&
				   functionParameters->kind == comma_VDL_K){
					if(callParameters->value.commaEL.exp->symboltype->type !=
					   functionParameters->value.commaVDL.var_type->symbol->symboltype->type){
						check_error_report("Wrong parameter type", term->lineno);
					}
				} else if(callParameters->kind == exp_EL_K &&
					      functionParameters->kind == var_VDL_typeK){
					if(callParameters->value.exp->symboltype->type !=
					   functionParameters->value.var_type->symbol->symboltype->type)
						check_error_report("Wrong parameter type", term->lineno);
				} 
				callParameters = callParameters->value.commaEL.explist;
				functionParameters = functionParameters->value.commaVDL.var_decl_list;
				
				count++;
			}

			if(count != symbol->noArguments ){
				check_error_report("Wrong no. of parameters", term->lineno);
			}

			symbolT = symbol->symboltype->value.return_type->symboltype;
			term->symboltype = symbolT;
			break;

		case termBang_T_K:
			check_term(term->value.term);
			if(term->value.term->symboltype->type == SYMBOL_BOOL){
				term->symboltype = term->value.term->symboltype;
			} else {
				check_error_report("Expected term bool", term->lineno);
			}
			break;

		case expresParent_T_K:
			check_expression(term->value.exp);
			term->symboltype = term->value.exp->symboltype;
			break;

		case expresPipes_T_K:
			check_expression(term->value.exp);
			if(term->value.exp->value.term->symboltype->type == SYMBOL_BOOL ||
			   term->value.exp->value.term->symboltype->type == SYMBOL_ID ||
			   term->value.exp->value.term->symboltype->type == SYMBOL_RECORD ||
			   term->value.exp->value.term->symboltype->type == SYMBOL_FUNCTION){
				check_error_report("Expected term int", term->lineno);
			}
			term->symboltype = term->value.exp->symboltype;
			break;

		case null_T_K:
			symbolT->type = SYMBOL_NULL;
			term->symboltype = symbolT;
			break;

		case boolTrue_T_K:
		case boolFalse_T_K:
			symbolT->type = SYMBOL_BOOL;
			term->symboltype = symbolT;
			break;

		case num_T_K:
			symbolT->type = SYMBOL_INT;
			term->symboltype = symbolT;
			break;

		default:
			break;
	}
	return 0;
}

void check_act_list ( ACT_LIST *actlst){
	switch(actlst->kind){
		case explist_AL_K:
			check_expression_list(actlst->value.exp_list);
			break;
		case empty_AL_K:
			break;
	}
}

void check_expression_list ( EXP_LIST *explst){
	switch(explst->kind){
		case exp_EL_K:
			check_expression(explst->value.exp);
			break;
		case commalist_EL_K:
			check_expression_list(explst->value.commaEL.explist);
			check_expression(explst->value.commaEL.exp);
			break;
	}
}
