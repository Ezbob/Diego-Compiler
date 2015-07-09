/*  
 * Based on the implementation of Robert I. Pitts
 * ----------------------------------------------------------------
 */

#ifndef _stackLOOP_H
#define _stackLOOP_H
#include "../parserscanner/kittytree.h"
#include "../symbol/symbol.h"

typedef struct stackElementt {
	 STATEMENT *function;
	 struct stackElementt *next;
} stackElementt;


typedef struct stackL {
  struct stackElementt *top;
} stackL;


stackL *loopStackInit();

STATEMENT *loopStackPeep(stackL *stackP);

void loopStackDestroy(stackL *stackP);

void loopStackPush(stackL *stackP, STATEMENT *element);
STATEMENT *loopStackPop(stackL *stackP);


#endif  /* not defined _stack_H */

