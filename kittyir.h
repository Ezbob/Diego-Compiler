#ifndef KITTY_EMITTER_H
#define KITTY_EMITTER_H 
#include "dlinkedlist.h"


typedef struct IR_INSTRUCTION {
	
	enum { mov, call, push, pop } op_code;
	ARGUMENT arg1;
	ARGUMENT arg2;

} IR_INSTRUCTION;

typedef struct ARGUMENT {
	enum { address_arg, register_arg, label_arg, constant_arg } kind;
	union {
		REGISTER reg;
		int intConst;
	}value;
} ARGUMENT;

typedef struct TEMP {
	int id;
	enum { register_temp, virtual_temp, spilled_temp } kind;
	union {
		enum { eax, ebx, ecx, edx, ebp, esi, edi, esp } registerName;
		void *address;
	}
} TEMP;

/*
 * stores all IR code in a linked_list
 */
linked_list *begin_ir_emit();


#endif