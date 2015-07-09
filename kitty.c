#include <stdio.h>
#include <string.h>
#include "parserscanner/y.tab.h"
#include "parserscanner/kittytree.h"
#include "parserscanner/kittyprinter.h"
#include "typechecker/kittyweed.h"
#include "typechecker/kittycollect.h"
#include "typechecker/kittymulticollect.h"
#include "typechecker/kittycheck.h"
#include "kittyir.h"
#include "kittyemit.h"
#include "peephole.h"

#ifndef SUCCESS_AND_FAILURE
#define SUCCESS_AND_FAILURE
#define COMPILATION_SUCCESS 0
#define COMPILATION_FAILURE 1
#endif

#ifndef PARSE_MSGS
#define PARSE_MSGS
#define PARSE_SUCCESS 0
#define PARSE_ERROR 1
#define PARSE_OUT_OF_MEM 2
#endif

int lineno = 1;
BODY *_main_; // root of the AST
linked_list *ir_lines; // list of the IR lines of code

int main ( int argc, char *argv[] ) {

	fprintf(stderr, "%s\n", "Initializing parsing phase");
	switch ( yyparse() ){
		case PARSE_ERROR:
			fprintf(stderr, "Error: Parse error detected\n");
			return COMPILATION_FAILURE;
		case PARSE_OUT_OF_MEM:
			fprintf(stderr, "Error: Parse out of memory\n");
			return COMPILATION_FAILURE;
		case PARSE_SUCCESS:
			begin_weed(_main_);
			begin_collect(_main_);
			begin_multi_collect(_main_);
			begin_check(_main_);
			if( argc > 1 && ( strcmp(argv[1],"--print") 
				|| strcmp(argv[1],"-p") ) ) {
				printer_body(_main_);
			} else {
				IR_build(_main_);
				begin_peephole();
				IR_printer(ir_lines);
			}
			break;
		default:
			fprintf(stderr, "Error: Fatal error in parsing \n");
			return COMPILATION_FAILURE;
	}

	return COMPILATION_SUCCESS;
}
