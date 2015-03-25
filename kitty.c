#include <stdio.h>
#include "parserscanner/y.tab.h"
#include "parserscanner/kittytree.h"
#include "kittyweed.h"
#include "kittyprinter.h"
#include "symbol/symbol.h"
#include "kittytype.h"
#include "kittycheck.h"
#ifndef SUCCESS_AND_FAILURE
#define SUCCESS 0
#define FAILURE -1
#endif

#ifndef PARSE_MSGS
#define PARSE_SUCCESS 0
#define PARSE_ERROR 1
#define PARSE_OUT_OF_MEM 2
#endif

int lineno = 1;
struct BODY *_main_;

int main( void )
{
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
	begin_weed(_main_);

	collect(_main_);
	
	begin_check(_main_);

	printer_body(_main_);


	return SUCCESS;
}
