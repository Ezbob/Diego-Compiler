#ifndef __SYMBOL_H
#define __SYMBOL_H
#define HASH_SIZE 317
#include "../parserscanner/memory.h"

typedef struct SYMBOLTYPE {
  int visited;
  enum { SYMBOL_FUNCTION, SYMBOL_INT, SYMBOL_BOOL, SYMBOL_ID, SYMBOL_RECORD, SYMBOL_ARRAY, SYMBOL_NULL, SYMBOL_UNKNOWN } type;
  union { 
		struct TYPE *array;
		struct TYPE *declaration_type;
		struct TYPE *return_type;
		struct FUNCTION *func;
	    } value;
}SYMBOLTYPE;


typedef struct SYMBOL {
  char *name;
  int value;
  SYMBOLTYPE *symboltype;
  struct SYMBOL *next;
} SYMBOL;

typedef struct SYMBOLTABLE {
    SYMBOL *table[HASH_SIZE];
    struct SYMBOLTABLE *next;
} SYMBOLTABLE;

int Hash(char *str);

SYMBOLTABLE *initSymbolTable(void);

SYMBOLTABLE *scopeSymbolTable(SYMBOLTABLE *t);

SYMBOL *putSymbol(SYMBOLTABLE *t, char *name, int value, SYMBOLTYPE *symbolT);

SYMBOL *getSymbol(SYMBOLTABLE *t, char *name);

void dumpSymbolTable(SYMBOLTABLE *t);

void destroySymbolTable(SYMBOLTABLE *t);

void dumpSymbol(SYMBOL *symbol);
#endif /* END OF __SYMBOL_H */
