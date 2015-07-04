/*  
 * Based on the implementation of Robert I. Pitts
 * ----------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>  /* for dynamic allocation */
#include "funcstack.h"


/************************ Function Definitions **********************/

stackT *funcStackInit()
{

  /* Allocate a new array to hold the contents. */

  stackT *the_stack = NEW(stackT);

  if (the_stack == NULL) {
    fprintf(stderr, "Insufficient memory to initialize stack.\n");
    return NULL;  /* Exit, returning error code. */
  }

  the_stack->top = NULL;  /* I.e., empty */

  return the_stack;
}

void funcStackDestroy(stackT *stackP)
{
  stackT *stack = stackP;
  stack->top = NULL;  /* I.e., empty */
  /* Get rid of array. */
  free(stack);
}

void funcStackPush(stackT *stackP, FUNC *element)
{

  /* Put information in array; update top. */
  stackElement *newElement = NEW(stackElement);
  if(newElement == NULL){
	 fprintf(stderr, "Failed to allocate for new element");
  }
  newElement->function = element;
  newElement->next = stackP->top;
  stackP->top = newElement;

}

// get the top function on stack
FUNC *funcStackPeep(stackT *stackP) {
  if (StackIsEmpty(stackP)) {
    return NULL;
  }
  return stackP->top->function;
}

FUNC *funcStackPop(stackT *stackP)
{
	if (StackIsEmpty(stackP)) {
	  fprintf(stderr, "Can't pop element from stack: stack is empty.\n");
	  return NULL;  /* Returning error. */
	}

	FUNC *returnF;

	returnF = stackP->top->function;
	stackP->top = stackP->top->next;

	return returnF;
	}

int StackIsEmpty(stackT *stackP)
{
  return stackP->top == NULL;
}
/* 
 *Not used 

int StackIsFull(stackT *stackP)
{
  return stackP->top >= stackP->maxSize - 1;
}
*/
