#include <string.h>
#include <stdlib.h>
#include "peephole.h"
#include "irInstructions.h"

extern linked_list *ir_lines;
static linked_list *iterator;

int *histogramValues = NULL;
static int useHistogram;
int frequencySum = 0;
int numberOfPasses = 0;
int lineCount = 1;
linked_list *first;

void begin_peephole(int showHistogram) {
    fprintf(stderr, "Initializing peephole optimization phase \n");
    useHistogram = showHistogram;
    first = ir_lines->next;
    if ( showHistogram ) {
        histogramValues = calloc(NUMBER_OF_TEMPLATES,sizeof(int));
    }
    list_run();
    if ( showHistogram ) {
        show_peephole_statistics();
        free(histogramValues);
    }
}

void list_run() {
    do {
        frequencySum = 0;
        lineCount = 0;
        iterator = ir_lines->next;
        while (iterator != ir_lines) {
            /* add templates here */

            useless_arithmetic_register_moves(iterator);
            useless_push_pop_self(iterator);
            useless_push_pop(iterator);
            useless_move_between_push_pop(iterator);
            useless_transient_move(iterator);
            useless_move_of_same_register(iterator);
            short_jumps(iterator);

            lineCount++;
            iterator = iterator->next;
        }
        numberOfPasses++;
    } while ( frequencySum > 0 );
}

/* Example:
 * pushl %ebx
 * popl %ebx
 * to: delete instruction
 */
void useless_push_pop_self(linked_list *currentLine) {

    IR_INSTRUCTION *currentInstruction =
            (IR_INSTRUCTION *) currentLine->data;
    IR_INSTRUCTION *nextInstruction =
            (IR_INSTRUCTION *) currentLine->next->data;

    if( currentInstruction->op_code == pushl &&
            nextInstruction->op_code == popl) {
        if( currentInstruction->arg1->kind == register_arg &&
            nextInstruction->arg1->kind == register_arg) {
            if( strcmp(currentInstruction->arg1->charConst,
                        nextInstruction->arg1->charConst) == 0){
                currentLine->previous->next = currentLine->next->next;
                currentLine->next->next->previous = currentLine->previous;

                COUNT_IN_HISTOGRAM(PUSH_POP_SELF);
                frequencySum++;

            }
        }
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
                currentInstruction->arg1->kind == address_arg ||
                currentInstruction->arg1->kind == indexing_arg) &&
                nextInstruction->arg1->kind == register_arg ) {

            new_list_element = NEW(linked_list);

            new_list_element->next = currentLine->next->next;
            new_list_element->previous = currentLine->previous;
            currentLine->next->next->previous = new_list_element;
            currentLine->previous->next = new_list_element;

            new_list_element->data = make_instruction_movl(
                    currentInstruction->arg1, nextInstruction->arg1);

            COUNT_IN_HISTOGRAM(PUSH_POP);
            frequencySum++;
        }
    }
}

/*
 * example for template:
 * movl $1, %ebx
 * movl %ebx, -4(%eax)
 * to:
 * movl $1, -4(%eax)
 * OR
 * movl %ecx, %ebx
 * movl %ebx, %edx
 * to:
 * movl %ecx, %edx
 */
void useless_transient_move(linked_list *currentLine) {
    IR_INSTRUCTION *currentInstruction =
            (IR_INSTRUCTION *) currentLine->data;
    IR_INSTRUCTION *nextInstruction =
            (IR_INSTRUCTION *) currentLine->next->data;
    IR_INSTRUCTION *thirdInstruction =
            (IR_INSTRUCTION *) currentLine->next->next->data;
    linked_list *new_list_element;

    linked_list *nextLine = currentLine->next;

    if( currentInstruction->op_code == movl
       && nextInstruction->op_code == movl ){
        ARGUMENT *currentArg1 = currentInstruction->arg1;
        ARGUMENT *currentArg2 = currentInstruction->arg2;

        ARGUMENT *nextArg1 = nextInstruction->arg1;
        ARGUMENT *nextArg2 = nextInstruction->arg2;

        if( (currentArg1->kind == register_arg ||
                currentArg1->kind == constant_arg )
            && currentArg2->kind == register_arg
             && nextArg1->kind == register_arg
             && strcmp(currentArg2->charConst, nextArg1->charConst) == 0){

            if ( thirdInstruction->arg1 != NULL &&
                    thirdInstruction->arg1->kind == register_arg &&
                    strcmp(nextArg1->charConst, thirdInstruction->arg1
                            ->charConst) == 0){
                // register still in use
                return;
            }

            new_list_element = NEW(linked_list);

            new_list_element->next = nextLine->next;
            new_list_element->previous = currentLine->previous;
            nextLine->next->previous = new_list_element;
            currentLine->previous->next = new_list_element;

            new_list_element->data = make_instruction_movl(
                    currentArg1, nextArg2);

            COUNT_IN_HISTOGRAM(TRANSIENT_MOVE);
            frequencySum++;
        }
    }
}

/*
 * example for template:

 */
void useless_move_between_push_pop(linked_list *currentLine) {
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

            COUNT_IN_HISTOGRAM(MOVE_BETWEEN_PUSH_POP);
            frequencySum++;
        }
    }
}

/*
 * example for template
 * movl %ebx, %eax
 * movl %edx, %ecx
 * addl %ecx, %eax
 * pushl %eax
 * to:
 * addl %edx, %ebx
 * pushl %ebx
 */
void useless_arithmetic_register_moves(linked_list *currentLine) {
    IR_INSTRUCTION *currentInstruction =
            (IR_INSTRUCTION *) currentLine->data;
    IR_INSTRUCTION *nextInstruction =
            (IR_INSTRUCTION *) currentLine->next->data;
    IR_INSTRUCTION *thirdInstruction =
            (IR_INSTRUCTION *) currentLine->next->next->data;
    IR_INSTRUCTION *fourthInstruction =
            (IR_INSTRUCTION *) currentLine->next->next->next->data;

    linked_list *newInstruction1;
    linked_list *newInstruction2;

    if (currentInstruction->op_code == movl &&
        nextInstruction->op_code == movl) {

        if (currentInstruction->arg1->kind == register_arg &&
            currentInstruction->arg2->kind == register_arg &&
            nextInstruction->arg2->kind == register_arg &&
                fourthInstruction->op_code == pushl) {

            if (thirdInstruction->op_code == addl ||
                thirdInstruction->op_code == imul ||
                thirdInstruction->op_code == subl) {

                newInstruction1 = NEW(linked_list);
                newInstruction2 = NEW(linked_list);

                if (thirdInstruction->op_code == addl) {
                    newInstruction1->data =
                            make_instruction_addl(nextInstruction->arg1,
                                                  currentInstruction->arg1);
                } else if (thirdInstruction->op_code == imul) {
                    newInstruction1->data =
                            make_instruction_imul(nextInstruction->arg1,
                                                  currentInstruction->arg1);

                } else if (thirdInstruction->op_code == subl) {
                    newInstruction1->data =
                            make_instruction_subl(nextInstruction->arg1,
                                                  currentInstruction->arg1);
                }
                newInstruction2->data =
                        make_instruction_pushl(currentInstruction->arg1);

                newInstruction1->previous = currentLine->previous;
                currentLine->previous->next = newInstruction1;
                newInstruction1->next = newInstruction2;

                newInstruction2->previous = newInstruction1;
                newInstruction2->next = currentLine->next->next->next->next;
                currentLine->next->next->next->next->previous
                        = newInstruction2;
                COUNT_IN_HISTOGRAM(ARITHMETIC_REGISTER_MOVE);
                frequencySum++;
            }
        }
    }
}

/*
 * example for template:
 * movl %ebx, %ebx
 * to: delete instruction
 */
void useless_move_of_same_register(linked_list *currentLine) {
    IR_INSTRUCTION *currentInstruction = (IR_INSTRUCTION *) currentLine->data;

    if ( currentInstruction->op_code == movl
         && currentInstruction->arg1->kind == register_arg
            && currentInstruction->arg2->kind == register_arg ) {
        if ( strcmp(currentInstruction->arg1->charConst,
                    currentInstruction->arg2->charConst) == 0) {

            currentLine->previous->next = currentLine->next;
            currentLine->next->previous = currentLine->previous;
            COUNT_IN_HISTOGRAM(MOVE_SAME_REGISTER);
            frequencySum++;
        }
    }
}

/*
 * example for template:
 * jmp label
 * label
 * to:
 * label
 */
void short_jumps(linked_list *currentLine) {

    IR_INSTRUCTION *currentInstruction =
            (IR_INSTRUCTION *) currentLine->data;
    IR_INSTRUCTION *nextInstruction =
            (IR_INSTRUCTION *) currentLine->next->data;


    if( currentInstruction->op_code == jmp &&
        nextInstruction->op_code == label){
        if( strcmp(currentInstruction->label, 
            nextInstruction->label) == 0 ){
            
            currentLine->previous->next = currentLine->next;
            currentLine->next->previous = currentLine->previous;

            COUNT_IN_HISTOGRAM(SHORT_JUMP);
            frequencySum++;
        }
    }

}


void get_template_name(int index) {

    fprintf(stderr, "useless_");
    switch (index){
        case PUSH_POP:
            fprintf(stderr, "push_pop");
            break;
        case TRANSIENT_MOVE:
            fprintf(stderr, "transient_move");
            break;
        case MOVE_BETWEEN_PUSH_POP:
            fprintf(stderr, "move_between_push_pop");
            break;
        case MOVE_SAME_REGISTER:
            fprintf(stderr, "move_of_same_register");
            break;
        case ARITHMETIC_REGISTER_MOVE:
            fprintf(stderr, "arithmetic_register_move");
            break;
        case PUSH_POP_SELF:
            fprintf(stderr, "push_pop_self");
            break;
        case SHORT_JUMP:
            fprintf(stderr, "short_jump");
            break;
        default:
            break;
    }
}


void show_peephole_statistics(){

    fprintf(stderr,"---peephole usage---\n");
    for(int i = 0; i < NUMBER_OF_TEMPLATES; i ++) {
        get_template_name(i);
        fprintf(stderr,": %i\n", histogramValues[i]);
    }
    fprintf(stderr,"number of peephole passes: %i \n", numberOfPasses);
}

