#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "kittytree.h"
#include "memory.h"

extern int lineno;

FUNC *make_FUNC_structure(HEAD *head, BODY *body, TAIL *tail){

	FUNC *function = NEW(FUNC);
	function->lineno = lineno;
	function->functionF.head = head;
	function->functionF.body = body;
	function->functionF.tail = tail;

	return function;

}

HEAD *make_HEAD_header(char *id, PAR_DECL_LIST *pdeclist, TYPE *returntype ){

	HEAD *head = NEW(HEAD);
	head->lineno = lineno;
	head->arguments = 0;
	head->headH.id = id;
	head->headH.pdeclist = pdeclist;
	head->headH.returntype = returntype;

	return head;

}

BODY *make_BODY_funcbody(DECL_LIST *decl_list, STATEMENT_LIST 
						*statement_list){

	BODY *body = NEW(BODY);
	body->lineno = lineno;
	body->decl_list = decl_list;
	body->statement_list = statement_list;

	return body;

}

TAIL *make_TAIL_functail(char *id) {

	TAIL *tail = NEW(TAIL);
	tail->lineno = lineno;
	tail->id = id;

	return tail;

}


/*Expression constructor */
EXPRES *make_EXPRESS_term(TERM *Tterm){

	EXPRES *Eterm = NEW(EXPRES);
	Eterm->lineno = lineno;
	Eterm->kind = term_E_K;
	Eterm->value.term = Tterm;

	return Eterm;

}

EXPRES *make_EXPRESS_plus(EXPRES *left, EXPRES *right){

	EXPRES *plus = NEW(EXPRES);
	plus->lineno = lineno;
	plus->kind = plus_E_K;
	plus->value.sides.left = left;
	plus->value.sides.right = right;

	return plus;

}

EXPRES *make_EXPRESS_minus(EXPRES *left, EXPRES *right){

	EXPRES *minus = NEW(EXPRES);
	minus->lineno = lineno;
	minus->kind = minus_E_K;
	minus->value.sides.left = left;
	minus->value.sides.right = right;

	return minus;

}

EXPRES *make_EXPRESS_times(EXPRES *left ,EXPRES *right){

	EXPRES *times = NEW(EXPRES);
	times->lineno = lineno;
	times->kind = times_E_K;
	times->value.sides.left = left;
	times->value.sides.right = right;

	return times;

}

EXPRES *make_EXPRESS_divide(EXPRES *left ,EXPRES *right){

	EXPRES *divide = NEW(EXPRES);
	divide->lineno = lineno;
	divide->kind = divide_E_K;
	divide->value.sides.left = left;
	divide->value.sides.right = right;

	return divide;

}

EXPRES *make_EXPRESS_booleq(EXPRES *left ,EXPRES *right){

	EXPRES *booleq = NEW(EXPRES);
	booleq->lineno = lineno;
	booleq->kind = booleq_E_K;
	booleq->value.sides.left = left;
	booleq->value.sides.right = right;

	return booleq;

}

EXPRES *make_EXPRESS_boolneq(EXPRES *left ,EXPRES *right){

	EXPRES *boolneq = NEW(EXPRES);
	boolneq->lineno = lineno;
	boolneq->kind = boolneq_E_K;
	boolneq->value.sides.left = left;
	boolneq->value.sides.right = right;

	return boolneq;

}

EXPRES *make_EXPRESS_boolgreater(EXPRES *left ,EXPRES *right){

	EXPRES *boolgreater = NEW(EXPRES);
	boolgreater->lineno = lineno;
	boolgreater->kind = boolgreater_E_K;
	boolgreater->value.sides.left = left;
	boolgreater->value.sides.right = right;

	return boolgreater;

}

EXPRES *make_EXPRESS_boolless(EXPRES *left ,EXPRES *right){

	EXPRES *boolless = NEW(EXPRES);
	boolless->lineno = lineno;
	boolless->kind = boolless_E_K;
	boolless->value.sides.left = left;
	boolless->value.sides.right = right;

	return boolless;

}

EXPRES *make_EXPRESS_boolgeq(EXPRES *left ,EXPRES *right){

	EXPRES *boolgeq = NEW(EXPRES);
	boolgeq->lineno = lineno;
	boolgeq->kind = boolgeq_E_K;
	boolgeq->value.sides.left = left;
	boolgeq->value.sides.right = right;

	return boolgeq;

}

EXPRES *make_EXPRESS_boolleq(EXPRES *left ,EXPRES *right){

	EXPRES *boolleq = NEW(EXPRES);
	boolleq->lineno = lineno;
	boolleq->kind = boolleq_E_K;
	boolleq->value.sides.left = left;
	boolleq->value.sides.right = right;

	return boolleq;

}

EXPRES *make_EXPRESS_booland(EXPRES *left ,EXPRES *right){

	EXPRES *booland = NEW(EXPRES);
	booland->lineno = lineno;
	booland->kind = booland_E_K;
	booland->value.sides.left = left;
	booland->value.sides.right = right;

	return booland;

}

EXPRES *make_EXPRESS_boolor(EXPRES *left ,EXPRES *right){

	EXPRES *boolor = NEW(EXPRES);
	boolor->lineno = lineno;
	boolor->kind = boolor_E_K;
	boolor->value.sides.left = left;
	boolor->value.sides.right = right;

	return boolor;

}


/*TERM constructor */

TERM *make_TERM_var(VAR *var){

	TERM *Tvar = NEW(TERM);
	Tvar->lineno = lineno;
	Tvar->kind = var_T_K;
	Tvar->value.var = var;

	return Tvar;

}

TERM *make_TERM_actlist(char *id, ACT_LIST *act_list){

	TERM *actlist = NEW(TERM);
	actlist->lineno = lineno;
	actlist->kind = actList_T_K;
	actlist->value.actlistT.id = id;
	actlist->value.actlistT.actlist = act_list;

	return actlist;

}

TERM *make_TERM_bangterm(TERM *term){

	TERM *bang = NEW(TERM);
	bang->lineno = lineno;
	bang->kind = termBang_T_K;
	bang->value.term = term;

	return bang;

}

TERM *make_TERM_parenteseexp(EXPRES *expres){
	TERM *parentese = NEW(TERM);
	parentese->lineno = lineno;
	parentese->kind = expresParent_T_K;
	parentese->value.exp = expres;

	return parentese;

}

TERM *make_TERM_pipeexp(EXPRES *expres){

	TERM *pipe = NEW(TERM);
	pipe->lineno = lineno;
	pipe->kind = expresPipes_T_K;
	pipe->value.exp = expres;
	
	return pipe;

}

TERM *make_TERM_null(){

	TERM *null = NEW(TERM);
	null->lineno = lineno;
	null->kind = null_T_K;

	return null;

}
 
TERM *make_TERM_true(){

	TERM *trueT = NEW(TERM);
	trueT->lineno = lineno;
	trueT->kind = boolTrue_T_K;

	return trueT;

}

TERM *make_TERM_false(){

	TERM *falseT = NEW(TERM);
	falseT->lineno = lineno;
	falseT->kind = boolFalse_T_K;

	return falseT;

}

TERM *make_TERM_num(int intconst){

	TERM *num = NEW(TERM);
	num->lineno = lineno;
	num->kind = num_T_K;
	num->value.intconst = intconst;

	return num;

}

ACT_LIST *make_ACT_LIST_explist(EXP_LIST *exp_list){

	ACT_LIST *explist = NEW(ACT_LIST);
	explist->lineno = lineno;
	explist->kind = explist_AL_K;
	explist->value.exp_list = exp_list;

	return explist;

}

ACT_LIST *make_ACT_LIST_epsilon(){
	
	ACT_LIST *epsilon = NEW(ACT_LIST);
	epsilon->lineno = lineno;
	epsilon->kind = empty_AL_K;

	return epsilon;

}

EXP_LIST *make_EXP_LIST_expression(EXPRES *expres){

	EXP_LIST *explist = NEW(EXP_LIST);
	explist->lineno = lineno;
	explist->kind = exp_EL_K;
	explist->value.exp = expres;
	
	return explist;

}

EXP_LIST *make_EXP_LIST_commalist(EXP_LIST *explist, EXPRES *expres){

	EXP_LIST *commalist = NEW(EXP_LIST);
	commalist->lineno = lineno;
	commalist->kind = commalist_EL_K;
	commalist->value.commaEL.explist = explist;
	commalist->value.commaEL.exp = expres;

	return commalist;

}

PAR_DECL_LIST *make_PAR_DECL_LIST_vardecllist(VAR_DECL_LIST *var_declst){

	PAR_DECL_LIST *pdeclst = NEW(PAR_DECL_LIST);
	pdeclst->lineno = lineno;
	pdeclst->kind = varType_PDL_K;
	pdeclst->value.var_decl_list = var_declst;
	
	return pdeclst;

}

PAR_DECL_LIST *make_PAR_DECL_LIST_empty(){

	PAR_DECL_LIST *emptylst = NEW(PAR_DECL_LIST);
	emptylst->lineno = lineno;
	emptylst->kind = empty_PDL_K;

	return emptylst;

}

VAR_DECL_LIST *make_VAR_DECL_LIST_compound(VAR_DECL_LIST *lst, 
							VAR_TYPE *type){

	VAR_DECL_LIST *compound = NEW(VAR_DECL_LIST);
	compound->lineno = lineno;
	compound->kind = comma_VDL_K;
	compound->value.commaVDL.var_decl_list = lst;
	compound->value.commaVDL.var_type = type;

	return compound;

}

VAR_DECL_LIST *make_VAR_DECL_LIST_vartype(VAR_TYPE *type){

	VAR_DECL_LIST *vartype = NEW(VAR_DECL_LIST);
	vartype->lineno = lineno;
	vartype->kind = var_VDL_typeK;
	vartype->value.var_type = type;

	return vartype;

}

VAR_TYPE *make_VAR_TYPE_id( char *id, TYPE *type ){

	VAR_TYPE *var_type = NEW(VAR_TYPE);
	var_type->lineno = lineno;
	var_type->id = calloc(strlen(id)+1,sizeof(char));
	strcpy(var_type->id, id);
	var_type->type = type;
	

	return var_type;

}

VAR *make_VAR_id(char *id){

	VAR *nid = NEW(VAR);
	nid->lineno = lineno;
	nid->kind = id_V_K;
	nid->value.id = calloc(strlen(id)+1,sizeof(char));
	strcpy(nid->value.id, id);

	return nid;

}

VAR *make_VAR_indexing(VAR *var, EXPRES *expres){

	VAR *index = NEW(VAR);
	index->lineno = lineno;
	index->kind = indexing_V_K;
	index->value.indexingV.variable = var;
	index->value.indexingV.exp = expres;

	return index;

}

VAR *make_VAR_dot(VAR *var, char *id){

	VAR *dot = NEW(VAR);
	dot->lineno = lineno;
	dot->kind = dot_V_K;
	dot->value.dotV.id = id;
	dot->value.dotV.variable = var;

	return dot;

}

DECL_LIST *make_DECL_LIST_compound(DECL_LIST *declst, DECLARATION *dec){

	DECL_LIST *compound = NEW(DECL_LIST);
	compound->lineno = lineno;
	compound->kind = compound_DL_K;
	compound->value.compoundDL.decl_list = declst;
	compound->value.compoundDL.declaration = dec;

	return compound;

}

DECL_LIST *make_DECL_LIST_empty(){

	DECL_LIST *empty = NEW(DECL_LIST);
	empty->lineno = lineno;
	empty->kind = empty_DL_K;

	return empty;

}

DECLARATION *make_DECLARATION_typeid(char *id, TYPE *type){

	DECLARATION *typeid = NEW(DECLARATION);
	typeid->lineno = lineno;
	typeid->kind = typeassign_D_K;
	typeid->value.typedeclID.id = id;
	typeid->value.typedeclID.type = type;

	return typeid;

}

DECLARATION *make_DECLARATION_func(FUNC *function){
	
	DECLARATION *func = NEW(DECLARATION);
	func->lineno = lineno;
	func->kind = func_D_K;
	func->value.function = function;

	return func;

}

DECLARATION *make_DECLARATION_var(VAR_DECL_LIST *vardeclst){

	DECLARATION *var = NEW(DECLARATION);
	var->lineno = lineno;
	var->kind = var_D_K;
	var->value.var_decl_list = vardeclst;

	return var;

}

STATEMENT_LIST *make_STATEMENT_LIST_statement(STATEMENT *stmt){

	STATEMENT_LIST *stm = NEW(STATEMENT_LIST);
	stm->lineno = lineno;
	stm->kind = statement_SL_K;
	stm->value.statement = stmt;

	return stm;

}

STATEMENT_LIST *make_STATEMENT_LIST_compound(STATEMENT_LIST *stmtlst,
										STATEMENT *stmt){

	STATEMENT_LIST *stmlst = NEW(STATEMENT_LIST);
	stmlst->lineno = lineno;
	stmt->next = NULL;
	stmlst->kind = compound_SL_K;
	stmlst->value.compoundSL.statement = stmt;
	stmlst->value.compoundSL.statement_list = stmtlst;

	return stmlst;

}

STATEMENT *make_STATEMENT_return(EXPRES *expres){

	STATEMENT *streturn = NEW(STATEMENT);
	streturn->lineno = lineno;
	streturn->kind = return_S_K;
	streturn->value.returnS.exp = expres;
	streturn->foundReturn = 0;
	return streturn;

}

STATEMENT *make_STATEMENT_write(EXPRES *expres){

	STATEMENT *write = NEW(STATEMENT);
	write->lineno = lineno;
	write->kind = print_S_K;
	write->value.exp = expres;

	return write;

}

STATEMENT *make_STATEMENT_allocate(VAR *var, OPT_LENGTH *oplen){

	STATEMENT *allocate = NEW(STATEMENT);
	allocate->lineno = lineno;
	allocate->kind = allocate_S_K;
	allocate->value.allocateS.variable = var;
	allocate->value.allocateS.opt_length = oplen;

	return allocate;

}

STATEMENT *make_STATEMENT_assignment(VAR *var,EXPRES *expres){

	STATEMENT *assign = NEW(STATEMENT);
	assign->lineno = lineno;
	assign->kind = assign_S_K;
	assign->value.assignS.variable = var;
	assign->value.assignS.exp = expres;

	return assign;

}

STATEMENT *make_STATEMENT_ifbranch(EXPRES *expres, STATEMENT *stmt ,
			OPT_ELSE *opel){

	STATEMENT *ifbranch = NEW(STATEMENT);
	ifbranch->lineno = lineno;
	ifbranch->kind = ifbranch_S_K;
	ifbranch->value.ifbranchS.exp = expres;
	ifbranch->value.ifbranchS.statement = stmt;
	ifbranch->value.ifbranchS.opt_else = opel;

	return ifbranch;

}

STATEMENT *make_STATEMENT_while(EXPRES *expres, STATEMENT *stmt){

	STATEMENT *stwhile = NEW(STATEMENT);
	stwhile->lineno = lineno;
	stwhile->kind = while_S_K;
	stwhile->value.whileS.exp = expres;
	stwhile->value.whileS.statement = stmt;

	return stwhile;

}

STATEMENT *make_STATEMENT_compound(STATEMENT_LIST *stmlst){

	STATEMENT *compound = NEW(STATEMENT);
	compound->lineno = lineno;
	compound->kind = statement_list_S_K;
	compound->value.statement_list = stmlst;

	return compound;

}

OPT_LENGTH *make_OPT_LENGTH_oflength(EXPRES *expres){

	OPT_LENGTH *oflen = NEW(OPT_LENGTH);
	oflen->lineno = lineno;
	oflen->kind = lengthof_OL_K;
	oflen->value.exp = expres;

	return oflen;

}
	
OPT_LENGTH *make_OPT_LENGTH_empty(){

	OPT_LENGTH *empty = NEW(OPT_LENGTH);
	empty->lineno = lineno;
	empty->kind = empty_OL_K;

	return empty;

}

OPT_ELSE *make_OPT_ELSE_elsestatement(STATEMENT *stmt){

	OPT_ELSE *else_stmt = NEW(OPT_ELSE);
	else_stmt->lineno = lineno;
	else_stmt->kind = else_OE_K;
	else_stmt->value.statement = stmt;
	
	return else_stmt;

}

OPT_ELSE *make_OPT_ELSE_empty(){
	
	OPT_ELSE *empty = NEW(OPT_ELSE);
	empty->lineno = lineno;
	empty->kind = empty_OE_K;

	return empty;

}

TYPE *make_TYPE_id(char *id){

	TYPE *tid = NEW(TYPE);
	tid->lineno = lineno;
	tid->kind = id_TY_K;
	tid->value.idconst = id;

	return tid;

}

TYPE *make_TYPE_int(){
	
	TYPE *int_type = NEW(TYPE);
	int_type->lineno = lineno;
	int_type->kind = int_TY_K;
	int_type->value.typeconst = NEW_SHORT_STR;

	return int_type;

}

TYPE *make_TYPE_bool(){
	
	TYPE *bool_type = NEW(TYPE);
	bool_type->lineno = lineno;
	bool_type->kind = bool_TY_K;
	bool_type->value.typeconst = NEW_SHORT_STR;

	return bool_type;

}

TYPE *make_TYPE_arrayof(TYPE *type){

	TYPE *array = NEW(TYPE);
	array->lineno = lineno;
	array->kind = arrayof_TY_K;
	array->value.type = type;

	return array;

}

TYPE *make_TYPE_recordof(VAR_DECL_LIST *vardeclst){

	TYPE *recordof = NEW(TYPE);
	recordof->lineno = lineno;
	recordof->kind = recordof_TY_K;
	recordof->value.var_decl_list = vardeclst;

	return recordof;

}
