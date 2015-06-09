#include <stdio.h>
#include <string.h>
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
#define COMPILATION_SUCCESS 0
#define COMPILATION_FAILURE -1
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

int main(int argc, char *argv[]) {

	fprintf(stderr, "%s\n", "Initializing parsing phase");
	switch ( yyparse() ){
		case PARSE_ERROR:
			fprintf(stderr, "Error: Parse error detected\n");
			return COMPILATION_FAILURE;
			break;
		case PARSE_OUT_OF_MEM:
			fprintf(stderr, "Error: Parse out of memory\n");
			return COMPILATION_FAILURE;
			break;
		case PARSE_SUCCESS:
			begin_weed(_main_);
			collect(_main_);
			begin_check(_main_);
			if( argc > 1 && ( strcmp(argv[1],"--print") 
				|| strcmp(argv[1],"-p") ) ) {
				printer_body(_main_);
			} else {
				ir_codes = IR_build(_main_);	
			}
			break;
		default:
			fprintf(stderr, "Error: Fatal error in parsing \n");
			return COMPILATION_FAILURE;
			break;
	}

	return COMPILATION_SUCCESS;
}
