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

#ifndef MAIN_ARGUMENTS
#define MAIN_ARGUMENTS
#define INVALID_ARGUMENTS -1
#define NO_ARGUMENTS 0
#define PEEPHOLE_STATISTICS 1
#define PRETTY_PRINTER 2
#define NO_OPTIMIZATION 3
#define RUNTIME_CHECKS 4
#endif

#ifndef PARSE_MSGS
#define PARSE_MSGS
#define PARSE_SUCCESS 0
#define PARSE_ERROR 1
#define PARSE_OUT_OF_MEM 2
#endif

int main_argument_decider(int argc, char **argStrings) {
	if ( argc == 2 ) {
		if (strcmp(argStrings[1], "--print") == 0 ||
			strcmp(argStrings[1], "-p") == 0) {
			return PRETTY_PRINTER;
		} else if (strcmp(argStrings[1], "--histogram") == 0 ||
				   strcmp(argStrings[1], "-h") == 0) {
			return PEEPHOLE_STATISTICS;
		} else if (strcmp(argStrings[1], "--nooptimization") == 0 ||
				strcmp(argStrings[1],"-n") == 0){
			return NO_OPTIMIZATION;
		} else if ( strcmp(argStrings[1], "-rtc") == 0) {
			return RUNTIME_CHECKS;
		}
	} else if ( argc == 1 ) {
		return NO_ARGUMENTS;
	}
	return INVALID_ARGUMENTS;
}

int lineno = 1;
BODY *_main_; // root of the AST
linked_list *ir_lines; // list of the IR lines of code

int main ( int argc, char *argv[] ) {

	if ( argc > 2 ) {
		fprintf(stderr, "Too many flags parsed to compiler.\n"
				"\nCorrect usage:\n\t./kitty \"flag\" "
				"< \"input_file\" > \"output_file\" \n\nValid "
				"flags are: \"--print\",\"-p\" OR \"--histogram\""
				",\"-h\" OR \"--nooptimization\",\"-n\" \n");
		exit(1);
	}

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
			switch ( main_argument_decider(argc, argv) ) {
				case PRETTY_PRINTER:
					printer_body(_main_);
					break;
				case PEEPHOLE_STATISTICS:
					IR_build(_main_, 0);
					begin_peephole( 1 );
					IR_printer(ir_lines);
					break;
				case NO_OPTIMIZATION:
					IR_build(_main_, 0);
					IR_printer(ir_lines);
					break;
				case NO_ARGUMENTS:
					IR_build(_main_, 0);
					begin_peephole( 0 );
					IR_printer(ir_lines);
					break;
				case RUNTIME_CHECKS:
					IR_build(_main_, 1);
					begin_peephole( 0 );
					IR_printer(ir_lines);
				default:
					break;
			}
			break;
		default:
			fprintf(stderr, "Error: Fatal error in parsing \n");
			return COMPILATION_FAILURE;
	}

	return COMPILATION_SUCCESS;
}
