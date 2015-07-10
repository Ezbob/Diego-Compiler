/*  
 * Based on the implementation of Robert I. Pitts
 * ----------------------------------------------------------------
 */

#ifndef _stack_H
#define _stack_H
#include "../parserscanner/kittytree.h"
#include "../symbol/symbol.h"

typedef struct stackElement {
	 void *data;
	 struct stackElement *next;
} stackElement;


typedef struct stackT {
  struct stackElement *top;
} stackT;

int stackIsEmpty(stackT *stackP);
void stackDestroy(stackT *stackP);
stackT *stackInit();


// functions

FUNC *funcStackPeek(stackT *stackP);
void funcStackPush(stackT *stackP, FUNC *element);
FUNC *funcStackPop(stackT *stackP);

// loop statements

STATEMENT *loopStackPeek(stackT *stackP);
void loopStackPush(stackT *stackP, STATEMENT *element);
STATEMENT *loopStackPop(stackT *stackP);

#endif  /* not defined _stack_H */

