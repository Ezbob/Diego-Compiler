#ifndef __KITTYREGISTER_H
#define __KITTYREGISTER_H
#include "kittyir.h"

typedef struct LIVENESS {

	int color;
	int use;
	int def;
	int in;
	int out;
	int id;
	struct LIVENESS *next;
	struct LIVENESS *prev;
	int number_successors;
	struct LIVENESS **successors;

} LIVENESS;


void begin_register(linked_list *ir_lines);
void liveness_analysis();
void analyseUseDef();
int ifTempAdd(ARGUMENT *arg);

#endif