/*  
 * Based on the implementation of Robert I. Pitts
 * ----------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>  /* for dynamic allocation */
#include "loopstack.h"


/************************ Function Definitions **********************/

stackL *loopStackInit()
{

  /* Allocate a new array to hold the contents. */

  stackL *the_stack = NEW(stackL);

  if (the_stack == NULL) {
    fprintf(stderr, "Insufficient memory to initialize stack.\n");
    return NULL;  /* Exit, returning error code. */
  }

  the_stack->top = NULL;  /* I.e., empty */

  return the_stack;
}

void loopStackDestroy(stackL *stackP)
{
  stackL *stack = stackP;
  stack->top = NULL;  /* I.e., empty */
  /* Get rid of array. */
  free(stack);
}

void loopStackPush(stackL *stackP, STATEMENT *element)
{

  /* Put information in array; update top. */
  stackElementt *newElement = NEW(stackElementt);
  if(newElement == NULL){
	 fprintf(stderr, "Failed to allocate for new element");
  }
  newElement->function = element;
  newElement->next = stackP->top;
  stackP->top = newElement;

}

// get the top function on stack
STATEMENT *loopStackPeep(stackL *stackP) {
  return stackP->top->function;
}

STATEMENT *loopStackPop(stackL *stackP)
{

	STATEMENT *returnF;

	returnF = stackP->top->function;
	stackP->top = stackP->top->next;

	return returnF;
	}


