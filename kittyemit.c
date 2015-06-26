#include "kittyemit.h"

void IR_printer(linked_list *ir_lines){
	fprintf(stderr,"Initializing code emitting phase\n");

	linked_list *temp;
	temp = ir_lines->next;
	IR_INSTRUCTION *instr_to_print;

	while(temp != ir_lines) {

		instr_to_print = (IR_INSTRUCTION *) temp->data;

		switch(instr_to_print->op_code){

			case popl:
				printf("\t%s", "popl ");
				IR_printer_arguments(instr_to_print->arg1);
				printf("\n");
				break;

			case pushl:
				printf("\t%s", "pushl ");
				IR_printer_arguments(instr_to_print->arg1);
				printf("\n");
				break;

			case ret:
				printf("\t%s\n", "ret");
				break;

			case movl:
				printf("\t%s", "movl ");
				IR_printer_arguments(instr_to_print->arg1);
				printf(", ");
				IR_printer_arguments(instr_to_print->arg2);
				printf("\n");
				break;

			case addl:
				printf("\t%s", "addl ");
				IR_printer_arguments(instr_to_print->arg1);
				printf(", ");
				IR_printer_arguments(instr_to_print->arg2);
				printf("\n");
				break;

			case subl:
				printf("\t%s", "subl ");
				IR_printer_arguments(instr_to_print->arg1);
				printf(", ");
				IR_printer_arguments(instr_to_print->arg2);
				printf("\n");
				break;

			case imul:
				printf("\t%s", "imul ");
				IR_printer_arguments(instr_to_print->arg1);
				printf(", ");
				IR_printer_arguments(instr_to_print->arg2);
				printf("\n");
				break;				

			case xor:
				printf("\t%s", "xorl ");
				IR_printer_arguments(instr_to_print->arg1);
				printf(", ");
				IR_printer_arguments(instr_to_print->arg2);
				printf("\n");
				break;

			case divl:
				printf("\t%s", "idivl ");
				IR_printer_arguments(instr_to_print->arg1);
				printf("\n");
				break;

			case call:
				printf("\t%s", "call ");
				IR_printer_arguments(instr_to_print->arg1);
				printf("\n");
				break;

			case directive:
				printf("%s", instr_to_print->label);
				printf("\n");
				break;

			case label:
				printf("%s:\n",instr_to_print->label);
				break;

			case cmp:
				printf("\t%s", "cmp ");
				IR_printer_arguments(instr_to_print->arg1);
				printf(", ");
				IR_printer_arguments(instr_to_print->arg2);
				printf("\n");
				break;

			case jne:
				printf("\t%s", "jne ");
				printf("%s", instr_to_print->label);
				printf("\n");
				break;

			case jmp:
				printf("\t%s", "jmp ");
				printf("%s", instr_to_print->label);
				printf("\n");
				break;

			case je:
				printf("\t%s", "je ");
				printf("%s", instr_to_print->label);
				printf("\n");
				break;

			case jl:
				printf("\t%s", "jl ");
				printf("%s", instr_to_print->label);
				printf("\n");
				break;

			case jg:
				printf("\t%s", "jg ");
				printf("%s", instr_to_print->label);
				printf("\n");
				break;

			case JGE:
				printf("\t%s", "jge ");
				printf("%s", instr_to_print->label);
				printf("\n");
				break;

			case JLE:
				printf("\t%s", "jle ");
				printf("%s", instr_to_print->label);
				printf("\n");
				break;

			case notl:
				printf("\t%s", "notl ");
				IR_printer_arguments(instr_to_print->arg1);
				printf("\n");
				break;

			case negl:
				printf("\t%s", "negl ");
				IR_printer_arguments(instr_to_print->arg1);
				printf("\n");
				break;

			case intCode:
				printf("\t%s", "INT ");
				printf("$%s", instr_to_print->label);
				printf("\n");
				break;				

			case space:
				IR_printer_arguments(instr_to_print->arg1);
				printf(":\n\t.space ");
				IR_printer_arguments(instr_to_print->arg2);
				printf("\n");
				break;

			case leal:
				printf("\tleal ");
				IR_printer_arguments(instr_to_print->arg1);
				printf(", ");
				IR_printer_arguments(instr_to_print->arg2);
				printf("\n");
				break;
			case incl:
				printf("\tincl ");
				IR_printer_arguments(instr_to_print->arg1);
				printf("\n");
				break;
			case decl:
				printf("\tdecl ");
				IR_printer_arguments(instr_to_print->arg1);
				printf("\n");
				break;
			default:
				break;
		}
		temp = temp->next;
	}
}

void IR_printer_arguments(ARGUMENT *arg){

	switch(arg->kind){

		case constant_arg:
			printf("$%d", arg->intConst);
			break;

		case register_arg:
			printf("%%");
			printf("%s", arg->charConst);
			break;

		case tempReg_arg:
			printf("%%");
			printf("%s%i", "temp", arg->temp_id);
			break;

		case label_arg:
			printf("%s", arg->label);
			break;

		case address_arg:
			printf("%d(%%ebp)", arg->intConst);
			break;

		case indexing_arg:
			IR_printer_arguments(arg->displace);
			printf("(,");
			IR_printer_arguments(arg->index);
			printf(",4)");
			break;
			
		case staticLink_arg:
			printf("%d(%%ebx)", arg->intConst);
			break;
		default:
			break;
	}
}