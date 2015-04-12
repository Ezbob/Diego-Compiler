#ifndef IR_INSTRUCTIONS_H
#define IR_INSTRUCTIONS_H

#include "kittyir.h"

IR_INSTRUCTION *make_instruction_movl();
IR_INSTRUCTION *make_instruction_call();
IR_INSTRUCTION *make_instruction_pushl();
IR_INSTRUCTION *make_instruction_popl();
IR_INSTRUCTION *make_instruction_addl();
IR_INSTRUCTION *make_instruction_subl();

ARGUMENT *make_argument_address();
ARGUMENT *make_argument_register();
ARGUMENT *make_argument_label();
ARGUMENT *make_argument_constant();

TEMP *make_temp_register();
TEMP *make_temp_spilled();
TEMP *make_temp_virtual();

#endif