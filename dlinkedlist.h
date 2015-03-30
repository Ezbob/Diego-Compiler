#ifndef DLINKED_LIST_H 
#define DLINKED_LIST_H

#include <stdio.h>

/* errors */
#define INVAL_ARGS_MSG fprintf(stderr,"Error: Invalid arguments\n")
#define OUT_OF_BOUND_MSG fprintf(stderr,"Error: List index out of bound\n")
#define OUT_OF_MEM_MSG fprintf(stderr,"Error: could not allocate memory to" \
															"new element\n") 

typedef struct linked_list{
	void *data; 
	struct linked_list *next;
	struct linked_list *previous;
} linked_list;

/*
 * Allocates memory to the list and setups the sentry element
 */
linked_list *initialize_list();

/*
 * Returns EXIT_SUCCESS if element was removed, returns EXIT_FAILURE otherwise
 */
int delete_element(linked_list *, void *);

/* 
 * Appending a new element to the list
 */
void append_element(linked_list *, void *);

/*
 * Prepends a new element to the list 
 */
void prepend_element(linked_list *, void *);

/* 
 * Retrives the first element in the list, or NULL if no first element is found
 */
void *get_first_element(linked_list *);

/*
 * Retrives the last element in the list, or NULL if no last element is found
 */
void *get_last_element(linked_list *);

/*
 * Get the index of the specified element, returns 
 * -1 if not found 
 */
int get_index_of_element(linked_list *, void *);

/*
 * Retrives the k'th element in the list, or NULL if index 
 * is out of bounce 
 */
void *get_element(linked_list *, int);

/*
 * Gets a count of the elements in the linked list or -1 if error
 */
int get_length(linked_list *);

/* 
 * Makes a string containing all pointers to the element data in the linked list.
 * This hopefully gives some insight into the contents of the linked list. 
 */
void list_dump(linked_list *);

/*
 * Deallocates all the nodes in the whole list
 */
void terminate_list(linked_list **);

#endif /* END OF DLINKED_LIST_H */
