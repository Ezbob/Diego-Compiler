#ifndef __KITTY_SETTYPE_H
#define __KITTY_SETTYPE_H
#include "../parserscanner/kittytree.h"
#include "kittytype.h"

void begin_set( BODY * );
void set_function ( FUNC * );
void set_head ( HEAD * );
void set_body ( BODY * );
void set_type ( TYPE * );
void set_par_decl_list ( PAR_DECL_LIST * );
void set_var_decl_list ( VAR_DECL_LIST * );
void set_var_type ( VAR_TYPE * );
void set_decl_list ( DECL_LIST * );
void set_declaration ( DECLARATION * );
TYPE *getRecursive( TYPE * );

#endif

