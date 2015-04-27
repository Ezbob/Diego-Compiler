#ifndef __KITTY_SETTYPE_H
#define __KITTY_SETTYPE_H
#include "../parserscanner/kittytree.h"
#include "kittytype.h"

void begin_set( BODY *body );
void set_function ( FUNC *function );
void set_head ( HEAD *header );
void set_body ( BODY *body );
void set_type ( TYPE *type );
void set_par_decl_list ( PAR_DECL_LIST *pdecl );
void set_var_decl_list ( VAR_DECL_LIST *vdecl );
void set_var_type ( VAR_TYPE *vtype );
void set_decl_list ( DECL_LIST *dlst );
void set_declaration ( DECLARATION *decl );
TYPE *getRecursive( TYPE *type );

#endif

