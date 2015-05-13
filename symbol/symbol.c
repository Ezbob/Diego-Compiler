#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbol.h"
#include "../parserscanner/kittytree.h"

//Computes the hash values
int Hash(char *str){
	int i;
	int result = 0;
	int str_size = strlen(str);

	for(i = 0 ; i < str_size; i++){
		result += str[i]; // add character value to partial result
		if(i < str_size-1){
			result <<= 1; // does not shift the last iteration
		}
	}

	return result % HASH_SIZE;
}

//Returns a pointer to a new initialized hash table
SYMBOLTABLE *initSymbolTable(int id){
	SYMBOLTABLE *tablePointer = NEW(SYMBOLTABLE);
	tablePointer->next = NULL;
	tablePointer->temps = 0;
	tablePointer->localVars = 0;
	tablePointer->id = id;
	int i;

	for (i = 0; i < HASH_SIZE; i++){
		(tablePointer->table)[i] = NULL;
	}
	
	return tablePointer;
}

/*
 * Takes a pointer to a hash table t as argument and returns
 * a new hash table with a pointer to t in its next field.
 */
SYMBOLTABLE *scopeSymbolTable(SYMBOLTABLE *t, int parentid){
	SYMBOLTABLE *tablePointer;
	int newid = parentid + 1;
	if ((tablePointer = initSymbolTable(newid)) == NULL){
		fprintf(stderr,
			"Error: Table not allocated, memory not available \n");
		return NULL;	
	}

	tablePointer->next = t;

	return tablePointer;
}

/*
 * Takes a hash table and a string, name, as arguments and inserts
 * name into the hash table together with the associated value value. A 
 * pointer to the SYMBOL value which stores name is returned.
 */
SYMBOL *putSymbol(SYMBOLTABLE *t, char *name, int value, SYMBOLTYPE *symbolT){
	int HashValue = Hash(name);
	SYMBOL *PutSymbol = NEW(SYMBOL);
	SYMBOL *Placeholder = NULL;

	if (PutSymbol == NULL){
		fprintf(stderr,
			"Error: Cannot allocate memory for new symbol \n");
		return NULL;		
	}
	PutSymbol->name = calloc(strlen(name)+1,sizeof(char));
	strncpy(PutSymbol->name,name,strlen(name)+1);
	PutSymbol->value = value;
	PutSymbol->next = NULL;
	PutSymbol->symboltype = symbolT;
	PutSymbol->uniquename = calloc(11,sizeof(char));
	PutSymbol->offset = 0;
	PutSymbol->arraySize = 0;
	PutSymbol->tableid = t->id;

	Placeholder = t->table[HashValue]; 
	if (Placeholder != NULL){ // has a SYMBOL in field 
		do{
			//Make sure that a SYMBOL name only appears once in a hash table
			if (strcmp(Placeholder->name, name)  == 0){
				fprintf(stderr, "Error: Symbol already exists at adress: %p\n", 
					   (void *) Placeholder);		
				
				free(PutSymbol->name);  // deallocate memory
				free(PutSymbol);

				return NULL;
			}
			Placeholder = Placeholder->next;
		}while(Placeholder != NULL); 
			// check if any name match or prepend to list		
		
	}
	
	// then we stack collisions
	PutSymbol->next = t->table[HashValue]; 
	t->table[HashValue] = PutSymbol;

	return PutSymbol;

}

/*
 * Takes a hash table and a string name as arguments and searches for
 * name in the following manner: First search for name in the hash table which
 * is one of the arguments of the function call. If name is not there, 
 * continue the search in the next hash table. This process is repeatedly 
 * recursively. If name has not been found after the root of the tree 
 * (see Fig. 1) has been checked, the result NULL is returned. 
 * If name is found, return a pointer to the SYMBOL value in which name 
 * is stored.
 */
SYMBOL *getSymbol(SYMBOLTABLE *t, char *name){
	SYMBOL *GetSymbol;
	SYMBOLTABLE *placeHolder;
	
	if (t == NULL){
		return NULL; // root reached, Symbol not found
	}	
	
	int HashValue = Hash(name);
	placeHolder = t;

	if (( GetSymbol = placeHolder->table[HashValue] ) == NULL){ 
		// not found in this table
		return getSymbol(t->next,name); 
	
	}else if(GetSymbol->next != NULL){ // collision detected
		do{
			if (strcmp(name,GetSymbol->name) == 0){
				return GetSymbol; // found name match
			}

			GetSymbol = GetSymbol->next;
			
		}while(GetSymbol != NULL);

		return getSymbol(t->next,name); 
			// collision but name did not match

	}else{ // found something and no collisions

		if(strcmp(name,GetSymbol->name) == 0){ // name match 
			return GetSymbol;
		}
		
		return getSymbol(t->next,name); 
			// no collision but name did not match
	}
}

/*
 * Takes a pointer to a hash table t as argument and prints all
 * the (name, value) pairs that are found in the hash tables from t up to the 
 * root.
 */
void dumpSymbolTable(SYMBOLTABLE *t){

	SYMBOL *symbol;
		
	if (t == NULL) { //Nothing or top node
 		return;
 	}
 
	printf("Reading hashtable located: %p\n",(void*) t );

 	for (int i = 0; i < HASH_SIZE; ++i){
 		symbol = t->table[i];
 
 		if (symbol != NULL) { // there is a symbol here 
			dumpSymbol(symbol);
			
 			while(symbol->next != NULL){ // print whole chain
 				dumpSymbol(symbol);				
				symbol = symbol->next; 
 			}printf("\n");
 		}
		
 	}
 
 	dumpSymbolTable(t->next); // recursively print the next SYMBOLTABLE 
}
void dumpTable(SYMBOLTABLE *t){

	SYMBOL *symbol;
		
	if (t == NULL) { //Nothing or top node
 		return;
 	}
 
	printf("Reading hashtable located: %p\n",(void*) t );

 	for (int i = 0; i < HASH_SIZE; ++i){
 		symbol = t->table[i];
 
 		if (symbol != NULL) { // there is a symbol here 
			dumpSymbol(symbol);
			
 			while(symbol->next != NULL){ // print whole chain
 				dumpSymbol(symbol);				
				symbol = symbol->next; 
 			}printf("\n");
 		}
		
 	}
}
void dumpSymbol(SYMBOL *symbol){
				
			switch(symbol->symboltype->type){
				case SYMBOL_FUNCTION:
					printf( "FUNCTION (%s , %i, %i) ", symbol->name, symbol->value, symbol->offset);
					break;
	
				case SYMBOL_INT:
					printf( "INT (%s , %i, %i) ", symbol->name, symbol->value, symbol->offset);
					break;

				case SYMBOL_BOOL:
					printf( "BOOL (%s , %i, %i) ", symbol->name, symbol->value, symbol->offset);
					break;

				case SYMBOL_ID:
					printf( "IDENTIFIER (%s , %i, %i) ", symbol->name, symbol->value, symbol->offset);
					break;

				case SYMBOL_RECORD:
					printf( "RECORD (%s , %i, %i) ", symbol->name, symbol->value, symbol->offset);
					break;

				case SYMBOL_ARRAY:
					printf( "ARRAY (%s , %i, %i)", symbol->name, symbol->value, symbol->offset);
					break;

				case SYMBOL_NULL:
					printf( "NULL (%s , %i, %i) ", symbol->name, symbol->value, symbol->offset);
					break;

				default:
					printf( "UNKNOWN (%s , %i, %i) ", symbol->name, symbol->value, symbol->offset);
					break;

			}

}


/* 
 * Deallocate the specific SYMBOLTABLE, all it's SYMBOL
 */
void destroySymbolTable(SYMBOLTABLE *t){
	int i;
	SYMBOL *s, *s2, *s3;

	for (i = 0; i < HASH_SIZE; i++){
		s = t->table[i];
		if ( s != NULL ){
			s2 = s;
			while(s->next != NULL) {
				s = s->next;
				s3 = s;
				free(s3->name);
				free(s3);
			}
			free(s2);
		}
	}
	free(t);
}
