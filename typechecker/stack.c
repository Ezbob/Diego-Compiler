/*  
 * Based on the implementation of Robert I. Pitts
 * ----------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>  /* for dynamic allocation */
#include "stack.h"

stackT *stackInit()
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

void stackDestroy(stackT *stackP)
{
  stackT *stack = stackP;
  stack->top = NULL;  /* I.e., empty */
  /* Get rid of array. */
  free(stack);
}

int stackIsEmpty(stackT *stackP)
{
  return stackP->top == NULL;
}

/********************** Function Definitions for functions ******************/


void funcStackPush(stackT *stackP, FUNC *element)
{

  /* Put information in array; update top. */
  stackElement *newElement = NEW(stackElement);
  if(newElement == NULL){
    fprintf(stderr, "Failed to allocate for new element");
    return;
  }
  newElement->data = (void*) element;
  newElement->next = stackP->top;
  stackP->top = newElement;

}

// get the top function on stack
FUNC *funcStackPeek(stackT *stackP) {
  if (stackIsEmpty(stackP)) {
    return NULL;
  }
  return stackP->top->data;
}

FUNC *funcStackPop(stackT *stackP)
{
	if (stackIsEmpty(stackP)) {
	  fprintf(stderr, "Can't pop element from stack: stack is empty.\n");
	  return NULL;  /* Returning error. */
	}

	FUNC *returnF;

	returnF = (FUNC *) stackP->top->data;
	stackP->top = stackP->top->next;

	return returnF;
}

/*********************** Function Definitions for loop *********************/

void loopStackPush(stackT *stackP, STATEMENT *element)
{

  /* Put information in array; update top. */
  stackElement *newElement = NEW(stackElement);
  if(newElement == NULL){
    fprintf(stderr, "Failed to allocate for new element");
    return;
  }
  newElement->data = element;
  newElement->next = stackP->top;
  stackP->top = newElement;

}

// get the top function on stack
STATEMENT *loopStackPeek(stackT *stackP) {
  return stackP->top->data;
}

STATEMENT *loopStackPop(stackT *stackP)
{
  STATEMENT *returnF;

  returnF = (STATEMENT *) stackP->top->data;
  stackP->top = stackP->top->next;

  return returnF;
}


