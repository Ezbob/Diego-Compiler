#include <stdio.h>
#include "kittyprinter.h"
#include "../parserscanner/kittytree.h"

static int indentation = 0;
static int tabSize = 4;

/*Line shift with the current indentations level */
void lineShift(){

	int i;
	for (i = (indentation * tabSize); i > 0; i--){
		printf("%s", " ");
	}

}

void printer_function(FUNC *func) {
	printf("Func ");
	printer_head(func->head);
	printf("\n");
	indentation++;
	printer_body(func->body);
	indentation--;
	printer_tail(func->tail);
	printf("\n");
}

void printer_head (HEAD *header){
	printf("%s( ", header->id);
	printer_par_decl_list(header->pdlist);
	printf(") :");
	printer_type(header->returntype);
}

void printer_body (BODY *body){
	printer_decl_list(body->decl_list);
	printer_statement_list(body->statement_list);
	
}

void printer_tail (TAIL *tail){
	printf("end %s", tail->id);
}


void printer_type ( TYPE *type){
	switch(type->kind){
		case TYPE_ID:
			printf(" %s", type->value.id);
			break;
		case TYPE_INT:
			printf(" int");
			break;
		case TYPE_BOOL:
			printf(" bool");
			break;
		case TYPE_ARRAY:
			printf(" array of");
			printer_type(type->value.type);
			break;
		case TYPE_RECORD:
			printf(" record of { ");
			printer_var_decl_list(type->value.var_decl_list);
			printf(" };");
			break;
	}
}
	
void printer_par_decl_list ( PAR_DECL_LIST *pdecl){
	switch(pdecl->kind){
		case PAR_DECL_LIST_LIST:
			printer_var_decl_list(pdecl->var_decl_list);
			break;
		case PAR_DECL_LIST_EMPTY:
			break;

	}
}

void printer_var_decl_list ( VAR_DECL_LIST *vdecl){
	switch(vdecl->kind){
		case VAR_DECL_LIST_LIST:
			printer_var_decl_list(vdecl->var_decl_list);
			printf(", ");
			printer_var_type(vdecl->var_type);
			break;
		case VAR_DECL_LIST_TYPE:
			printer_var_type(vdecl->var_type);
			break;
	}
}

void printer_var_type ( VAR_TYPE *vtype){
	printf("%s :", vtype->id);
	printer_type(vtype->type);
}

void printer_decl_list ( DECL_LIST *dlst){
	switch(dlst->kind){
		case DECL_LIST_LIST:
			printer_decl_list(dlst->decl_list);
			printer_declaration(dlst->declaration);
			printf("\n");
			break;
		case DECL_LIST_EMPTY:
			break;
	}
}


void printer_declaration ( DECLARATION *decl){
	switch(decl->kind){
		case DECLARATION_ID:
			printf("Type %s =", decl->value.declaration_id.id);
			printer_type(decl->value.declaration_id.type);
			break;
		case DECLARATION_FUNC:
			printer_function(decl->value.function);
			break;
		case DECLARATION_VAR:
			printf("Var ");
			printer_var_decl_list(decl->value.var_decl_list);
			printf(";");
			break;
	}
}

void printer_statement_list ( STATEMENT_LIST *slst){
	switch(slst->kind){
		case STATEMENT_LIST_STATEMENT:
			printer_statement(slst->statement);
			break;
		case STATEMENT_LIST_LIST: 
			printer_statement_list(slst->statement_list);
			printer_statement(slst->statement);
			break;
	}
}

void printer_statement ( STATEMENT *st){
	lineShift();
	switch(st->kind){
		case STATEMENT_RETURN:
			printf("return ");
			printer_expression(st->value.statement_return.exp);
			printf(";");
			break;
		case STATEMENT_WRITE:
			printf("write ");
			printer_expression(st->value.exp);
			printf(";");
			break;
		case STATEMENT_ALLOCATE:
			printf("allocate ");
			printer_variable(st->value.statement_allocate.var);
			printer_opt_length(st->value.statement_allocate.opt_length);
			printf(";");
			break;
		case STATEMENT_ASSIGN:
			printer_variable(st->value.statement_assign.var);
			printf(" := ");
			printer_expression(st->value.statement_assign.exp);
			printf(";");
			break;
		case STATEMENT_ADDASSIGN:
			printer_variable(st->value.statement_assign.var);
			printf(" += ");
			printer_expression(st->value.statement_assign.exp);
			printf(";");
			break;
		case STATEMENT_SUBASSIGN:
			printer_variable(st->value.statement_assign.var);
			printf(" -= ");
			printer_expression(st->value.statement_assign.exp);
			printf(";");
			break;
		case STATEMENT_MULASSIGN:
			printer_variable(st->value.statement_assign.var);
			printf(" *= ");
			printer_expression(st->value.statement_assign.exp);
			printf(";");
			break;
		case STATEMENT_DIVASSIGN:
			printer_variable(st->value.statement_assign.var);
			printf(" /= ");
			printer_expression(st->value.statement_assign.exp);
			printf(";");
			break;
		case STATEMENT_MODASSIGN:
			printer_variable(st->value.statement_assign.var);
			printf(" %%= ");
			printer_expression(st->value.statement_assign.exp);
			printf(";");
			break;
		case STATEMENT_IFBRANCH:
			printf("If ");
			printer_expression(st->value.statement_if_branch.condition);
			printf(" then\n");
			indentation++;
			printer_statement(st->value.statement_if_branch.statement);
			indentation--;
			printer_opt_else(st->value.statement_if_branch.opt_else);
			break;
		case STATEMENT_WHILE:
			printf("while ");
			printer_expression(st->value.statement_while.condition);
			printf(" ");
			printer_statement(st->value.statement_while.statement);
			break;
		case STATEMENT_FOR:
			printf("for ");
			printer_statement(st->value.statement_for.assignment);
			printf(" ");
			printer_expression(st->value.statement_for.condition);
			printf("; ");
			printer_statement(st->value.statement_for.update);
			printf(" do \n");
			indentation++;
			printer_statement(st->value.statement_for.statement);
			indentation--;
			break;
		case STATEMENT_FOREACH:
			printf("for each ");
			printer_variable(st->value.statement_foreach.element);
			printf(" in ");
			printer_variable(st->value.statement_foreach.collection);
			printf(" do\n");
			indentation++;
			printer_statement(st->value.statement_foreach.statement);
			indentation--;
		case STATEMENT_LISTS:
			printf("{\n");
			indentation++;
			printer_statement_list(st->value.statement_list);
			indentation--;
			printf("}");
			break;
		case STATEMENT_BREAK:
			printf("break;");
			break;
		case STATEMENT_CONTINUE:
			printf("continue;");
	}
	printf("\n");
}

void printer_opt_length ( OPT_LENGTH *oplen){
	switch(oplen->kind){
		case OPT_LENGTH_EXPRES:
			printf(" length of ");
			printer_expression(oplen->exp);
			break;
		case OPT_LENGTH_EMPTY:
			break;
	}
}

void printer_opt_else ( OPT_ELSE *opel){
	switch(opel->kind){
		case OPT_ELSE_STATEMENT:
			lineShift();
			printf("else:\n");
			indentation++;
			printer_statement(opel->statement);
			indentation--;
			break;
		case OPT_ELSE_EMPTY:
			break;
	}
}

void printer_variable ( VAR *var){
	switch(var->kind){
		case VAR_ID:
			printf("%s",var->id);
			break;
		case VAR_ARRAY:
			printer_variable(var->value.var_array.var);
			printf("[");
			printer_expression(var->value.var_array.exp);
			printf("]");
			break;
		case VAR_RECORD:
			printer_variable(var->value.var_record.var);
			printf(".%s",var->value.var_record.id );
			break;
	}
}

void printer_expression ( EXPRES *exp){
	printf("(, ");
	decide_type(exp->symbolType, NULL);
	printf(": ");
	switch(exp->kind){
		case EXPRES_TERM:
			printer_term(exp->value.term);
			break;
		case EXPRES_PLUS:
			printer_expression(exp->value.sides.left);
			printf(" + ");
			printer_expression(exp->value.sides.right);
			break;
		case EXPRES_MINUS:
			printer_expression(exp->value.sides.left);
			printf(" - ");
			printer_expression(exp->value.sides.right);
			break;
		case EXPRES_TIMES:
			printer_expression(exp->value.sides.left);
			printf(" * ");
			printer_expression(exp->value.sides.right);
			break;
		case EXPRES_DIVIDE:
			printer_expression(exp->value.sides.left);
			printf(" / ");
			printer_expression(exp->value.sides.right);
			break;
		case EXPRES_MODULO:
			printer_expression(exp->value.sides.left);
			printf(" %% ");
			printer_expression(exp->value.sides.right);
			break;
		case EXPRES_EQ:
			printer_expression(exp->value.sides.left);
			printf(" == ");
			printer_expression(exp->value.sides.right);
			break;
		case EXPRES_NEQ:
			printer_expression(exp->value.sides.left);
			printf(" != ");
			printer_expression(exp->value.sides.right);
			break;
		case EXPRES_GREATER:
			printer_expression(exp->value.sides.left);
			printf(" > ");
			printer_expression(exp->value.sides.right);
			break;
		case EXPRES_LESS:
			printer_expression(exp->value.sides.left);
			printf(" < ");
			printer_expression(exp->value.sides.right);
			break;
		case EXPRES_LEQ:
			printer_expression(exp->value.sides.left);
			printf(" <= ");
			printer_expression(exp->value.sides.right);
			break;
		case EXPRES_GEQ:
			printer_expression(exp->value.sides.left);
			printf(" >= ");
			printer_expression(exp->value.sides.right);
			break;
		case EXPRES_AND:
			printer_expression(exp->value.sides.left);
			printf(" && ");
			printer_expression(exp->value.sides.right);
			break;
		case EXPRES_OR:
			printer_expression(exp->value.sides.left);
			printf(" || ");
			printer_expression(exp->value.sides.right);
			break;
	}
	printf(" ,)");
}

void printer_term ( TERM *term){
	switch(term->kind){
		case TERM_VAR:
			printer_variable(term->value.var);
			break;
		case TERM_ACT_LIST:
			printf("%s(", term->value.term_act_list.id);
			printer_act_list(term->value.term_act_list.actlist);
			printf(")");
			break;
		case TERM_NOT:
			printf("!");
			printer_term(term->value.term);
			break;
		case TERM_UMINUS:
			printf("-(");
			printer_term(term->value.term);
			printf(")");
			break;
		case TERM_PARENTESES:
			printf("(");
			printer_expression(term->value.exp);
			printf(")");
			break;
		case TERM_ABS:
			printf("|");
			printer_expression(term->value.exp);
			printf("|");
			break;
		case TERM_NULL:
			printf("NULL");
			break;
		case TERM_TRUE:
			printf("TRUE");
			break;
		case TERM_FALSE:
			printf("FALSE");
			break;
		case TERM_NUM:
			printf("%i",term->value.intconst);
			break;
	}
}

void printer_act_list ( ACT_LIST *actlst){
	switch(actlst->kind){
		case ACT_LIST_EXPLIST:
			printer_expression_list(actlst->exp_list);
			break;
		case ACT_LIST_EMPTY:
			break;
	}
}

void printer_expression_list ( EXP_LIST *explst){
	switch(explst->kind){
		case EXP_LIST_EXP:
			printer_expression(explst->exp);
			break;
		case EXP_LIST_LIST:
			printer_expression_list(explst->explist);
			printf(", ");
			printer_expression(explst->exp);
			break;
	}
}

void decide_type (SYMBOL_TYPE *stype, char *id_of_type) {
	switch (stype->type) {
		case SYMBOL_RECORD:
			printf("record");
			break;
		case SYMBOL_INT:
			printf("int");
			break;
		case SYMBOL_ARRAY:
			printf("array");
			break;
		case SYMBOL_FUNCTION:
			printf("function");
			break;
		case SYMBOL_ID:
			if(id_of_type != NULL) {
				printf("%s",id_of_type);
			} else {
				printf("id");
			}
			break;
		case SYMBOL_BOOL:
			printf("bool");
			break;
		case SYMBOL_NULL:
			printf("null");
			break;
		case SYMBOL_UNKNOWN:
			printf("unknown");
			break;
	}

}