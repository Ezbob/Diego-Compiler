#include "kittyir.h"

static dlinked_list *instructions; // linked list of instructions
extern BODY *_main_;

linked_list *IR_build() {
	instructions = initialize_list();
	IR_builder_body(_main_);
	return instructions;
}

void IR_builder_function(FUNC *func) {

}

void IR_builder_head (HEAD *header){

}

void IR_builder_body (BODY *body){
	IR_builder_statement_list(body->statement_list);
}

void IR_builder_tail (TAIL *tail){

}

void IR_builder_type ( TYPE *type){

}

void IR_builder_par_decl_list ( PAR_DECL_LIST *pdecl){

}

void IR_builder_var_decl_list ( VAR_DECL_LIST *vdecl){

}

void IR_builder_var_type ( VAR_TYPE *vtype){

}

void IR_builder_decl_list ( DECL_LIST *dlst){

}

void IR_builder_declaration ( DECLARATION *decl){

}

void IR_builder_statement_list ( STATEMENT_LIST *slst){
	switch(slst->kind){
		case compound_SL_K:
			IR_builder_statement(slst->value.compoundSL->statement);
			IR_builder_statement_list(slst->value.compoundSL->statement_list);
			break;
		case statement_SL_K:
			IR_builder_statement(slst->value->statement);
			break;
	}
}

void IR_builder_statement ( STATEMENT *st){

} 

void IR_builder_opt_length ( OPT_LENGTH *oplen){

}

void IR_builder_opt_else ( OPT_ELSE *opel){

}

void IR_builder_variable ( VAR *var){

}

void IR_builder_expression ( EXPRES *exp){

}

void IR_builder_term ( TERM *term){

}

void IR_builder_act_list ( ACT_LIST *actlst){

}

void IR_builder_expression_list ( EXP_LIST *explst){

}
