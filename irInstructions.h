#ifndef IR_INSTRUCTIONS_H
#define IR_INSTRUCTIONS_H

#include "kittyir.h"

IR_LINE *make_line_empty();
IR_LINE *make_line_instruction(IR_INSTRUCTION *,char *);
IR_LINE *make_line_label(char *);

IR_INSTRUCTION *make_instruction_movl();
IR_INSTRUCTION *make_instruction_call();
IR_INSTRUCTION *make_instruction_pushl();
IR_INSTRUCTION *make_instruction_popl();
IR_INSTRUCTION *make_instruction_addl();
IR_INSTRUCTION *make_instruction_subl();
IR_INSTRUCTION *make_instruction_globl();
IR_INSTRUCTION *make_instruction_string();
IR_INSTRUCTION *make_instruction_ret();

ARGUMENT *make_argument_address();
ARGUMENT *make_argument_register();
ARGUMENT *make_argument_label(char *);
ARGUMENT *make_argument_constant();

TEMP *make_temp_register(int);
TEMP *make_temp_spilled(int);
TEMP *make_temp_virtual(int);

#endif