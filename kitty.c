#include <stdio.h>
#include "parserscanner/y.tab.h"
#include "parserscanner/kittytree.h"
#include "typechecker/kittyweed.h"
#include "typechecker/kittytype.h"
#include "typechecker/kittycheck.h"
#include "typechecker/kittyprinter.h"
#include "typechecker/kittysettype.h"
#include "symbol/symbol.h"
#include "kittyir.h"
#include "kittyregister.h"

#ifndef SUCCESS_AND_FAILURE
#define SUCCESS_AND_FAILURE
#define SUCCESS 0
#define FAILURE -1
#endif

#ifndef PARSE_MSGS
#define PARSE_MSGS
#define PARSE_SUCCESS 0
#define PARSE_ERROR 1
#define PARSE_OUT_OF_MEM 2
#endif

int lineno = 1;
struct BODY *_main_;
struct SYMBOLTABLE *globalTable;
struct SECTION *mainSection;
struct linked_list *ir_codes;

int main( void ) {

	globalTable = initSymbolTable();
	fprintf(stderr, "%s\n", "Initializing parsing phase");
	switch ( yyparse() ){
		case PARSE_ERROR:
			fprintf(stderr, "Error: Parse error detected\n");
			return FAILURE;
			break;
		case PARSE_OUT_OF_MEM:
			fprintf(stderr, "Error: Parse out of memory\n");
			return FAILURE;
			break;
		case PARSE_SUCCESS:
			begin_weed(_main_);
			collect(_main_, globalTable);
			//begin_set(_main_);
			fprintf(stderr, "%s\n", "Initializing typecheck phase");
			begin_check(_main_);
			printer_body(_main_);
			ir_codes = IR_build(_main_, globalTable);
			//begin_register(ir_codes);
			break;
		default:
			fprintf(stderr, "Error: Fatal error in parsing \n");
			return FAILURE;
			break;
	}

	return SUCCESS;
}
