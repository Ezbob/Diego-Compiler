#ifndef __SYMBOL_H
#define __SYMBOL_H
#define HASH_SIZE 317
#include "../parserscanner/memory.h"

typedef enum TYPES { // types valid in our compiles
  SYMBOL_FUNCTION, 
  SYMBOL_INT, 
  SYMBOL_BOOL, 
  SYMBOL_ID, 
  SYMBOL_RECORD, 
  SYMBOL_ARRAY, 
  SYMBOL_NULL, 
  SYMBOL_UNKNOWN
} TYPES;

typedef struct SYMBOLTYPE {
  int visited;
  struct SYMBOLTABLE *child;
  struct SYMBOLTYPE *nextInArray;
  TYPES type;
  union {
    struct TYPE *array;
    struct TYPE *declaration_type;
    struct TYPE *return_type;
    struct FUNCTION *func;
    struct VAR_DECL_LIST *parameters;
    int arguments;
  } value;
}SYMBOLTYPE;


typedef struct SYMBOL {
  char *name;
  int value;
  int noArguments;
  SYMBOLTYPE *symboltype;
  SYMBOLTYPE *subtype;
  struct SYMBOL *next;
  struct PAR_DECL_LIST *parameters;
  struct FUNCTION *func;
  struct TYPE *declarationtype;
  struct TYPE *returntype;
  struct TYPE *array;
  struct TYPE *realtype;
  char *uniquename;
  int offset;
  int arraySize;
} SYMBOL;

typedef struct SYMBOLTABLE {
    SYMBOL *table[HASH_SIZE];
    struct SYMBOLTABLE *next;
    int temps;
    int localVars;
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
