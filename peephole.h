#ifndef PEEPHOLE_H
#define PEEPHOLE_H

#include "kittyir.h"

#define NUMBER_OF_TEMPLATES 7

#define PUSH_POP 0
#define TRANSIENT_MOVE 1
#define MOVE_BETWEEN_PUSH_POP 2
#define MOVE_SAME_REGISTER 3
#define ARITHMETIC_REGISTER_MOVE 4
#define PUSH_POP_SELF 5
#define SHORT_JUMP 6

#define COUNT_IN_HISTOGRAM(type) if( useHistogram ) histogramValues[type]++

void begin_peephole(int showHistogram);

void show_peephole_statistics();
void list_run();
void useless_push_pop(linked_list *);
void useless_transient_move(linked_list *);
void useless_move_between_push_pop(linked_list *);
void useless_arithmetic_register_moves(linked_list *);
void useless_move_of_same_register(linked_list *);
void useless_push_pop_self(linked_list *);
void short_jumps(linked_list *);
#endif //PEEPHOLE_H
