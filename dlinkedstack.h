#ifndef D_LINKED_STACK_H
#define D_LINKED_STACK_H
#include "dlinkedlist.h"

/**
 * This is a stack that acts like a wrapper for the dlinkedlist list to pro-
 * vide pop, peek, and push functionality
**/

typedef struct linked_stack {
	linked_list *elements; 
	int size; 
} linked_stack;

void *peek(linked_stack *);
void *pop(linked_stack *);
void push(linked_stack *,void *);

int get_size(linked_stack *);
linked_stack *initialize_stack();
void terminate_stack(linked_stack *);
void stack_dump(linked_stack *);

#endif