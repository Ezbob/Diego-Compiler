#include <stdlib.h>
#include <string.h>
#include "kittyregister.h"

//Macro inspiration from mathcs.emory.edu
#define setbit(A,k) 	(A[(k / 32)] |= (1 << (k % 32)) )
#define clearbit(A,k)	(A[(k / 32)] &= ~(1 << (k % 32)))
#define testbit(A,k)	(A[(k / 32)] & (1 << (k % 32)) )

extern SECTION *mainSection;
struct SYMBOL_TABLE *symboltable;
struct linked_list *code;
struct LIVENESS *tmproot;

BIT_ARRAY *createarray(int i){
	return calloc(i,sizeof(BIT_ARRAY));
}

void begin_register(linked_list *ir_lines){

	SECTION *tmpsec = mainSection;
	symboltable = mainSection->symboltable;
	code = ir_lines;
	liveness_analysis();

	mainSection = tmpsec;
}

void liveness_analysis(){

	while(mainSection != NULL){
		linked_list *temp;
		temp = code->next;
		IR_INSTRUCTION *nextinstr;
		IR_INSTRUCTION *next = mainSection->first;
		symboltable = mainSection->symboltable;
		while(temp != code && (nextinstr = (IR_INSTRUCTION *)
				temp->data) != next){
			temp = temp->next;
		}

		LIVENESS *root = NEW(LIVENESS);
		LIVENESS *liveNext = root;
		root->prev = NULL;

		while(temp != code && (nextinstr = (IR_INSTRUCTION *) temp->data)
							  != mainSection->last){
			analyseUseDef(liveNext, next);

			temp = temp->next;
			next = (IR_INSTRUCTION *) temp->data;
			liveNext->next = NEW(LIVENESS);

			liveNext->next->prev = liveNext;
			liveNext = liveNext->next;
		}

		//While stops before last instruction so we need to do the last
		analyseUseDef(liveNext, next);

		mainSection->root = root;
		mainSection->tail = liveNext;

		getSuccessors(root, mainSection->first);

		//analyse next section
		mainSection = mainSection->nextSection;


	}
}

void analyseUseDef(LIVENESS *line, IR_INSTRUCTION *instr){

	int temps = mainSection->symboltable->temps;

	line->def = createarray(temps);
	line->use = createarray(temps);
	line->in = createarray(temps);
	line->out = createarray(temps);

		switch(instr->op_code){
			case subl:
				if(ifTempAdd(instr->arg2)){
					setbit(line->use, instr->arg2->temp_id);
				}
				if(ifTempAdd(instr->arg1)){
					setbit(line->use, instr->arg1->temp_id);
				}
				if(ifTempAdd(instr->arg2)){
					setbit(line->def, instr->arg2->temp_id);
				}
			break;
			case divl:
				if(ifTempAdd(instr->arg1)){
					setbit(line->use, instr->arg1->temp_id);
				}
			break;
			case cmp:
				if(ifTempAdd(instr->arg2)){
					setbit(line->use, instr->arg2->temp_id);
				}
				if(ifTempAdd(instr->arg1)){
					setbit(line->use, instr->arg1->temp_id);
				}
			break;
			case imul:
			case addl:
				if(ifTempAdd(instr->arg2)){
					setbit(line->use, instr->arg2->temp_id);
				}
				if(ifTempAdd(instr->arg1)){
					setbit(line->use, instr->arg1->temp_id);
				}
				if(ifTempAdd(instr->arg2)){
					setbit(line->def, instr->arg2->temp_id);
				}
			break;
			default:
				if(instr->op_code == movl){
					if(ifTempAdd(instr->arg2)){
						setbit(line->use, instr->arg2->temp_id);
					}
				}
				if(ifTempAdd(instr->arg1)){
					setbit(line->use, instr->arg1->temp_id);
				}
				if(ifTempAdd(instr->arg2)){
					setbit(line->def, instr->arg2->temp_id);
				}
			break;
		}
}


int ifTempAdd(ARGUMENT *arg){
	if(arg == NULL)
		return 0;

	if(arg->kind == tempReg_arg){
		return 1;
	}

	return 0;
}

void getSuccessors(LIVENESS *line, IR_INSTRUCTION *instr){

	IR_INSTRUCTION *startinstr = instr;
	LIVENESS *next = line;
	LIVENESS *linestore = next;
	linked_list *temp = code->next;
	linked_list *linkstore = temp;
	SECTION *secstore = mainSection;
	int found = 0;
	//Trying to keep track of successors
	SUCLIST *list = NEW(SUCLIST);
	SUCLIST *sroot = list;

	//Identifying labels
	while(next != mainSection->tail && temp != code){
		if(startinstr->op_code == label){
			list->element = next;
			list->labelId = startinstr->arg1->label;
			list->next = NEW(SUCLIST);
			list = list->next;
		}

		temp = temp->next;
		startinstr = (IR_INSTRUCTION *) temp->data;
		next = next->next;
	}
	//Pointer compare stops before the last instruction, hence:
	if(startinstr->op_code == directive){
		list->element = next;			
		list->labelId = startinstr->label;
		list->next = NEW(SUCLIST);
		list = list->next;
	}

	//Reset for second run
	temp = linkstore;
	mainSection = secstore;
	startinstr = instr;
	next = linestore;

	while(next != mainSection->tail && temp != code){
		switch(startinstr->op_code){
			//Conditional jump have 2 branches of successors
			case jne:
			case je:
			case jg:
			case JGE:
			case jl:
			case JLE:
				next->number_successors = 2;
				next->successors = malloc(sizeof(LIVENESS)*2);
				next->successors[0] = next->next;

				//find jump label
				list = sroot;
				found = 0;
				while(!found && list != NULL){
					if(strcmp(startinstr->label, list->labelId)==0){
						next->successors[1] = list->element;
						found = 1;
					}
					list = list->next;
				}
			break;

			//Unconditional jumps only have 1 branch of successors
			case jmp:
				next->number_successors = 1;
				next->successors = malloc(sizeof(LIVENESS));

				//find jump label
				list = sroot;
				found = 0;
				while(!found && list != NULL){
					if(strcmp(startinstr->label, list->labelId)==0){
						next->successors[0] = list->element;
						found = 1;
					}
					list = list->next;
				}
			break;

			default:
				next->number_successors = 1;
				next->successors = malloc(sizeof(LIVENESS));
				next->successors[0] = next->next;
			break;
		}
		temp = temp->next;
		startinstr = (IR_INSTRUCTION *) temp->data;
		next = next->next;
	}

}

			