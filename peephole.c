#include <string.h>
#include "peephole.h"
#include "dlinkedlist.h"
#include "kittyir.h"
#include "irInstructions.h"

extern linked_list *ir_lines;
static linked_list *iterator;

int *historGramValues;
int patternIsGood = 0;
int lineCount = 1;

void begin_peephole() {
    fprintf(stderr, "Initializing peephold optimization phase \n ");
    list_run();

}

void list_run() {

    for (int i = 0; i < 3; i++) {
        patternIsGood = 0;
        iterator = ir_lines->next;
        while (iterator != ir_lines) {
            /* add templates here */

            useless_push_pop(iterator);
            // useless_constant_push(iterator);
            //useless_move_of_constant_to_reg(iterator);
            useless_move_between_push_pop(iterator);

            lineCount++;
            iterator = iterator->next;
        }
        fprintf(stderr,"pattern found: %i \n", patternIsGood);
    }
}

/* Example:
 * pushl %eax
 * popl %ebx
 * to:
 * movl %eax,%ebx
 */
void useless_push_pop(linked_list *currentLine) {

    IR_INSTRUCTION *currentInstruction =
            (IR_INSTRUCTION *) currentLine->data;
    IR_INSTRUCTION *nextInstruction =
            (IR_INSTRUCTION *) currentLine->next->data;

    linked_list *new_list_element;

    if( currentInstruction->op_code == pushl &&
                nextInstruction->op_code == popl ) {
        if ( (currentInstruction->arg1->kind == register_arg ||
                currentInstruction->arg1->kind == constant_arg ||
                currentInstruction->arg1->kind == address_arg) &&
                nextInstruction->arg1->kind == register_arg ) {

            new_list_element = NEW(linked_list);

            new_list_element->next = currentLine->next->next;
            new_list_element->previous = currentLine->previous;
            currentLine->next->next->previous = new_list_element;
            currentLine->previous->next = new_list_element;

            new_list_element->data = make_instruction_movl(
                    currentInstruction->arg1, nextInstruction->arg1);

            //patternIsGood++;
        }
    }
}

/*
 * example for template:
 * movl $1, %ebx
 * movl %ebx, %eax
 * to:
 * movl $1, %eax
 */
void useless_move_of_constant_to_reg(linked_list *currentLine){
    IR_INSTRUCTION *currentInstruction =
            (IR_INSTRUCTION *) currentLine->data;
    IR_INSTRUCTION *nextInstruction =
            (IR_INSTRUCTION *) currentLine->next->data;

    if(currentInstruction->op_code == movl
       && nextInstruction->op_code == movl){
        ARGUMENT *currentArg1 = currentInstruction->arg1;
        ARGUMENT *currentArg2 = currentInstruction->arg2;

        ARGUMENT *nextArg1 = nextInstruction->arg1;
        ARGUMENT *nextArg2 = nextInstruction->arg2;

        if( (currentArg1->kind == constant_arg &&
                currentArg2->kind == register_arg) &&
                (nextArg1->kind == register_arg &&
                        nextArg2->kind == register_arg) &&
                strcmp(currentArg2->charConst, nextArg1->charConst) == 0){
            //patternIsGood++;
        }
    }
}

void useless_move_between_push_pop(linked_list *currentLine){
    IR_INSTRUCTION *currentInstruction =
            (IR_INSTRUCTION *) currentLine->data;
    IR_INSTRUCTION *nextInstruction =
            (IR_INSTRUCTION *) currentLine->next->data;
    IR_INSTRUCTION *thirdInstruction =
            (IR_INSTRUCTION *) currentLine->next->next->data;

    linked_list *nextLine = currentLine->next;
    linked_list *thirdLine = currentLine->next->next;

    linked_list *new_list_element;

    if( currentInstruction->op_code == pushl &&
            nextInstruction->op_code == movl &&
            thirdInstruction->op_code == popl ) {


        if ( (currentInstruction->arg1->kind == address_arg ||
                currentInstruction->arg1->kind == register_arg ||
                currentInstruction->arg1->kind == constant_arg ) &&
             thirdInstruction->arg1->kind == register_arg ) {

            new_list_element = NEW(linked_list);
            new_list_element->data = make_instruction_movl(
                    currentInstruction->arg1, thirdInstruction->arg1);

            thirdLine->next->previous = nextLine;

            nextLine->previous = new_list_element;
            nextLine->next = thirdLine->next;

            currentLine->previous->next = new_list_element;
            new_list_element->next = nextLine;
            new_list_element->previous = currentLine->previous;

            //fprintf(stderr,"at line %i\n", lineCount);
            patternIsGood++;
        }
    }
}