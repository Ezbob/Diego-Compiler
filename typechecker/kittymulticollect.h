#ifndef __KITTY_MULTI_COLLECT_H
#define __KITTY_MULTI_COLLECT_H
#define MAXIMUM_PASSES 3

#include "../symbol/symbol.h"
#include "../parserscanner/kittytree.h"
#include "kittycollect.h"

void begin_multi_collect ( BODY * );
void multi_collect_body ( BODY * );
void multi_collect_head ( HEAD * );
void multi_collect_function ( FUNC * );
void multi_collect_type ( TYPE * );
void multi_collect_par_decl_list ( PAR_DECL_LIST * );
void multi_collect_var_decl_list ( VAR_DECL_LIST * );
void multi_collect_var_type ( VAR_TYPE * );
void multi_collect_decl_list ( DECL_LIST * );
void multi_collect_declaration ( DECLARATION * );

#endif