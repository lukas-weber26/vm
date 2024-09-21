#include "disassembler.h"
#include <stdint.h>
#include <stdio.h>

typedef enum {PRINT, NO_PRINT} print_imediate_length;

void print_cl(FILE * output_stream) {
	fprintf(output_stream, "cl"); 
}

void print_reg(uint16_t reg, uint16_t data, uint8_t w, FILE * output_stream) {
	if (w == 0) {
		switch (reg) {
			case 0: fprintf(output_stream, "al"); break;	
			case 1: fprintf(output_stream, "cl"); break;	
			case 2: fprintf(output_stream, "dl"); break;	
			case 3: fprintf(output_stream, "bl"); break;	
			case 4: fprintf(output_stream, "ah"); break;	
			case 5: fprintf(output_stream, "ch"); break;	
			case 6: fprintf(output_stream, "dh"); break;	
			case 7: fprintf(output_stream, "bh"); break;	
		}
	} else if (w == 1) {
		switch (reg) {
			case 0: fprintf(output_stream, "ax"); break;	
			case 1: fprintf(output_stream, "cx"); break;	
			case 2: fprintf(output_stream, "dx"); break;	
			case 3: fprintf(output_stream, "bx"); break;	
			case 4: fprintf(output_stream, "sp"); break;	
			case 5: fprintf(output_stream, "bp"); break;	
			case 6: fprintf(output_stream, "si"); break;	
			case 7: fprintf(output_stream, "di"); break;	
		}
	}
}

void print_direct_address(uint16_t reg, uint16_t data, FILE * output_stream,print_imediate_length print) {
	if (print == PRINT) {
		fprintf(output_stream, "word ");
	} 
	fprintf(output_stream, "[%d]", data);
}

void print_acc(FILE * output_stream) {
	fprintf(output_stream, "ax");
}


void print_im_8(uint16_t data, FILE * output_stream, print_imediate_length print) {
	if (print == PRINT) {
		fprintf(output_stream, "byte %d", data);
	} else {
		fprintf(output_stream, "%d", data);
	}

}

void print_im_16(uint16_t data, FILE * output_stream, print_imediate_length print) {
	if (print == PRINT) {
		fprintf(output_stream, "word %d", data);
	} else {
		fprintf(output_stream, "%d", data);
	}
}

void print_ip(uint16_t print_data, FILE * output_stream) {
	fprintf(output_stream, "%d", (int16_t)print_data);
}

void print_ip_8(uint16_t print_data, FILE * output_stream) {
	fprintf(output_stream, "%d", (int8_t)print_data);
}

void print_ip_16(uint16_t print_data, FILE * output_stream) {
	fprintf(output_stream, "%d", (int16_t)print_data);
}

void print_mem(uint16_t reg, FILE * output_stream,print_imediate_length print) {
	if (print == PRINT) {
		fprintf(output_stream, "word ");
	} 

	switch (reg) {
		case 0: fprintf(output_stream, "[bx + si]"); break;	
		case 1: fprintf(output_stream, "[bx + di]"); break;	
		case 2: fprintf(output_stream, "[bp + si]"); break;	
		case 3: fprintf(output_stream, "[bp + di]"); break;	
		case 4: fprintf(output_stream, "[si]"); break;	
		case 5: fprintf(output_stream, "[di]"); break;	
		case 6: fprintf(output_stream, "error"); break;	
		case 7: fprintf(output_stream, "[bx]"); break;	
	}
}

void print_seg(uint16_t seg, FILE * output_stream) {
	switch (seg) {
		case 0: fprintf(output_stream, "es"); break;	
		case 1: fprintf(output_stream, "cs"); break;	
		case 2: fprintf(output_stream, "ss"); break;	
		case 3: fprintf(output_stream, "ds"); break;	
	}
}

void print_mem_8(uint16_t reg, uint16_t data, FILE * output_stream,print_imediate_length print) {
	if (print == PRINT) {
		fprintf(output_stream, "byte ");
	} 

	char signed_data = (char) data;	

	//THESE PRINTS MAY BE PRODUCING WEIRD VALUES BECAUSE i AM USING UINT8 AND NOT BOTHERING WITH A SIGN
	///I THINK THE LOGIC IS RIGHT, IF ANYTHING MASSAGE THE TYPES AND PRINT TYPES

	switch (reg) {
		case 0: fprintf(output_stream, "[bx + si"); break;	
		case 1: fprintf(output_stream, "[bx + di"); break;	
		case 2: fprintf(output_stream, "[bp + si"); break;	
		case 3: fprintf(output_stream, "[bp + di"); break;	
		case 4: fprintf(output_stream, "[si"); break;	
		case 5: fprintf(output_stream, "[di"); break;	
		case 6: fprintf(output_stream, "[bp"); break;	
		case 7: fprintf(output_stream, "[bx"); break;	
	}

	if (signed_data > 0) {
		fprintf(output_stream, " + %d]", signed_data);
	} else if (signed_data < 0){
		fprintf(output_stream, " - %d]", -signed_data);
	} else {
		fprintf(output_stream, "]");
	}
}

void print_mem_16(uint16_t reg, uint16_t data, FILE * output_stream, print_imediate_length print) {
	int16_t signed_data = data;	

	if (print == PRINT) {
		fprintf(output_stream, "word ");
	} 
	//THESE PRINTS MAY BE PRODUCING WEIRD VALUES BECAUSE i AM USING UINT8 AND NOT BOTHERING WITH A SIGN
	///I THINK THE LOGIC IS RIGHT, IF ANYTHING MASSAGE THE TYPES AND PRINT TYPES

	switch (reg) {
		case 0: fprintf(output_stream, "[bx + si"); break;	
		case 1: fprintf(output_stream, "[bx + di"); break;	
		case 2: fprintf(output_stream, "[bp + si"); break;	
		case 3: fprintf(output_stream, "[bp + di"); break;	
		case 4: fprintf(output_stream, "[si"); break;	
		case 5: fprintf(output_stream, "[di"); break;	
		case 6: fprintf(output_stream, "[bp"); break;	
		case 7: fprintf(output_stream, "[bx"); break;	
	}

	if (signed_data > 0) {
		fprintf(output_stream, " + %d]", signed_data);
	} else if (signed_data < 0){
		fprintf(output_stream, " - %d]", -signed_data);
	} else {
		fprintf(output_stream, "]");
	}
}

void print_instruction_half(instruction * new_instruction, FILE * output_stream, print_type print_target) {
	target print_type; 	
	target opposite_type; 	
	uint16_t print_register;
	uint16_t print_data;

	if ((new_instruction->order == ARG_1_SOURCE && print_target == SOURCE) || (new_instruction->order == ARG_2_SOURCE && print_target == DEST)) {
		print_type = new_instruction->arg_one_type;
		opposite_type = new_instruction->arg_two_type; 	
		print_register = new_instruction->register_one;
		print_data= new_instruction->data_one;
	} else {
		print_type = new_instruction->arg_two_type;
		opposite_type = new_instruction->arg_one_type; 	
		print_register = new_instruction->register_two;
		print_data= new_instruction->data_two;
	}

	//little something to determine if the imediate register's length must be printed.
	print_imediate_length print_length_switch = NO_PRINT;	

	if (print_target == SOURCE && (opposite_type == DIRECT || opposite_type== MEM || opposite_type == MEM_8 || opposite_type == MEM_16) && (print_type == IM8 || print_type == IM16)) {
		print_length_switch  = PRINT;	
	} else if (print_target == SOURCE && (print_type == MEM || print_type == MEM_8 || print_type == MEM_16 || print_type == DIRECT) && (opposite_type == NONE)) {
		print_length_switch  = PRINT;	
	}

	switch (print_type) {
		case REG: print_reg(print_register, print_data, new_instruction->w, output_stream); break;
		case MEM: print_mem(print_register, output_stream, print_length_switch); break;
		case MEM_8: print_mem_8(print_register, print_data, output_stream, print_length_switch); break;
		case MEM_16: print_mem_16(print_register, print_data, output_stream, print_length_switch); break;
		case DIRECT: print_direct_address(print_register, print_data, output_stream, print_length_switch); break;
		case IM8: print_im_8(print_data, output_stream, print_length_switch); break;
		case IM16: print_im_16(print_data, output_stream, print_length_switch); break;
		case ACC: print_acc(output_stream); break;
		case SEG: print_seg(print_register, output_stream); break;
		case CL: print_cl(output_stream); break;
		case IP: print_ip(print_data, output_stream); break;
		case IP_INC8: print_ip_8(print_data, output_stream); break;
		case IP_INC16: print_ip_16(print_data, output_stream); break;
		case NONE: break;
	}	

}

void print_two_arg_instruction(instruction_type type,instruction * new_instruction, FILE * output_stream) {
	switch (type) {
		case MOV: fprintf(output_stream, "mov "); break;
		case XCHG: fprintf(output_stream, "xchg "); break;
		case ADD: fprintf(output_stream, "add "); break;
		case ADC: fprintf(output_stream, "adc "); break;
		case SUB: fprintf(output_stream, "sub "); break;
		case SBB: fprintf(output_stream, "sbb "); break;
		case AND: fprintf(output_stream, "and "); break;
		case TEST: fprintf(output_stream, "test "); break;
		case CMP: fprintf(output_stream, "cmp "); break;
		case OR: fprintf(output_stream, "or "); break;
		case XOR: fprintf(output_stream, "xor "); break;
		case MUL: fprintf(output_stream, "mul "); break;
		case IMUL: fprintf(output_stream, "imul "); break;
		case DIV: fprintf(output_stream, "div "); break;
		case IDIV: fprintf(output_stream, "idiv "); break;
		case LEA: fprintf(output_stream, "lea "); break;
		case LDS: fprintf(output_stream, "lds "); break;
		case LES: fprintf(output_stream, "les "); break;
		default: printf("Invalid print.\n"); exit(0);
	}


	print_instruction_half(new_instruction, output_stream, DEST);
	fprintf(output_stream, ", ");
	print_instruction_half(new_instruction, output_stream, SOURCE);

	fprintf(output_stream, "\n");
}

void print_one_arg_instruction(instruction_type type,instruction * new_instruction, FILE * output_stream) {
	switch (type) {
		case POP: fprintf(output_stream, "pop "); break;
		case PUSH: fprintf(output_stream, "push "); break;
		case INC: fprintf(output_stream, "inc "); break;
		case DEC: fprintf(output_stream, "dec "); break;
		case NEG: fprintf(output_stream, "neg "); break;
		case AAM: fprintf(output_stream, "aam "); break;
		case AAD: fprintf(output_stream, "aad "); break;
	
		//most of this vm is just ways to jump.
		case JE: fprintf(output_stream, "pop "); break;
		case JL: fprintf(output_stream, "push "); break;
		case JLE: fprintf(output_stream, "inc "); break;
		case JB: fprintf(output_stream, "dec "); break;
		case JBE: fprintf(output_stream, "neg "); break;
		case JP: fprintf(output_stream, "aam "); break;
		case JO: fprintf(output_stream, "aad "); break;
		case JS: fprintf(output_stream, "pop "); break;
		case JNE: fprintf(output_stream, "push "); break;
		case JNL: fprintf(output_stream, "inc "); break;
		case JNLE: fprintf(output_stream, "dec "); break;
		case JNB: fprintf(output_stream, "neg "); break;
		case JNBE: fprintf(output_stream, "aam "); break;
		case JNP: fprintf(output_stream, "aad "); break;
		case JNO: fprintf(output_stream, "dec "); break;
		case JNS: fprintf(output_stream, "neg "); break;
		case LOOP: fprintf(output_stream, "aam "); break;
		case LOOPZ: fprintf(output_stream, "aad "); break;
		case LOOPNZ: fprintf(output_stream, "aad "); break;
		case JCXZ: fprintf(output_stream, "aad "); break;
		
		case CALL: fprintf(output_stream, "call "); break;
		case JMP: fprintf(output_stream, "jmp "); break;

		default: printf("Invalid print.\n"); exit(0);
	}

	print_instruction_half(new_instruction, output_stream, SOURCE);

	fprintf(output_stream, "\n");
}


void print_v_arg_instruction(instruction_type type,instruction * new_instruction, FILE * output_stream) {
	switch (type) {
		case NOT: fprintf(output_stream, "not "); break;
		case SHL: fprintf(output_stream, "shl "); break;
		case SHR: fprintf(output_stream, "shr "); break;
		case SAR: fprintf(output_stream, "sar "); break;
		case ROL: fprintf(output_stream, "rol "); break;
		case ROR: fprintf(output_stream, "ror "); break;
		case RCL: fprintf(output_stream, "rcl "); break;
		case RCR: fprintf(output_stream, "rcr "); break;
		default: printf("Invalid print.\n"); exit(0);
	}

	//the destinations and sources here could easily be wrong 
	if (new_instruction->v == 1) {
		print_instruction_half(new_instruction, output_stream, SOURCE);
		fprintf(output_stream, ", ");
		print_instruction_half(new_instruction, output_stream, DEST);
	} else {
		print_instruction_half(new_instruction, output_stream, SOURCE);
	}

	fprintf(output_stream, "\n");
}

void print_special_instruction(instruction_type type,instruction * new_instruction, FILE * output_stream) {
	switch (type) {
		case REP: fprintf(output_stream, "rep "); break;
		case MOVS: fprintf(output_stream, "movs "); break;
		case CMPS: fprintf(output_stream, "cmps "); break;
		case SCAS: fprintf(output_stream, "scas "); break;
		case LODS: fprintf(output_stream, "lods "); break;
		case STDS: fprintf(output_stream, "stds "); break;
		case AAA: fprintf(output_stream, "aaa "); break;
		case DAA: fprintf(output_stream, "daa "); break;
		case AAS: fprintf(output_stream, "aas "); break;
		case DAS: fprintf(output_stream, "das "); break;
		case CBW: fprintf(output_stream, "cbw"); break;
		case CWD: fprintf(output_stream, "cwd "); break;
		case XLAT: fprintf(output_stream, "xlat\n"); break;
		case LAHF: fprintf(output_stream, "lahf\n"); break;
		case SAHF: fprintf(output_stream, "sahf\n"); break;
		case PUSHF: fprintf(output_stream, "pushf\n"); break;
		case POPF: fprintf(output_stream, "popf\n"); break;
		default: printf("Instruction not recognized as special. Exiting. \n"); exit(0); 
	}
}

void print_one_or_zero_arg1(instruction_type type, instruction * new_instruction , FILE * output_stream) {
	switch (type) {
		case RET: fprintf(output_stream, "ret ");
		case IN: fprintf(output_stream, "in");
		case OUT: fprintf(output_stream, "out ");
		default: printf("Instruction not recognized as special. Exiting. \n"); exit(0);
	}

	if (new_instruction ->arg_one_type != NONE) {
		print_instruction_half(new_instruction, output_stream, SOURCE);
	} 

	fprintf(output_stream, "\n");
}
