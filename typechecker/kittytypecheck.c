#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kittytypecheck.h"
#include "symbol/symbol.h"

void begin_typecheck( BODY *body ){

	SYMBOLTABLE *symboltable = initSymbolTable();

	collect(body, symboltable);
	begin_set(body);
	begin_check(body);


}