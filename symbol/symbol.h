#ifndef __SYMBOL_H
#define __SYMBOL_H
#define HASH_SIZE 317
#include "../parserscanner/memory.h"

typedef enum TYPES_SUPPORTED {
    SYMBOL_FUNCTION, 
    SYMBOL_INT, 
    SYMBOL_BOOL, 
    SYMBOL_ID, 
    SYMBOL_RECORD, 
    SYMBOL_ARRAY, 
    SYMBOL_NULL, 
    SYMBOL_UNKNOWN 
} TYPES_SUPPORTED;

typedef enum SYMBOL_KIND {
    LOCAL_VARIABLE_SYMBOL,
    PARAMETER_SYMBOL,
    RECORD_MEMBER_SYMBOL,
    TYPE_DEFINE_SYMBOL
} SYMBOL_KIND;

typedef struct SYMBOL_TYPE {
    struct SYMBOL_TYPE *nextArrayType; // for arrays
    struct VAR_DECL_LIST *recordMembers; // for records
    struct SYMBOL_TYPE *return_type; // for functions
    struct SYMBOL_TABLE *childScope; // for records
    TYPES_SUPPORTED type;
    int arguments; // or members
    int arrayDim;
} SYMBOL_TYPE;

typedef struct SYMBOL {
    char *name;
    SYMBOL_TYPE *symbolType;
    SYMBOL_KIND symbolKind;
    struct SYMBOL *next; // for hash chaining
    struct PAR_DECL_LIST *functionParameters;
    int noParameters; // number of parameters needed by the function
    int offset; // for offsetting into the stack
    int tableId; // for checking when to use static link
} SYMBOL;

typedef struct SYMBOL_TABLE {
    SYMBOL *table[HASH_SIZE];
    struct SYMBOL_TABLE *next;
    struct SYMBOL_TABLE *recordParentScope;
    int localVars; // used in allocation of local variables
    int id;
} SYMBOL_TABLE;

int Hash(char *str);

SYMBOL_TABLE *initSymbolTable(int id);

SYMBOL_TABLE *scopeSymbolTable(SYMBOL_TABLE *parentTable, int parentId);

SYMBOL *putSymbol(SYMBOL_TABLE *t, char *name, SYMBOL_TYPE *symbolT);

SYMBOL *getSymbol(SYMBOL_TABLE *t, char *name);

void dumpSymbolTable(SYMBOL_TABLE *t);

void dumpTable(SYMBOL_TABLE *t);

void destroySymbolTable(SYMBOL_TABLE *t);

void dumpSymbol(SYMBOL *symbol);
#endif /* END OF __SYMBOL_H */
