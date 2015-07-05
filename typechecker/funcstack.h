/*  
 * Based on the implementation of Robert I. Pitts
 * ----------------------------------------------------------------
 */

#ifndef _stack_H
#define _stack_H
#include "../parserscanner/kittytree.h"
#include "../symbol/symbol.h"

typedef struct stackElement {
	 FUNC *function;
	 struct stackElement *next;
} stackElement;


typedef struct stackT {
  struct stackElement *top;
} stackT;


stackT *funcStackInit();

FUNC *funcStackPeep(stackT *stackP);

void funcStackDestroy(stackT *stackP);

void funcStackPush(stackT *stackP, FUNC *element);
FUNC *funcStackPop(stackT *stackP);


int StackIsEmpty(stackT *stackP);
int StackIsFull(stackT *stackP);

#endif  /* not defined _stack_H */
