/*
	This is a circular doubly linked list.
	The very first element in the list is the sentry 
	element and does not count as a element of the list.
	Made by Anders Busch (2015)
*/
#include <stdlib.h>
#include "dlinkedlist.h"

/*
 * Allocates memory to the list and setups the sentry element
 * returns NULL if unsuccessful
 */
linked_list *initialize_list(){
	linked_list *new_list;
	new_list = malloc(sizeof(linked_list));

	if(new_list == NULL){
		OUT_OF_MEM_MSG;
		return NULL;	
	}

	new_list->next = new_list;
	new_list->previous = new_list;
	return new_list;
}

/* 
 * Appending a new element to the list
 */
void append_element(linked_list *list, void *element){
	if(list == NULL || element == NULL){
		INVAL_ARGS_MSG;
		return;	
	}

	linked_list *new_list_element;

	new_list_element = malloc(sizeof(linked_list));
	
	if(new_list_element == NULL){
		OUT_OF_MEM_MSG;
		return;	
	}

	new_list_element->data = element;

	// list->previous is the (old) last element in the list

	new_list_element->previous = list->previous;
	new_list_element->next = list;

	list->previous->next = new_list_element;
	list->previous = new_list_element;
}

/*
 * Prepends a new element to the list 
 */
void prepend_element(linked_list *list, void *element){
	if(list == NULL || element == NULL){
		INVAL_ARGS_MSG;
		return;	
	}

	linked_list *new_list_element;

	new_list_element = malloc(sizeof(linked_list));
	
	if(new_list_element == NULL){
		OUT_OF_MEM_MSG;
		return;	
	}

	new_list_element->data = element;

	// list->next is the old first element in the list

	new_list_element->next = list->next;
	new_list_element->previous = list;

	list->next->previous = new_list_element;
	list->next = new_list_element;
}

/*
 * Returns EXIT_SUCCESS if element was removed, returns EXIT_FAILURE otherwise
 */
int delete_element(linked_list *list, void *element){
	if(list == NULL || element == NULL){
		INVAL_ARGS_MSG;
		return EXIT_FAILURE;	
	}

	linked_list *iterator;

	iterator = list->next;

	while(iterator != list){
		if(iterator->data == element){
			iterator->previous->next = iterator->next;
			iterator->next->previous = iterator->previous;
			free(iterator);
			return EXIT_SUCCESS;
		}
		iterator = iterator->next;
	}
	return EXIT_FAILURE;
}

/* 
 * Retrives the first element in the list, or NULL if no first element is 
 * found
 */
void *get_first_element(linked_list *list){
	if(list == NULL){
		INVAL_ARGS_MSG;
		return NULL;	
	}
	
	if(list->next == list){
		OUT_OF_BOUND_MSG;
		return NULL;
	}
	return list->next->data;
}


/*
 * Retrives the last element in the list, or NULL if no last element is found
 */
void *get_last_element(linked_list *list){
	if(list == NULL){
		INVAL_ARGS_MSG;
		return NULL;	
	}

	if(list->previous == list){
		OUT_OF_BOUND_MSG;
		return NULL;
	}
	return list->previous->data;
}

/*
 * Retrives the k'th element in the list, or NULL and an error message if 
 * index is out of bounce 
 */
void *get_element(linked_list *list, int k){
	if(list == NULL ){
		INVAL_ARGS_MSG;
		return NULL;	
	}

	linked_list *iterator;
	int i;

	if( k >= 0 && k < get_length(list) ){
		iterator = list->next;

		for (i = 0; i < k; ++i)
			iterator = iterator->next;

		return iterator->data;
	}

	OUT_OF_BOUND_MSG;
	return NULL;
}

/*
 * Gets the index of the element
 * returns -1 if not found
 */
int get_index_of_element(linked_list *list, void *data){
	if (list == NULL || data == NULL){
		INVAL_ARGS_MSG;
		return -1;
	}
	int i;
	linked_list *iterator;
	iterator = list->next;

	for(i = 0; list != iterator; i++ ){
		if(data == iterator->data){
			return i;
		}
		iterator = iterator->next;
	}

	return -1;
}

/*
 * Gets a count of the elements in the linked list or -1 if error
 */
int get_length(linked_list *list){
	if(list == NULL){
		INVAL_ARGS_MSG;
		return -1;	
	}

	linked_list *iterator;
	int counter;

	counter = 0;
	iterator = list->next;

	while(iterator != list){
		counter++;
		iterator = iterator->next;
	}

	return counter;
}

/* 
 * Makes a string containing all pointers to the element data 
 * in the linked list. This hopefully gives some insight into 
 * the contents of the linked list. 
 */
void list_dump(linked_list *list){
	if(list == NULL){
		INVAL_ARGS_MSG;
		return;	
	}

	linked_list *iterator;
	
	iterator = list->next;
	
	printf("[");
	while(list != iterator){
		if(iterator->next == list){
			printf("%p",iterator->data);
		}else{
			printf("%p,",iterator->data);
		}
		iterator = iterator->next;
	}
	printf("]\n");
}

/*
 * Deallocates all the nodes in the whole list
 */
void terminate_list(linked_list **list){
	if(list == NULL || *list == NULL){
		INVAL_ARGS_MSG;
		return;	
	}

	linked_list *iterator;
	linked_list *placeholder;

	iterator = (*list)->next;
	while (iterator != (*list)){
		placeholder = iterator->next;
		free(iterator);
		iterator = placeholder;
	}
	free(*list);
	*list = NULL;
}


