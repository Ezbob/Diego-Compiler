#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "kittytree.h"
#include "memory.h"

extern int lineno;

FUNC *make_FUNC(HEAD *head, BODY *body, TAIL *tail){

	FUNC *function = NEW(FUNC);
	function->lineno = lineno;
	function->head = head;
	function->body = body;
	function->tail = tail;

	return function;

}

HEAD *make_HEAD(char *id, PAR_DECL_LIST *pdeclist, TYPE *returntype ){

	HEAD *head = NEW(HEAD);
	head->lineno = lineno;
	head->arguments = 0;
	head->id = id;
	head->pdlist = pdeclist;
	head->returntype = returntype;

	return head;

}

BODY *make_BODY(DECL_LIST *decl_list, STATEMENT_LIST 
						*statement_list){

	BODY *body = NEW(BODY);
	body->lineno = lineno;
	body->decl_list = decl_list;
	body->statement_list = statement_list;

	return body;

}

TAIL *make_TAIL(char *id) {

	TAIL *tail = NEW(TAIL);
	tail->lineno = lineno;
	tail->id = id;

	return tail;

}


/*Expression constructor */
EXPRES *make_EXPRES_TERM(TERM *Tterm){

	EXPRES *Eterm = NEW(EXPRES);
	Eterm->lineno = lineno;
	Eterm->kind = EXPRES_TERM;
	Eterm->value.term = Tterm;

	return Eterm;

}

EXPRES *make_EXPRES_PLUS(EXPRES *left, EXPRES *right){

	EXPRES *plus = NEW(EXPRES);
	plus->lineno = lineno;
	plus->kind = EXPRES_PLUS;
	plus->value.sides.left = left;
	plus->value.sides.right = right;

	return plus;

}

EXPRES *make_EXPRES_MINUS(EXPRES *left, EXPRES *right){

	EXPRES *minus = NEW(EXPRES);
	minus->lineno = lineno;
	minus->kind = EXPRES_MINUS;
	minus->value.sides.left = left;
	minus->value.sides.right = right;

	return minus;

}

EXPRES *make_EXPRES_TIMES(EXPRES *left ,EXPRES *right){

	EXPRES *times = NEW(EXPRES);
	times->lineno = lineno;
	times->kind = EXPRES_TIMES;
	times->value.sides.left = left;
	times->value.sides.right = right;

	return times;

}

EXPRES *make_EXPRES_DIVIDE(EXPRES *left ,EXPRES *right){

	EXPRES *divide = NEW(EXPRES);
	divide->lineno = lineno;
	divide->kind = EXPRES_DIVIDE;
	divide->value.sides.left = left;
	divide->value.sides.right = right;

	return divide;

}

EXPRES *make_EXPRES_EQ(EXPRES *left ,EXPRES *right){

	EXPRES *booleq = NEW(EXPRES);
	booleq->lineno = lineno;
	booleq->kind = EXPRES_EQ;
	booleq->value.sides.left = left;
	booleq->value.sides.right = right;

	return booleq;

}

EXPRES *make_EXPRES_NEQ(EXPRES *left ,EXPRES *right){

	EXPRES *boolneq = NEW(EXPRES);
	boolneq->lineno = lineno;
	boolneq->kind = EXPRES_NEQ;
	boolneq->value.sides.left = left;
	boolneq->value.sides.right = right;

	return boolneq;

}

EXPRES *make_EXPRES_GREATER(EXPRES *left ,EXPRES *right){

	EXPRES *boolgreater = NEW(EXPRES);
	boolgreater->lineno = lineno;
	boolgreater->kind = EXPRES_GREATER;
	boolgreater->value.sides.left = left;
	boolgreater->value.sides.right = right;

	return boolgreater;

}

EXPRES *make_EXPRES_LESS(EXPRES *left ,EXPRES *right){

	EXPRES *boolless = NEW(EXPRES);
	boolless->lineno = lineno;
	boolless->kind = EXPRES_LESS;
	boolless->value.sides.left = left;
	boolless->value.sides.right = right;

	return boolless;

}

EXPRES *make_EXPRES_GEQ(EXPRES *left ,EXPRES *right){

	EXPRES *boolgeq = NEW(EXPRES);
	boolgeq->lineno = lineno;
	boolgeq->kind = EXPRES_GEQ;
	boolgeq->value.sides.left = left;
	boolgeq->value.sides.right = right;

	return boolgeq;

}

EXPRES *make_EXPRES_LEQ(EXPRES *left ,EXPRES *right){

	EXPRES *boolleq = NEW(EXPRES);
	boolleq->lineno = lineno;
	boolleq->kind = EXPRES_LEQ;
	boolleq->value.sides.left = left;
	boolleq->value.sides.right = right;

	return boolleq;

}

EXPRES *make_EXPRES_AND(EXPRES *left ,EXPRES *right){

	EXPRES *booland = NEW(EXPRES);
	booland->lineno = lineno;
	booland->kind = EXPRES_AND;
	booland->value.sides.left = left;
	booland->value.sides.right = right;

	return booland;

}

EXPRES *make_EXPRES_OR(EXPRES *left ,EXPRES *right){

	EXPRES *boolor = NEW(EXPRES);
	boolor->lineno = lineno;
	boolor->kind = EXPRES_OR;
	boolor->value.sides.left = left;
	boolor->value.sides.right = right;

	return boolor;

}


/*TERM constructor */

TERM *make_TERM_VAR(VAR *var){

	TERM *Tvar = NEW(TERM);
	Tvar->lineno = lineno;
	Tvar->kind = TERM_VAR;
	Tvar->value.var = var;

	return Tvar;

}

TERM *make_TERM_ACT_LIST(char *id, ACT_LIST *act_list){

	TERM *actlist = NEW(TERM);
	actlist->lineno = lineno;
	actlist->kind = TERM_ACT_LIST;
	actlist->value.term_act_list.id = id;
	actlist->value.term_act_list.actlist = act_list;

	return actlist;

}

TERM *make_TERM_NOT(TERM *term){

	TERM *bang = NEW(TERM);
	bang->lineno = lineno;
	bang->kind = TERM_NOT;
	bang->value.term = term;

	return bang;

}

TERM *make_TERM_UMINUS(TERM *term){

	TERM *new_term = NEW(TERM);
	new_term->lineno = lineno;
	new_term->kind = TERM_UMINUS;
	new_term->value.term = term;

	return new_term;
}

TERM *make_TERM_PARENTESES(EXPRES *expres){
	TERM *parentese = NEW(TERM);
	parentese->lineno = lineno;
	parentese->kind = TERM_PARENTESES;
	parentese->value.exp = expres;

	return parentese;

}

TERM *make_TERM_ABS(EXPRES *expres){

	TERM *pipe = NEW(TERM);
	pipe->lineno = lineno;
	pipe->kind = TERM_ABS;
	pipe->value.exp = expres;
	
	return pipe;

}

TERM *make_TERM_NULL(){

	TERM *null = NEW(TERM);
	null->lineno = lineno;
	null->kind = TERM_NULL;

	return null;

}
 
TERM *make_TERM_TRUE(){

	TERM *trueT = NEW(TERM);
	trueT->lineno = lineno;
	trueT->kind = TERM_TRUE;

	return trueT;

}

TERM *make_TERM_FALSE(){

	TERM *falseT = NEW(TERM);
	falseT->lineno = lineno;
	falseT->kind = TERM_FALSE;

	return falseT;

}

TERM *make_TERM_NUM(int intconst){

	TERM *num = NEW(TERM);
	num->lineno = lineno;
	num->kind = TERM_NUM;
	num->value.intconst = intconst;

	return num;

}

ACT_LIST *make_ACT_LIST_EXPLIST(EXP_LIST *exp_list){

	ACT_LIST *explist = NEW(ACT_LIST);
	explist->lineno = lineno;
	explist->kind = ACT_LIST_EXPLIST;
	explist->exp_list = exp_list;

	return explist;

}

ACT_LIST *make_ACT_LIST_EMPTY(){
	
	ACT_LIST *epsilon = NEW(ACT_LIST);
	epsilon->lineno = lineno;
	epsilon->kind = ACT_LIST_EMPTY;
	epsilon->exp_list = NULL;

	return epsilon;

}

EXP_LIST *make_EXP_LIST_EXP(EXPRES *expres){

	EXP_LIST *explist = NEW(EXP_LIST);
	explist->lineno = lineno;
	explist->kind = EXP_LIST_EXP;
	explist->exp = expres;
	explist->explist = NULL;
	
	return explist;

}

EXP_LIST *make_EXP_LIST_LIST(EXP_LIST *explist, EXPRES *expres){

	EXP_LIST *commalist = NEW(EXP_LIST);
	commalist->lineno = lineno;
	commalist->kind = EXP_LIST_LIST;
	commalist->explist = explist;
	commalist->exp = expres;

	return commalist;

}

PAR_DECL_LIST *make_PAR_DECL_LIST_LIST(VAR_DECL_LIST *var_declst){

	PAR_DECL_LIST *pdeclst = NEW(PAR_DECL_LIST);
	pdeclst->lineno = lineno;
	pdeclst->kind = PAR_DECL_LIST_LIST;
	pdeclst->var_decl_list = var_declst;
	
	return pdeclst;

}

PAR_DECL_LIST *make_PAR_DECL_LIST_EMPTY(){

	PAR_DECL_LIST *emptylst = NEW(PAR_DECL_LIST);
	emptylst->lineno = lineno;
	emptylst->kind = PAR_DECL_LIST_EMPTY;
	emptylst->var_decl_list = NULL;

	return emptylst;

}

VAR_DECL_LIST *make_VAR_DECL_LIST_LIST(VAR_DECL_LIST *lst, 
							VAR_TYPE *type){

	VAR_DECL_LIST *compound = NEW(VAR_DECL_LIST);
	compound->lineno = lineno;
	compound->kind = VAR_DECL_LIST_LIST;
	compound->var_decl_list = lst;
	compound->var_type = type;

	return compound;

}

VAR_DECL_LIST *make_VAR_DECL_LIST_TYPE(VAR_TYPE *type){

	VAR_DECL_LIST *vartype = NEW(VAR_DECL_LIST);
	vartype->lineno = lineno;
	vartype->kind = VAR_DECL_LIST_TYPE;
	vartype->var_type = type;
	vartype->var_decl_list = NULL;

	return vartype;

}

VAR_TYPE *make_VAR_TYPE( char *id, TYPE *type ){

	VAR_TYPE *var_type = NEW(VAR_TYPE);
	var_type->lineno = lineno;
	var_type->id = calloc(strlen(id)+1,sizeof(char));
	strcpy(var_type->id, id);
	var_type->type = type;
	
	return var_type;

}

VAR *make_VAR_ID(char *id){

	VAR *nid = NEW(VAR);
	nid->lineno = lineno;
	nid->kind = VAR_ID;
	nid->id = calloc(strlen(id)+1,sizeof(char));
	strcpy(nid->id, id);

	return nid;

}

VAR *make_VAR_ARRAY(VAR *var, EXPRES *expres){

	VAR *index = NEW(VAR);
	index->lineno = lineno;
	index->kind = VAR_ARRAY;
	index->value.var_array.var = var;
	index->value.var_array.exp = expres;

	return index;

}

VAR *make_VAR_RECORD(VAR *var, char *id){

	VAR *dot = NEW(VAR);
	dot->lineno = lineno;
	dot->kind = VAR_RECORD;
	dot->value.var_record.id = id;
	dot->value.var_record.var = var;

	return dot;

}

DECL_LIST *make_DECL_LIST_LIST(DECL_LIST *declst, DECLARATION *dec){


	DECL_LIST *compound = NEW(DECL_LIST);
	compound->lineno = lineno;
	compound->kind = DECL_LIST_LIST;
	compound->decl_list = declst;
	compound->declaration = dec;

	return compound;

}

DECL_LIST *make_DECL_LIST_EMPTY(){

	DECL_LIST *empty = NEW(DECL_LIST);
	empty->lineno = lineno;
	empty->kind = DECL_LIST_EMPTY;
	empty->decl_list = NULL;
	empty->declaration = NULL;

	return empty;

}

DECLARATION *make_DECLARATION_ID(char *id, TYPE *type){

	DECLARATION *typeid = NEW(DECLARATION);
	typeid->lineno = lineno;
	typeid->kind = DECLARATION_ID;
	typeid->value.declaration_id.id = id;
	typeid->value.declaration_id.type = type;

	return typeid;

}

DECLARATION *make_DECLARATION_FUNC(FUNC *function){
	
	DECLARATION *func = NEW(DECLARATION);
	func->lineno = lineno;
	func->kind = DECLARATION_FUNC;
	func->value.function = function;

	return func;

}

DECLARATION *make_DECLARATION_VAR(VAR_DECL_LIST *vardeclst){

	DECLARATION *var = NEW(DECLARATION);
	var->lineno = lineno;
	var->kind = DECLARATION_VAR;
	var->value.var_decl_list = vardeclst;

	return var;

}

STATEMENT_LIST *make_STATEMENT_LIST_STATEMENT(STATEMENT *stmt){

	STATEMENT_LIST *stm = NEW(STATEMENT_LIST);
	stm->lineno = lineno;
	stm->kind = STATEMENT_LIST_STATEMENT;
	stm->statement = stmt;
	stm->statement_list = NULL;

	return stm;

}

STATEMENT_LIST *make_STATEMENT_LIST_LIST(STATEMENT_LIST *stmtlst,
										STATEMENT *stmt){

	STATEMENT_LIST *stmlst = NEW(STATEMENT_LIST);
	stmlst->lineno = lineno;
	stmt->currentLoop = NULL;
	stmlst->kind = STATEMENT_LIST_LIST;
	stmlst->statement = stmt;
	stmlst->statement_list = stmtlst;

	return stmlst;

}

STATEMENT *make_STATEMENT_RETURN(EXPRES *expres){

	STATEMENT *streturn = NEW(STATEMENT);
	streturn->lineno = lineno;
	streturn->kind = STATEMENT_RETURN;
	streturn->value.statement_return.exp = expres;
	streturn->foundReturn = 0;
	return streturn;

}

STATEMENT *make_STATEMENT_WRITE(EXPRES *expres){

	STATEMENT *write = NEW(STATEMENT);
	write->lineno = lineno;
	write->kind = STATEMENT_WRITE;
	write->value.exp = expres;

	return write;

}

STATEMENT *make_STATEMENT_ALLOCATE(VAR *var, OPT_LENGTH *oplen){

	STATEMENT *allocate = NEW(STATEMENT);
	allocate->lineno = lineno;
	allocate->kind = STATEMENT_ALLOCATE;
	allocate->value.statement_allocate.var = var;
	allocate->value.statement_allocate.opt_length = oplen;

	return allocate;

}

STATEMENT *make_STATEMENT_ASSIGN(VAR *var,EXPRES *expres){

	STATEMENT *assign = NEW(STATEMENT);
	assign->lineno = lineno;
	assign->kind = STATEMENT_ASSIGN;
	assign->value.statement_assign.var = var;
	assign->value.statement_assign.exp = expres;

	return assign;

}

STATEMENT *make_STATEMENT_IFBRANCH(EXPRES *expres, STATEMENT *stmt ,
			OPT_ELSE *opel){

	STATEMENT *ifbranch = NEW(STATEMENT);
	ifbranch->lineno = lineno;
	ifbranch->kind = STATEMENT_IFBRANCH;
	ifbranch->value.statement_ifbranch.exp = expres;
	ifbranch->value.statement_ifbranch.statement = stmt;
	ifbranch->value.statement_ifbranch.opt_else = opel;

	return ifbranch;

}

STATEMENT *make_STATEMENT_WHILE(EXPRES *expres, STATEMENT *stmt){

	STATEMENT *stwhile = NEW(STATEMENT);
	stwhile->lineno = lineno;
	stwhile->kind = STATEMENT_WHILE;
	stwhile->value.statement_while.exp = expres;
	stwhile->value.statement_while.statement = stmt;
	stwhile->value.statement_while.start_label =
		calloc(50+1,sizeof(char));
	stwhile->value.statement_while.end_label =
		calloc(50+1,sizeof(char));

	return stwhile;

}

STATEMENT *make_STATEMENT_BREAK(){
	STATEMENT *stbreak = NEW(STATEMENT);
	stbreak->lineno = lineno;
	stbreak->kind = STATEMENT_BREAK;

	return stbreak;

}

STATEMENT *make_STATEMENT_CONTINUE(){

	STATEMENT *stcontinue = NEW(STATEMENT);
	stcontinue->lineno = lineno;
	stcontinue->kind = STATEMENT_CONTINUE;

	return stcontinue;

}

STATEMENT *make_STATEMENT_LIST(STATEMENT_LIST *stmlst){

	STATEMENT *compound = NEW(STATEMENT);
	compound->lineno = lineno;
	compound->kind = STATEMENT_LISTS;
	compound->value.statement_list = stmlst;

	return compound;

}

OPT_LENGTH *make_OPT_LENGTH_EXPRES(EXPRES *expres){

	OPT_LENGTH *oflen = NEW(OPT_LENGTH);
	oflen->lineno = lineno;
	oflen->kind = OPT_LENGTH_EXPRES;
	oflen->exp = expres;

	return oflen;

}
	
OPT_LENGTH *make_OPT_LENGTH_EMPTY(){

	OPT_LENGTH *empty = NEW(OPT_LENGTH);
	empty->lineno = lineno;
	empty->kind = OPT_LENGTH_EMPTY;
	empty->exp = NULL;

	return empty;

}

OPT_ELSE *make_OPT_ELSE_STATEMENT(STATEMENT *stmt){

	OPT_ELSE *else_stmt = NEW(OPT_ELSE);
	else_stmt->lineno = lineno;
	else_stmt->kind = OPT_ELSE_STATEMENT;
	else_stmt->statement = stmt;
	
	return else_stmt;

}

OPT_ELSE *make_OPT_ELSE_EMPTY(){
	
	OPT_ELSE *empty = NEW(OPT_ELSE);
	empty->lineno = lineno;
	empty->kind = OPT_ELSE_EMPTY;
	empty->statement = NULL;

	return empty;

}

TYPE *make_TYPE_ID(char *id){

	TYPE *tid = NEW(TYPE);
	tid->lineno = lineno;
	tid->kind = TYPE_ID;
	tid->value.id = id;

	return tid;

}

TYPE *make_TYPE_INT(){
	
	TYPE *int_type = NEW(TYPE);
	int_type->lineno = lineno;
	int_type->kind = TYPE_INT;

	return int_type;

}

TYPE *make_TYPE_BOOL(){
	
	TYPE *bool_type = NEW(TYPE);
	bool_type->lineno = lineno;
	bool_type->kind = TYPE_BOOL;

	return bool_type;

}

TYPE *make_TYPE_ARRAY(TYPE *type){

	TYPE *array = NEW(TYPE);
	array->lineno = lineno;
	array->kind = TYPE_ARRAY;
	array->value.type = type;

	return array;

}

TYPE *make_TYPE_RECORD(VAR_DECL_LIST *vardeclst){

	TYPE *recordof = NEW(TYPE);
	recordof->lineno = lineno;
	recordof->kind = TYPE_RECORD;
	recordof->value.var_decl_list = vardeclst;

	return recordof;

}
