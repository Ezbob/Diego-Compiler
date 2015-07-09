#ifndef PEEPHOLE_H
#define PEEPHOLE_H

#include "kittyir.h"

#define PUSH_POP 1
#define MOVE_BETWEEN_PUSH_POP 2

void begin_peephole();

void show_peephole_statistics();
void list_run();
void useless_push_pop(linked_list *);
void useless_move_of_constant_to_reg(linked_list *);
void useless_move_between_push_pop(linked_list *currentLine);
#endif //PEEPHOLE_H
