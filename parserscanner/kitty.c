#include <stdio.h>
#include <string.h>
#include "y.tab.h"
#include "kittytree.h"
#include "kittyprinter.h"


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

struct BODY *_main_;

int main() {
//int argc, char* argv[]
	//int DEBUG = 0;

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
			break;
		default:
			fprintf(stderr, "Error: Fatal error in parsing \n");
			return FAILURE;
			break;
	}
/*	if(argc > 1){

		if(strcmp(argv[1], "-d") == 0)
			DEBUG = 1;
	}

	if(DEBUG == 1){
		printer_body(_main_);
	}
*/

	return SUCCESS;
}