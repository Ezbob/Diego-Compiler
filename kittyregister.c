#include <stdlib.h>
#include "kittyregister.h"


extern SECTION *mainSection;
struct SYMBOLTABLE *symboltable;
struct linked_list *code;
struct LIVENESS *tmproot;

void begin_register(linked_list *ir_lines){

	SECTION *tmpsec = mainSection;

	code = ir_lines;
	liveness_analysis();

	mainSection = tmpsec;

	
	while(mainSection != NULL){
		LIVENESS *lol = mainSection->root;
		printf("DEF, USE, IN, OUT\n");
		while(lol != NULL){
			printf("%d, %d, %d, %d\n", lol->def, lol->use, lol->in, lol->out);
			lol = lol->next;

		}
		mainSection = mainSection->nextSection;
	}
}

void liveness_analysis(){

	while(mainSection != NULL){
		linked_list *temp;
		temp = code->next;
		IR_INSTRUCTION *nextinstr;
		IR_INSTRUCTION *next = mainSection->first;
		symboltable = mainSection->symboltable;
		while(temp != code && (nextinstr = (IR_INSTRUCTION *) temp->data) != next){
			temp = temp->next;
		}

		LIVENESS *root = NEW(LIVENESS);
		LIVENESS *liveNext = root;
		root->prev = NULL;

		while(temp != code && (nextinstr = (IR_INSTRUCTION *) temp->data) != mainSection->last){
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

		//analyse next section
		mainSection = mainSection->nextSection;


	}
}

void analyseUseDef(LIVENESS *line, IR_INSTRUCTION *inst){

	line->def = 0;
	line->use = 0;
	line->in = 0;
	line->out = 0;

		switch(inst->op_code){
			case subl:
				if(ifTempAdd(inst->arg2)){
					line->use++;
				}
				if(ifTempAdd(inst->arg1)){
					line->use++;
				}
				if(ifTempAdd(inst->arg2)){
					line->def++;
				}
			break;
			case divl:
				if(ifTempAdd(inst->arg1)){
					line->use++;
				}
			break;
			case cmp:
				if(ifTempAdd(inst->arg2)){
					line->use++;
				}
				if(ifTempAdd(inst->arg1)){
					line->use++;
				}
			break;
			case imul:
			case addl:
				if(ifTempAdd(inst->arg2)){
					line->use++;
				}
				if(ifTempAdd(inst->arg1)){
					line->use++;
				}
				if(ifTempAdd(inst->arg2)){
					line->def++;
				}
			break;
			default:
				if(inst->op_code == movl){
					if(ifTempAdd(inst->arg2)){
						line->use++;
					}
				}
				if(ifTempAdd(inst->arg1)){
					line->use++;
				}
				if(ifTempAdd(inst->arg2)){
					line->def++;
				}
			break;
		}
}


int ifTempAdd(ARGUMENT *arg){
	if(arg == NULL)
		return 0;

	if(arg->kind == tempreg_arg || arg->kind == tempreg_arg){
		return 1;
	}

	return 0;
}