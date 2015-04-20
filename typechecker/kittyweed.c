#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kittyweed.h"
#include "../symbol/symbol.h"
#include "stack.h"
#define TRUE 1 
#define FALSE 0
 
stackT *the_stack;

void weed_error_report(const char* errorMsg, int lineno){
	if (lineno < 0){
		fprintf(stderr, "Error: %s \n",errorMsg);
		exit(WEEDER_FAILURE);
	}
	fprintf(stderr, "Error at line %i: %s \n",lineno,errorMsg);
	exit(WEEDER_FAILURE);
}


BODY *begin_weed(BODY *body){

	/*
	 * Using a stack to keep track of functions,
	 * need this to attach to statements to make  
	 * sure the return type is correct
	 */
	the_stack = funcStackInit(HASH_SIZE);

	body = weed_body(body);

	return body;

}


/* TODO */
FUNC *weed_function ( FUNC *function ){

	FUNC *assertion;

	funcStackPush(the_stack, function);

	if(strcmp(function->functionF.head->headH.id, 
					function->functionF.tail->id) != 0){
		weed_error_report("Mismatch in header and tail id of function",
											function->functionF.head->lineno);
	}

	weed_head(function->functionF.head);
	weed_body(function->functionF.body);


	if( function->functionF.body->statement_list->value.compoundSL.statement->foundReturn == 0 ||
	    function->functionF.body->statement_list == NULL ){

		weed_error_report("No return in function", function->functionF.head->lineno);
	}

	if((assertion = funcStackPop(the_stack)) == NULL){
		weed_error_report("Cant pop function pointer from stack, empty",
											function->functionF.head->lineno);
	}

	return function;

}

HEAD *weed_head ( HEAD *header){

	header->headH.pdeclist = weed_par_decl_list(header->headH.pdeclist);
	header->headH.returntype = weed_type(header->headH.returntype);

	return header;

}

TYPE *weed_type ( TYPE *type){

	switch(type->kind){

		case arrayof_TY_K:
			type->value.type = weed_type(type->value.type);
			break;

		case recordof_TY_K:
			type->value.var_decl_list = weed_var_decl_list(type->value.var_decl_list);
			break;

		default:
			break; //id, int, bool
	}

	return type;

}

PAR_DECL_LIST *weed_par_decl_list ( PAR_DECL_LIST *pdecl){


	switch(pdecl->kind){

		case varType_PDL_K:
			pdecl->value.var_decl_list = weed_var_decl_list(pdecl->value.var_decl_list);
			break;

		case empty_PDL_K:
			break;
	}

	return pdecl;

}

VAR_DECL_LIST *weed_var_decl_list ( VAR_DECL_LIST *vdecl){ 

	switch(vdecl->kind){

		case comma_VDL_K:
			if(vdecl->value.commaVDL.var_decl_list != NULL){
				//vdecl->value.var_type = weed_var_type(vdecl->value.var_type);
				vdecl->value.commaVDL.var_decl_list = weed_var_decl_list(vdecl->value.commaVDL.var_decl_list);
			}
		break;

		case var_VDL_typeK:
			vdecl->value.var_type = weed_var_type(vdecl->value.var_type);
			break;

	}
	return vdecl;

}

VAR_TYPE *weed_var_type ( VAR_TYPE *vtype){

	vtype->type = weed_type(vtype->type);

	return vtype;
}


OPT_LENGTH *weed_opt_length ( OPT_LENGTH *oplen){

	switch(oplen->kind){

		case lengthof_OL_K:
			oplen->value.exp = weed_expression(oplen->value.exp);
			break;

		case empty_OL_K:
			break;
	}

	return oplen;

}


VAR *weed_variable ( VAR *var){

	switch(var->kind){
		case id_V_K:
			break;

		case indexing_V_K:
			var->value.indexingV.variable = weed_variable(var->value.indexingV.variable);
			var->value.indexingV.exp = weed_expression(var->value.indexingV.exp);
			break;

		case dot_V_K:
			var->value.dotV.variable = weed_variable(var->value.dotV.variable);
			break;
	}


	return var;

}


ACT_LIST *weed_act_list ( ACT_LIST *actlst){

	switch(actlst->kind){

		case explist_AL_K:
			actlst->value.exp_list = weed_expression_list(actlst->value.exp_list);
			break;
		case empty_AL_K:
			break;
	}


	return actlst;

}


EXP_LIST *weed_expression_list ( EXP_LIST *explst){

	switch(explst->kind){

		case exp_EL_K:
			explst->value.exp = weed_expression(explst->value.exp);
			break;

		case commalist_EL_K:
			explst->value.commaEL.exp = weed_expression(explst->value.commaEL.exp);
			explst->value.commaEL.explist = weed_expression_list(explst->value.commaEL.explist);
			break;
	}


	return explst;

}




/* TODO */
BODY *weed_body ( BODY *body ){

	body->decl_list = weed_decl_list(body->decl_list);
	body->statement_list = weed_statement_list(body->statement_list);

	return body;

}

/* TODO */
DECL_LIST *weed_decl_list ( DECL_LIST *dlst ){
	switch(dlst->kind){
		case compound_DL_K:
			dlst->value.compoundDL.decl_list = weed_decl_list(dlst->value.compoundDL.decl_list);
			dlst->value.compoundDL.declaration = weed_declaration(dlst->value.compoundDL.declaration);
			break;

		case empty_DL_K:
			break;

		default:
			break;
	}

	return dlst;

}


DECLARATION *weed_declaration ( DECLARATION *decl ){
	switch(decl->kind){

		case typeassign_D_K:
			decl->value.typedeclID.type = weed_type(decl->value.typedeclID.type);
			break;

		case func_D_K:
			decl->value.function = weed_function(decl->value.function);
			break;

		case var_D_K:
			decl->value.var_decl_list = weed_var_decl_list(decl->value.var_decl_list);
			break;

		default:
			break;
	}

	return decl;

}

STATEMENT_LIST *weed_statement_list ( STATEMENT_LIST *slst ){

	if(slst == NULL){
		return NULL;
	}

	slst->value.compoundSL.statement_list = weed_statement_list(slst->value.compoundSL.statement_list);
	slst->value.compoundSL.statement = weed_statement(slst->value.compoundSL.statement);

	if(slst->value.statement == NULL) {
		return slst->value.compoundSL.statement_list;
	 }

	/*
	 * Trying to remove statements after a return,
	 * but returns after a return is still not weeded
	 */
	if(slst->value.compoundSL.statement_list != NULL &&
	   slst->value.compoundSL.statement_list->value.statement->foundReturn == 1) {
		return slst->value.compoundSL.statement_list;
	}

	return slst;

}


STATEMENT *weed_statement ( STATEMENT *st ){

	st->foundReturn = 0;
	TERM *ifTerm = NEW(TERM);

	switch(st->kind){
		case return_S_K:
			st->value.returnS.exp = weed_expression(st->value.returnS.exp);
			if(the_stack->top == NULL){
				weed_error_report("Return not associated with function",
																st->lineno);
			}
			st->value.returnS.function = the_stack->top->function;
			st->foundReturn = 1;
			return st;


		case ifbranch_S_K:
			st->value.ifbranchS.exp = weed_expression(st->value.ifbranchS
																		.exp);
			st->value.ifbranchS.statement = weed_statement(st->value.ifbranchS
																  .statement);
			st->value.ifbranchS.opt_else = weed_opt_else(st->value.ifbranchS
																   .opt_else);
			if(st->value.ifbranchS.opt_else->kind == empty_OE_K &&
			   st->value.ifbranchS.statement == NULL){
					return NULL;
			}

			//Case where there exists a return in both if and else, can ignore everything after the if/else
			if(st->value.ifbranchS.statement->foundReturn == 1 &&
			   st->value.ifbranchS.opt_else->value.statement->foundReturn == 1 &&
         st->value.ifbranchS.opt_else->kind != empty_OE_K &&
			   st->value.ifbranchS.statement != NULL) {

			   st->foundReturn = 1;

			}
			if (st->value.ifbranchS.exp->kind == term_E_K){
				// a term
				ifTerm = weed_term(st->value.ifbranchS.exp->value.term);
				if(ifTerm->kind == num_T_K){
					weed_error_report("Invalid expression", st->lineno - 1);
					break;
				}

				if (ifTerm->kind == boolFalse_T_K && (st->value.ifbranchS.opt_else->kind != empty_OE_K)){
					// if FALSE then we should only look at else
					return st->value.ifbranchS.opt_else->value.statement;
				}else if (ifTerm->kind == boolTrue_T_K){
					// if TRUE then we should only look at if
					return st->value.ifbranchS.statement;
				}
			}

			break;
		case statement_list_S_K:
			st->value.statement_list = weed_statement_list(st->value.statement_list);
			if(st->value.statement_list == NULL){
				return NULL;
			}
			//Look for returns
			if(st->value.statement_list != NULL){
				st->foundReturn = st->value.statement_list->value.statement->foundReturn;
			}
			break;

		case assign_S_K:
			st->value.assignS.variable = weed_variable(st->value.assignS.variable);
			st->value.assignS.exp = weed_expression(st->value.assignS.exp);
			break;

		case print_S_K:
			st->value.exp = weed_expression(st->value.exp);
			break;

		case allocate_S_K:
			st->value.allocateS.variable = weed_variable(st->value.allocateS.variable);
			st->value.allocateS.opt_length = weed_opt_length(st->value.allocateS.opt_length);
			break;

		case while_S_K:
			st->value.whileS.statement = weed_statement(st->value.whileS.statement);

		default:
			break;

	}
	return st;

}

/* TODO */
OPT_ELSE *weed_opt_else ( OPT_ELSE *opel){
	switch(opel->kind){
		case else_OE_K:
			opel->value.statement = weed_statement(opel->value.statement);
			if (opel->value.statement == NULL){
				opel->kind = empty_OE_K;
			}
			break;

		case empty_OE_K:
			break;

		default:
			break;
	}

	return opel;
}

EXPRES *weed_expression( EXPRES *exp ){


	EXPRES *left_exp;
	EXPRES *right_exp;
	TERM *left_term;
	TERM *right_term;

	if(exp->kind == term_E_K ){
		exp->value.term = weed_term(exp->value.term);
		return exp;
	}

	exp->value.sides.left = weed_expression(exp->value.sides.left);
	exp->value.sides.right = weed_expression(exp->value.sides.right);

	left_exp = exp->value.sides.left;
	right_exp = exp->value.sides.right;

	switch(exp->kind){

		case booland_E_K:

			left_term  = left_exp->value.term;
			right_term = right_exp->value.term;

			if((left_term->kind == boolTrue_T_K || left_term->kind 
													== boolFalse_T_K) &&
				 (right_term->kind == boolTrue_T_K || right_term->kind 
				 									== boolFalse_T_K)){

				if(	left_exp->value.term->kind == boolTrue_T_K && 
								right_exp->value.term->kind	== boolTrue_T_K){
					exp->value.term = make_TERM_true();
					exp->kind = term_E_K;
				} else {
					exp->value.term = make_TERM_false();
					exp->kind = term_E_K;
				}
		 	}
			break;

		case boolor_E_K:

			left_term  = left_exp->value.term;
			right_term = right_exp->value.term;

			if((left_term->kind == boolTrue_T_K || 
										left_term->kind == boolFalse_T_K) &&
				(right_term->kind == boolTrue_T_K || right_term->kind == 
															boolFalse_T_K)){
				if(	left_exp->value.term->kind == boolFalse_T_K && 
								right_exp->value.term->kind	== boolFalse_T_K){
					exp->value.term = make_TERM_false();
					exp->kind = term_E_K;
				} else {
					exp->value.term = make_TERM_true();
					exp->kind = term_E_K;
				}
			}
			break;

		case plus_E_K:
			if( left_exp->kind == term_E_K && right_exp->kind == term_E_K ){
				left_term = left_exp->value.term;
				right_term = right_exp->value.term;
				if(left_term->kind != num_T_K || 
				   right_term->kind != num_T_K ) {
					break;
				}

				exp->value.term = make_TERM_num(left_term->value.intconst + 
												right_term->value.intconst);

				exp->kind = term_E_K;
			}
			break;

		case minus_E_K:

			if( left_exp->kind == term_E_K && right_exp->kind == term_E_K ){

				left_term = left_exp->value.term;
				right_term = right_exp->value.term;

				if(left_term->kind != num_T_K || 
				   right_term->kind != num_T_K ) {
					break;
				}

				exp->value.term = make_TERM_num(left_term->value.intconst - 
												right_term->value.intconst);

				exp->kind = term_E_K;
			}

			break;

		case times_E_K:

			if( left_exp->kind == term_E_K && right_exp->kind == term_E_K ){

				left_term = left_exp->value.term;
				right_term = right_exp->value.term;

				if(left_term->kind != num_T_K || 
				   right_term->kind != num_T_K ) {
					break;
				}

				exp->value.term = make_TERM_num(left_term->value.intconst * 
												right_term->value.intconst);

				exp->kind = term_E_K;

			}

			break;

		case divide_E_K:

			if( left_exp->kind == term_E_K && right_exp->kind == term_E_K ){

					left_term = left_exp->value.term;
					right_term = right_exp->value.term;

					if(left_term->kind != num_T_K || 
					   right_term->kind != num_T_K ) {
						break;
					}

					if(right_term->value.intconst == 0){
						weed_error_report("Division by 0", exp->lineno);
					}

					int temp_res = left_term->value.intconst / right_term->
															value.intconst;			

					exp->value.term = make_TERM_num(temp_res);
					exp->kind = term_E_K;

			}

			break;

		case boolleq_E_K:

			if( left_exp->kind == term_E_K && right_exp->kind == term_E_K ){

				left_term = left_exp->value.term;
				right_term = right_exp->value.term;

				if(left_term->kind != num_T_K || 
				   right_term->kind != num_T_K ) {
					break;
				}

				if( left_term->value.intconst <= right_term->value.intconst ){
						exp->value.term = make_TERM_true();
						exp->kind = term_E_K;
				} else {
						exp->value.term = make_TERM_false();
						exp->kind = term_E_K;
				}

			}

			break;

		case boolgeq_E_K:

			if( left_exp->kind == term_E_K && right_exp->kind == term_E_K ){

				left_term = left_exp->value.term;
				right_term = right_exp->value.term;

				if(left_term->kind != num_T_K || 
				   right_term->kind != num_T_K ) {
					break;
				}

				if( left_term->value.intconst >= right_term->value.intconst ){
						exp->value.term = make_TERM_true();
						exp->kind = term_E_K;
				} else {
						exp->value.term = make_TERM_false();
						exp->kind = term_E_K;
				}

			}

			break;

		case booleq_E_K:
			if( left_exp->kind == term_E_K && right_exp->kind == term_E_K ){
				left_term = left_exp->value.term;
				right_term = right_exp->value.term;
				if(left_term->kind == num_T_K && 
				    right_term->kind == num_T_K ) { 
					if( left_term->value.intconst == right_term->value.intconst ){
						exp->value.term = make_TERM_true();
						exp->kind = term_E_K;
					} else {
						exp->value.term = make_TERM_false();
						exp->kind = term_E_K;
					}
				}
			}
			break;

		case boolneq_E_K:

			if( left_exp->kind == term_E_K && right_exp->kind == term_E_K ){


				left_term = left_exp->value.term;
				right_term = right_exp->value.term;
				if(left_term->kind == num_T_K && 
				    right_term->kind == num_T_K ) { 
					if( left_term->value.intconst != 
						right_term->value.intconst ){
						exp->value.term = make_TERM_true();
						exp->kind = term_E_K;
					} else {
						exp->value.term = make_TERM_false();
						exp->kind = term_E_K;
					}
				}
			}
			break;

		case boolgreater_E_K:

			if( left_exp->kind == term_E_K && right_exp->kind == term_E_K ){

				left_term = left_exp->value.term;
				right_term = right_exp->value.term;

				if(left_term->kind != num_T_K || 
				   right_term->kind != num_T_K ) {
					break;
				}

				if( left_term->value.intconst > right_term->value.intconst ){
						exp->value.term = make_TERM_true();
						exp->kind = term_E_K;
				} else {
						exp->value.term = make_TERM_false();
						exp->kind = term_E_K;
				}

			}

			break;

		case boolless_E_K:

			if( left_exp->kind == term_E_K && right_exp->kind == term_E_K ){

				left_term = left_exp->value.term;
				right_term = right_exp->value.term;

				if(left_term->kind != num_T_K || 
				   right_term->kind != num_T_K ) {
					break;
				}
				if( left_term->value.intconst < right_term->value.intconst ){
						exp->value.term = make_TERM_true();
						exp->kind = term_E_K;
				} else {
						exp->value.term = make_TERM_false();	
						exp->kind = term_E_K;
				}
	
			}
			break;
		default:
			break;
	}

	return exp;

}


TERM *weed_term ( TERM *term){

	TERM *between = NEW(TERM); 

	switch(term->kind){

		case var_T_K:
			term->value.var = weed_variable(term->value.var);
			break;

		case actList_T_K:
			term->value.actlistT.actlist = weed_act_list(term->value.actlistT.actlist);
			break;

		case termBang_T_K:
			term->value.term = weed_term(term->value.term);

			if (term->value.term->kind == boolTrue_T_K){
				term->kind = boolFalse_T_K;
			}

			if (term->value.term->kind == boolFalse_T_K){
				term->kind = boolTrue_T_K;
			}
			
			break;

		case expresParent_T_K:
			term->value.exp = weed_expression(term->value.exp);
			if (term->value.exp->kind == term_E_K){
				between = term->value.exp->value.term;
				if(between->kind == var_T_K || between->kind == num_T_K 
				   || between->kind == boolTrue_T_K || between->kind == 
     				boolFalse_T_K || between->kind == null_T_K){

					term->kind = between->kind;
					term->value = between->value;
				}
			break; 

		case expresPipes_T_K: //Absolut værdi |-værdi| = værdi
			term->value.exp = weed_expression(term->value.exp);
			if (term->value.exp->kind == term_E_K){
				between = term->value.exp->value.term;
				if (between->kind == num_T_K){
					term->kind = num_T_K;
					if(between->value.intconst < 0){
						term->value.intconst = between->value.intconst *(-1);
					} else{
						term->value.intconst = between->value.intconst;
					}

				}
			}
			break;

		case null_T_K:
			break;
		case boolTrue_T_K:
			break;
		case boolFalse_T_K:
			break;
		case num_T_K:
			break;

		default:
			break;
	}
		
}
return term;
}




