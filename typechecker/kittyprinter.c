#include <stdio.h>
#include <stdarg.h> 
#include "kittyprinter.h"
 
static int tab_indentation = -1;

#define NEXT_INDENT_LEVEL (tab_indentation++)
#define PREV_INDENT_LEVEL (tab_indentation--)

/*
 * indentation printf
 * prints the string with indentation according to tab_indentation
 */
void iprintf(const char* format, ...){
	va_list args; // placeholder for ... arguments

	va_start(args,format); // what is the first argument?
	INDENT(tab_indentation); // indent according to tab_indentation
	vprintf(format,args); // prints to stdout from args list
	va_end(args); // end of argument list
}

void printer_function(FUNC *func) {
	iprintf("Func");
	printer_head(func->functionF.head);
	printer_body(func->functionF.body);
	printer_tail(func->functionF.tail);
}

void printer_head (HEAD *header){
	printf(" %s ( ", header->headH.id);
	printer_par_decl_list(header->headH.pdeclist);
	printf(") :");
	printer_type(header->headH.returntype);
	printf("\n");
}

void printer_body (BODY *body){
	NEXT_INDENT_LEVEL;
	printer_decl_list(body->decl_list);
	printer_statement_list(body->statement_list);
	PREV_INDENT_LEVEL;
}

void printer_tail (TAIL *tail){
	iprintf("End %s \n", tail->id);
}


void printer_type ( TYPE *type){
	switch(type->kind){
		case id_TY_K:
			printf(" %s", type->value.idconst);
			break;
		case int_TY_K:
			printf(" int");
			break;
		case bool_TY_K:
			printf(" bool");
			break;
		case arrayof_TY_K:
			printf(" array of");
			printer_type(type->value.type);
			break;
		case recordof_TY_K:
			printf(" record of {");
			printer_var_decl_list(type->value.var_decl_list);
			printf("};\n");
			break;
	}
}
	
void printer_par_decl_list ( PAR_DECL_LIST *pdecl){
	switch(pdecl->kind){
		case varType_PDL_K:
			printer_var_decl_list(pdecl->value.var_decl_list);
			break;
		case empty_PDL_K:
			break;

	}
}

void printer_var_decl_list ( VAR_DECL_LIST *vdecl){
	switch(vdecl->kind){
		case comma_VDL_K:
			printer_var_decl_list(vdecl->value.commaVDL.var_decl_list);
			printf(" , ");
			printer_var_type(vdecl->value.commaVDL.var_type);
			break;
		case var_VDL_typeK:
			printer_var_type(vdecl->value.var_type);
			break;
	}
}

void printer_var_type ( VAR_TYPE *vtype){
	printf("%s :", vtype->id);
	printer_type(vtype->type);
}

void printer_decl_list ( DECL_LIST *dlst){
	switch(dlst->kind){
		case compound_DL_K:
			printer_decl_list(dlst->value.compoundDL.decl_list);		
			printer_declaration(dlst->value.compoundDL.declaration);
			break;
		case empty_DL_K:
			break;
	}
}


void printer_declaration ( DECLARATION *decl){
	NEXT_INDENT_LEVEL;
	switch(decl->kind){
		case typeassign_D_K:
			iprintf("Type %s = ", decl->value.typedeclID.id);
			printer_type(decl->value.typedeclID.type);
			break;
		case func_D_K:
			printer_function(decl->value.function);
			break;
		case var_D_K:
			iprintf("Var ");
			printer_var_decl_list(decl->value.var_decl_list);
			printf(";\n");
			break;
	}
	PREV_INDENT_LEVEL;
}

void printer_statement_list ( STATEMENT_LIST *slst){
	switch(slst->kind){
		case statement_SL_K:
			printer_statement(slst->value.statement);
			break;
		case compound_SL_K: 
			printer_statement_list(slst->value.compoundSL.statement_list);
			printer_statement(slst->value.compoundSL.statement);
			break;
	}
}

void printer_statement ( STATEMENT *st){
	NEXT_INDENT_LEVEL;
	switch(st->kind){
		case return_S_K:
			iprintf("Return ");
			printer_expression(st->value.returnS.exp);
			printf(";\n"); 	
			break;
		case print_S_K:
			iprintf("Write ");
			printer_expression(st->value.exp);
			printf(";\n");
			break;
		case allocate_S_K:
			iprintf("Allocate ");
			printer_variable(st->value.allocateS.variable);
			printer_opt_length(st->value.allocateS.opt_length);
			printf(";\n");
			break;
		case assign_S_K:
			INDENT(tab_indentation);
			printer_variable(st->value.assignS.variable);
			printf(" := ");
			printer_expression(st->value.assignS.exp);
			printf(";\n");
			break;
		case ifbranch_S_K:
			iprintf("If ");
			printer_expression(st->value.ifbranchS.exp);
			printf(" then\n");
			printer_statement(st->value.ifbranchS.statement);
			printer_opt_else(st->value.ifbranchS.opt_else);
			printf("\n");
			break;
		case while_S_K:
			iprintf("While ");
			printer_expression(st->value.whileS.exp);
			printer_statement(st->value.whileS.statement);
			printf("\n");
			break;
		case statement_list_S_K:
			iprintf("{\n");
			printer_statement_list(st->value.statement_list);
			iprintf("}");
			break; 
	}
	PREV_INDENT_LEVEL;
} 

void printer_opt_length ( OPT_LENGTH *oplen){
	switch(oplen->kind){
		case lengthof_OL_K:
			printf(" length of ");
			printer_expression(oplen->value.exp);
			break;
		case empty_OL_K:
			break;
	}
}

void printer_opt_else ( OPT_ELSE *opel){
	switch(opel->kind){
		case else_OE_K:
			iprintf(" else: \n");
			printer_statement(opel->value.statement);
			break;
		case empty_OE_K:
			break;
	}
}

void printer_variable ( VAR *var){
	switch(var->kind){
		case id_V_K:
			printf("%s",var->value.id);
			break;
		case indexing_V_K:
			printer_variable(var->value.indexingV.variable);
			printf("[ ");
			printer_expression(var->value.indexingV.exp);
			printf(" ] ");
			break;
		case dot_V_K:
			printer_variable(var->value.dotV.variable);
			printf(".%s",var->value.dotV.id );
			break;
	}
}

void printer_expression ( EXPRES *exp){
	switch(exp->kind){
		case term_E_K:
			printer_term(exp->value.term);
			break;
		case plus_E_K:
			printf("(");
			printer_expression(exp->value.sides.left);
			printf(" + ");
			printer_expression(exp->value.sides.right);
			printf(")");
			break;
		case minus_E_K:
			printf("(");
			printer_expression(exp->value.sides.left);
			printf(" - ");
			printer_expression(exp->value.sides.right);
			printf(")");
			break;
		case times_E_K:
			printf("(");
			printer_expression(exp->value.sides.left);
			printf(" * ");
			printer_expression(exp->value.sides.right);
			printf(")");
			break;
		case divide_E_K:
			printf("(");
			printer_expression(exp->value.sides.left);
			printf(" / ");
			printer_expression(exp->value.sides.right);
			printf(")");
			break;
		case booleq_E_K:
			printf("(");
			printer_expression(exp->value.sides.left);
			printf(" == ");
			printer_expression(exp->value.sides.right);
			printf(")");
			break;
		case boolneq_E_K:
			printf("(");
			printer_expression(exp->value.sides.left);
			printf(" != ");
			printer_expression(exp->value.sides.right);
			printf(")");
			break;
		case boolgreater_E_K:
			printf("(");
			printer_expression(exp->value.sides.left);
			printf(" > ");
			printer_expression(exp->value.sides.right);
			printf(")");
			break;
		case boolless_E_K:
			printf("(");
			printer_expression(exp->value.sides.left);
			printf(" < ");
			printer_expression(exp->value.sides.right);
			printf(")");
			break;
		case boolleq_E_K:
			printf("(");
			printer_expression(exp->value.sides.left);
			printf(" <= ");
			printer_expression(exp->value.sides.right);
			printf(")");
			break;
		case boolgeq_E_K:
			printf("(");
			printer_expression(exp->value.sides.left);
			printf(" >= ");
			printer_expression(exp->value.sides.right);
			printf(")");
			break;
		case booland_E_K:
			printf("(");
			printer_expression(exp->value.sides.left);
			printf(" && ");
			printer_expression(exp->value.sides.right);
			printf(")");
			break;
		case boolor_E_K:
			printf("(");
			printer_expression(exp->value.sides.left);
			printf(" || ");
			printer_expression(exp->value.sides.right);
			printf(")");
			break;
	}

}

void printer_term ( TERM *term){
	switch(term->kind){
		case var_T_K:
			printer_variable(term->value.var);
			break;
		case actList_T_K:
			printf("%s (", term->value.actlistT.id);
			printer_act_list(term->value.actlistT.actlist);
			printf(")");
			break;
		case termBang_T_K:
			printf("!");
			printer_term(term->value.term);
			break;
		case expresParent_T_K:
			printf("(");
			printer_expression(term->value.exp);
			printf(")");
			break;
		case expresPipes_T_K:
			printf("|");
			printer_expression(term->value.exp);
			printf("|");
			break;
		case null_T_K:
			printf("%s", term->value.nullval);
			break;
		case boolTrue_T_K:
			printf("TRUE");
			break;
		case boolFalse_T_K:
			printf("FALSE");
			break;
		case num_T_K:
			printf("%i",term->value.intconst);
			break;
	}
}

void printer_act_list ( ACT_LIST *actlst){
	switch(actlst->kind){
		case explist_AL_K:
			printer_expression_list(actlst->value.exp_list);
			break;
		case empty_AL_K:
			break;
	}
}

void printer_expression_list ( EXP_LIST *explst){
	switch(explst->kind){
		case exp_EL_K:
			printer_expression(explst->value.exp);
			break;
		case commalist_EL_K:
			printer_expression_list(explst->value.commaEL.explist);
			printf(",");
			printer_expression(explst->value.commaEL.exp);
			break;
	}
}
