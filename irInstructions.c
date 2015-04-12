#include "irInstructions.h"
#include <stdlib.h>

IR_INSTRUCTION *make_instruction_movl(){
	IR_INSTRUCTION *new_instruction;
	new_instruction = malloc(sizeof(IR_INSTRUCTION *));
	new_instruction->op_code = movl;
	new_instruction->arg1 = NULL;
	new_instruction->arg2 = NULL;
	return new_instruction;
}

IR_INSTRUCTION *make_instruction_call(){
	IR_INSTRUCTION *new_instruction;
	new_instruction = malloc(sizeof(IR_INSTRUCTION *));
	new_instruction->op_code = call;
	new_instruction->arg1 = NULL;
	new_instruction->arg2 = NULL;
	return new_instruction;
}

IR_INSTRUCTION *make_instruction_pushl(){
	IR_INSTRUCTION *new_instruction;
	new_instruction = malloc(sizeof(IR_INSTRUCTION *));
	new_instruction->op_code = pushl;
	new_instruction->arg1 = NULL;
	new_instruction->arg2 = NULL;
	return new_instruction;
}

IR_INSTRUCTION *make_instruction_popl(){
	IR_INSTRUCTION *new_instruction;
	new_instruction = malloc(sizeof(IR_INSTRUCTION *));
	new_instruction->op_code = popl;
	new_instruction->arg1 = NULL;
	new_instruction->arg2 = NULL;
	return new_instruction;
}

IR_INSTRUCTION *make_instruction_addl(){
	IR_INSTRUCTION *new_instruction;
	new_instruction = malloc(sizeof(IR_INSTRUCTION *));
	new_instruction->op_code = addl;
	new_instruction->arg1 = NULL;
	new_instruction->arg2 = NULL;
	return new_instruction;
}

IR_INSTRUCTION *make_instruction_subl(){
	IR_INSTRUCTION *new_instruction;
	new_instruction = malloc(sizeof(IR_INSTRUCTION *));
	new_instruction->op_code = subl;
	new_instruction->arg1 = NULL;
	new_instruction->arg2 = NULL;
	return new_instruction;
}

ARGUMENT *make_argument_address(){
	ARGUMENT *new_argument;
	new_argument = malloc(sizeof(ARGUMENT *));
	new_argument->value = NULL;
	new_argument->kind = address_arg;
	return new_argument;
}

ARGUMENT *make_argument_register(){
	ARGUMENT *new_argument;
	new_argument = malloc(sizeof(ARGUMENT *));
	new_argument->value = NULL;
	new_argument->kind = register_arg;
	return new_argument;
}

ARGUMENT *make_argument_label(){
	ARGUMENT *new_argument;
	new_argument = malloc(sizeof(ARGUMENT *));
	new_argument->value = NULL;
	new_argument->kind = label_arg;
	return new_argument;
}

ARGUMENT *make_argument_constant(){
	ARGUMENT *new_argument;
	new_argument = malloc(sizeof(ARGUMENT *));
	new_argument->value = NULL;
	new_argument->kind = constant_arg;
	return new_argument;
}

TEMP *make_temp_register(int id){
	TEMP *new_temp;
	new_temp = malloc(sizeof(TEMP*));
	new_temp->id = id;
	new_temp->kind = register_temp;
	new_temp->value = NULL;
	return new_temp;	
}

TEMP *make_temp_spilled(int id){
	TEMP *new_temp;
	new_temp = malloc(sizeof(TEMP*));
	new_temp->id = id;
	new_temp->kind = spilled_temp;
	new_temp->value = NULL;
	return new_temp;
}

TEMP *make_temp_virtual(int id){
	TEMP *new_temp;
	new_temp = malloc(sizeof(TEMP*));
	new_temp->id = id;
	new_temp->kind = virtual_temp;
	new_temp->value = NULL;
	return new_temp;
}
