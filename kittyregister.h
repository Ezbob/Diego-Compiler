#ifndef __KITTYREGISTER_H
#define __KITTYREGISTER_H
#include "kittyir.h"

typedef unsigned long BITARRAY;

typedef struct LIVENESS {

	int color;
	BITARRAY *use;
	BITARRAY *def;
	BITARRAY *in;
	BITARRAY *out;
	int id;
	struct LIVENESS *next;
	struct LIVENESS *prev;
	int number_successors;
	struct LIVENESS **successors;

} LIVENESS;

typedef struct SUCLIST {
	struct LIVENESS *element;
	char *labelId;
	struct SUCLIST *next;
} SUCLIST;


void begin_register(linked_list *ir_lines);
void liveness_analysis();
void analyseUseDef();
int ifTempAdd(ARGUMENT *arg);
void getSuccessors(LIVENESS *line, IR_INSTRUCTION *instr);

#endif