#ifndef IR_INSTRUCTIONS_H
#define IR_INSTRUCTIONS_H

#include "kittyir.h"

IR_INSTRUCTION *make_instruction_movl(ARGUMENT *arg1, ARGUMENT *arg2);
IR_INSTRUCTION *make_instruction_cmp(ARGUMENT *arg1, ARGUMENT *arg2);
IR_INSTRUCTION *make_instruction_call(ARGUMENT *arg1);
IR_INSTRUCTION *make_instruction_pushl(ARGUMENT *arg1);
IR_INSTRUCTION *make_instruction_notl(ARGUMENT *arg1);
IR_INSTRUCTION *make_instruction_negl(ARGUMENT *arg1);
IR_INSTRUCTION *make_instruction_intcode(char *c);
IR_INSTRUCTION *make_instruction_jmp(char *);
IR_INSTRUCTION *make_instruction_jne(char *);
IR_INSTRUCTION *make_instruction_je(char *);
IR_INSTRUCTION *make_instruction_jg(char *);
IR_INSTRUCTION *make_instruction_jl(char *);
IR_INSTRUCTION *make_instruction_JGE(char *);
IR_INSTRUCTION *make_instruction_JLE(char *);
IR_INSTRUCTION *make_instruction_incl(ARGUMENT *arg1);
IR_INSTRUCTION *make_instruction_decl(ARGUMENT *arg1);
IR_INSTRUCTION *make_instruction_popl(ARGUMENT *arg1);
IR_INSTRUCTION *make_instruction_addl(ARGUMENT *arg1, ARGUMENT *arg2);
IR_INSTRUCTION *make_instruction_subl(ARGUMENT *arg1, ARGUMENT *arg2);
IR_INSTRUCTION *make_instruction_xor(ARGUMENT *arg1, ARGUMENT *arg2);
IR_INSTRUCTION *make_instruction_div(ARGUMENT *arg1);
IR_INSTRUCTION *make_instruction_imul(ARGUMENT *arg1, ARGUMENT *arg2);
IR_INSTRUCTION *make_instruction_label(char *);
IR_INSTRUCTION *make_instruction_directive(char *);
IR_INSTRUCTION *make_instruction_space(ARGUMENT *arg1,ARGUMENT *arg2);
IR_INSTRUCTION *make_instruction_leal(ARGUMENT *,ARGUMENT *);
IR_INSTRUCTION *make_instruction_ret();


ARGUMENT *make_argument_address(int i);
ARGUMENT *make_argument_register(REGISTERS reg, char *name);
ARGUMENT *make_argument_label(char *);
ARGUMENT *make_argument_constant(int i);
ARGUMENT *make_argument_temp(int i);
ARGUMENT *make_argument_tempregister(int i);
ARGUMENT *make_argument_indexing(ARGUMENT *, ARGUMENT *);
ARGUMENT *make_argument_static(int i);

#endif