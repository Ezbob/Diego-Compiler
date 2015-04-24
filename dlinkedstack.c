#include "dlinkedstack.h"
#include <stdlib.h>
#include <stdio.h>

void *peek(linked_stack *stack) {
	return get_first_element(stack->elements);
}

void *pop(linked_stack *stack) {
	void *result;
	result = get_first_element(stack->elements);
	if ( delete_element(stack->elements, result) == EXIT_FAILURE ) {
		fprintf(stderr, "Error: could not delete top element\n");
		return NULL;
	}
	stack->size--;
	return result;
}

void push(linked_stack *stack, void *data) {
	prepend_element(stack->elements, data);
	stack->size++;
}

int get_size(linked_stack *stack) {
	return stack->size;
}

linked_stack *initialize_stack() {
	linked_stack *new_stack;
	new_stack = malloc(sizeof(linked_stack));
	new_stack->size = 0;
	initialize_list(new_stack->elements);
	return new_stack;
}

void terminate_stack(linked_stack *stack) {
	terminate_list(stack->elements);
	free(stack);
}

void stack_dump(linked_stack *stack) {
	printf("Stack %p contains: ", (void*) stack);
	list_dump(stack->elements);
}