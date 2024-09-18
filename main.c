#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

typedef enum {MOV} instruction_type;
typedef enum {RM_RM, I_RM, I_R, M_A, A_M, RM_S, SR_RM} instruction_subtype;
typedef enum {REG, REG2, MEM, IM, SR, ACC, DMEM} target;

typedef struct instruction {
	instruction_type type;
	instruction_subtype subtype;
	int direction;
	int word;
	int mod; 
	int rm;
	int reg;
	int displacement;
	int imediate_value;
	int SR;
	//the following are derrived items
	target source;
	target destination;
} instruction;

void decode(FILE * assembly_file, FILE * output_file);
void decode_instruction_type(instruction *new_instruction, int first_byte, FILE * assembly_file);
int match_instructions(int byte, int count, int value);
int get_bit(int byte, int mask, int shift); //carefull 0 is the least significant bit 
int get_next_byte(FILE * assembly_file);
int get_highest_two_bits(int byte);
int get_middle_three_bits(int byte);
int get_lowest_three_bits(int byte);
int get_middle_two_bits(int byte);
int read_address_byte(int byte, FILE * assembly_file);
int read_data_byte(int byte, int w, FILE * assembly_file);
void read_mem_displacement(instruction *current_instruction,FILE * assembly_file);
void print_register(instruction * current_instruction, FILE * output_file, int reg_number);
void print_value(instruction * current_instruction, FILE * output_file);
void print_segment_register(instruction * current_instruction, FILE * output_file);
void print_memory(instruction * current_instruction, FILE * output_file);
void print_mov_instruction(instruction * current_instruction, FILE * output_file);
void print_instruction(instruction * current_instruction, FILE * output_file);
void print_accumilator(instruction * current_instruction, FILE * output_file);
void print_direct_mem(instruction * current_instruction, FILE * output_file);
void print_from_target(target target_type, instruction * current_instruction, FILE * output_file);
void set_instruction_type(instruction * new_instruction, instruction_type type, instruction_subtype subtype );
void split_mod_sr_rm(instruction * new_instruction, int second_byte);
void split_mod_reg_rm(instruction * new_instruction, int second_byte);

void decode_instruction_type(instruction *new_instruction, int first_byte, FILE * assembly_file) {
	//mov reg/mem to reg/mem
	if (match_instructions(first_byte, 6, 0b00100010)) { 
		set_instruction_type(new_instruction, MOV, RM_RM); 
		new_instruction->direction = get_bit(first_byte, 2,1);
		new_instruction->word = get_bit(first_byte, 1,0);

		int second_byte = get_next_byte(assembly_file);
		split_mod_reg_rm(new_instruction, second_byte);

		//determine if the last two data bits will be read or something.
		read_mem_displacement(new_instruction,assembly_file);

		//this branch statement is unique to this subtype
		if (new_instruction->direction == 0) {
			//reg is source s
			new_instruction->source = REG;
			new_instruction->destination = new_instruction->mod == 3 ? REG2 : MEM;
		} else {
			new_instruction->source = new_instruction->mod == 3 ? REG2 : MEM;
			new_instruction->destination = REG;
		}

	//mov imediate to reg/mem
	} else if (match_instructions(first_byte, 7, 0b01100011)) {
		set_instruction_type(new_instruction, MOV, I_RM);
		new_instruction->word = get_bit(first_byte, 1,0);
		
		int second_byte = get_next_byte(assembly_file);
		new_instruction->mod = get_highest_two_bits(second_byte);
		new_instruction->rm= get_lowest_three_bits(second_byte);
		
		//determine if the last two data bits will be read or something.
		read_mem_displacement(new_instruction,assembly_file);

		//this case can actually have another couple of direct data bytes. Crazy ass instruction to be honest
		int first_data_byte = get_next_byte(assembly_file); 
		new_instruction->imediate_value = read_data_byte(first_data_byte, new_instruction->word, assembly_file);
			
		new_instruction->source = IM;
		new_instruction->destination = new_instruction->mod == 3 ? REG2 : MEM;

	//mov imediate to register
	} else if (match_instructions(first_byte, 4, 0b00001011)) {
		set_instruction_type(new_instruction, MOV, I_R);
		new_instruction->word = get_bit(first_byte, 8,3);
		new_instruction->reg = get_lowest_three_bits(first_byte);

		//read data... this one is next
		int second_byte = get_next_byte(assembly_file); 
		new_instruction->imediate_value = read_data_byte(second_byte, new_instruction->word, assembly_file);

		new_instruction->source = IM;
		new_instruction->destination = REG;

	//mov memory to accumilator
	} else if (match_instructions(first_byte, 7, 0b01010000)) {
		set_instruction_type(new_instruction, MOV, M_A);
		new_instruction->word = get_bit(first_byte, 1,0);

		//read address
		int second_byte = get_next_byte(assembly_file); 
		new_instruction->imediate_value = read_address_byte(second_byte, assembly_file);

		new_instruction->source = DMEM;
		new_instruction->destination = ACC;

	//mov accumilator to memory 
	} else if (match_instructions(first_byte, 7, 0b01010001)) {
		set_instruction_type(new_instruction, MOV, A_M);
		new_instruction->word = get_bit(first_byte, 1,0);

		//read address
		int second_byte = get_next_byte(assembly_file); 
		new_instruction->imediate_value = read_address_byte(second_byte, assembly_file);
		
		new_instruction->source = ACC;
		new_instruction->destination = DMEM;
	
	//mov reg/mem to segment register
	} else if (match_instructions(first_byte, 8, 0b10001110)) {
		set_instruction_type(new_instruction, MOV, RM_S);

		int second_byte = get_next_byte(assembly_file);
		split_mod_sr_rm(new_instruction, second_byte);

		//determine if the last two data bits will be read or something.
		read_mem_displacement(new_instruction,assembly_file);
		
		new_instruction->source = new_instruction->mod == 3 ? REG2 : MEM;
		new_instruction->destination = SR;

	//mov segment register to reg/mem
	} else if (match_instructions(first_byte, 8, 0b10001100)) {
		set_instruction_type(new_instruction, MOV, SR_RM);

		int second_byte = get_next_byte(assembly_file);
		split_mod_sr_rm(new_instruction, second_byte);
		
		//determine if the last two data bits will be read or something.
		read_mem_displacement(new_instruction,assembly_file);
		
		new_instruction->destination= new_instruction->mod == 3 ? REG2 : MEM;
		new_instruction->source = SR;
		
	} else {
		printf("Encountered an unknown instruction code.\n");
		exit(0);
	}
}

void decode(FILE * assembly_file, FILE * output_file) {

	int first_byte;

	if ((first_byte = getc(assembly_file)) == EOF) {
		return;
	}
	
	instruction *new_instruction = calloc(1,sizeof(instruction));

	decode_instruction_type(new_instruction,first_byte,assembly_file);
	print_instruction(new_instruction, output_file);

	//question. Can this thing now be turned into a string
	free(new_instruction);
	decode(assembly_file, output_file);

}

void print_register(instruction * current_instruction, FILE * output_file, int reg_number) {

	int register_source;

	if (reg_number == 1) {
		register_source = current_instruction->reg;
	} else {
		register_source = current_instruction->rm;
	}

	switch (current_instruction->word) {
		case 0:
			switch (register_source) {//current_instruction->reg) {
				case 0:	fprintf(output_file, "AL"); break;
				case 1:	fprintf(output_file, "CL"); break;
				case 2:	fprintf(output_file, "DL"); break;
				case 3:	fprintf(output_file, "BL"); break;
				case 4:	fprintf(output_file, "AH"); break;
				case 5:	fprintf(output_file, "CH"); break;
				case 6:	fprintf(output_file, "DH"); break;
				case 7:	fprintf(output_file, "BH"); break;
				default:printf("Invalid register contents. Exiting.\n"); exit(0); 
			}
			break;
		case 1:
			switch (register_source) { //current_instruction->reg) {
				case 0:	fprintf(output_file, "AX"); break;
				case 1:	fprintf(output_file, "CX"); break;
				case 2:	fprintf(output_file, "DX"); break;
				case 3:	fprintf(output_file, "BX"); break;
				case 4:	fprintf(output_file, "SP"); break;
				case 5:	fprintf(output_file, "BP"); break;
				case 6:	fprintf(output_file, "SI"); break; //kind of unclear if these mean SI or DI
				case 7:	fprintf(output_file, "DI"); break;
				default:printf("Invalid register contents. Exiting.\n"); exit(0); 
			}
			break;
		default:
			printf("Invalid word value. Exiting.\n");
			break;
	}		
}

void print_value(instruction * current_instruction, FILE * output_file) {
	//unsure if this thing should go in brackets or something
	if (current_instruction->destination == MEM) {
		//print word or byte	
		if (current_instruction->word == 1) {
			fprintf(output_file,"word "); 
		} else {
			fprintf(output_file,"byte "); 
		}
	} 

	fprintf(output_file,"%d",current_instruction->imediate_value); 
}

void print_direct_mem(instruction * current_instruction, FILE * output_file) {
	fprintf(output_file,"[%d]",current_instruction->imediate_value); 
}

void print_memory(instruction * current_instruction, FILE * output_file) {

	char plus = '+';

	switch (current_instruction->mod) {
		case 0: 
			switch (current_instruction->rm) {
				case 0: fprintf(output_file, "[BX + SI]"); break;
				case 1: fprintf(output_file, "[BX + DI]"); break;
				case 2: fprintf(output_file, "[BP + SI]"); break;
				case 3: fprintf(output_file, "[BP + DI]"); break;
				case 4: fprintf(output_file, "[SI]"); break;
				case 5: fprintf(output_file, "[DI]"); break;
				case 6: fprintf(output_file, "[%d]", current_instruction->displacement); break;
				case 7: fprintf(output_file, "[BX]"); break;
			}
		break;
		case 1:
		case 2:
			if ((int16_t)current_instruction->displacement < 0) {plus = ' ';}
			//8 and 16 bit value variants are functionally identical
			switch (current_instruction->rm) {
				case 0: fprintf(output_file, "[BX + SI %c%hd]", plus, current_instruction->displacement); break;
				case 1: fprintf(output_file, "[BX + DI %c%hd]",plus,  current_instruction->displacement); break;
				case 2: fprintf(output_file, "[BP + SI %c%hd]",plus,  current_instruction->displacement); break;
				case 3: fprintf(output_file, "[BP + DI %c%hd]",plus,  current_instruction->displacement); break;
				case 4: fprintf(output_file, "[SI %c%hd]", plus, current_instruction->displacement); break;
				case 5: fprintf(output_file, "[DI %c%hd]",plus,  current_instruction->displacement); break;
				case 6: fprintf(output_file, "[BP %c%hd]",plus,  current_instruction->displacement); break;
				case 7: fprintf(output_file, "[BX %c%hd]", plus, current_instruction->displacement); break;
				default:printf("Invalid RM value. Exiting.\n"); exit(0);
			}
		break;
		default:
			printf("Invalid MOD found. Exiting.\n");
			exit(0);
	}	
}

void print_segment_register(instruction * current_instruction, FILE * output_file) {
	switch (current_instruction->SR) {
		case 0: fprintf(output_file,"ES"); break;
		case 1: fprintf(output_file,"CS"); break;
		case 2: fprintf(output_file,"SS"); break;
		case 3: fprintf(output_file,"DS"); break;
		default: fprintf(output_file, "Invalid SR value. Exiting. \n"); exit(0);
	}
}

void print_accumilator(instruction * current_instruction, FILE * output_file) {
	//simply do not know enought about this kind of assembly to know if AH and AL are accessible in this instruction.
	fprintf(output_file,"AX");
}

void print_from_target(target target_type, instruction * current_instruction, FILE * output_file) {
	switch (target_type) {
		case REG: print_register(current_instruction, output_file, 1); break;
		case REG2: print_register(current_instruction, output_file, 2); break;
		case MEM: print_memory(current_instruction, output_file); break;
		case DMEM: print_direct_mem(current_instruction, output_file); break;
		case IM: print_value(current_instruction, output_file); break;
		case SR: print_segment_register(current_instruction, output_file); break;
		case ACC: print_accumilator(current_instruction, output_file); break;
		default: printf("Invalid instruction target type. Exiting.\n"); exit(0);
	}	
}

void print_mov_instruction(instruction * current_instruction, FILE * output_file) {
	fprintf(output_file, "mov ");
	print_from_target(current_instruction->destination, current_instruction, output_file);
	fprintf(output_file, ", ");
	print_from_target(current_instruction->source, current_instruction, output_file);
	fprintf(output_file, "\n");
}

//usefull for mod
int get_highest_two_bits(int byte) {
	return get_bit(byte, 0b11000000, 6);
}

//usefull for reg 
int get_middle_three_bits(int byte) {
	return get_bit(byte, 0b00111000, 3);
} 

//usefull for rm 
int get_lowest_three_bits(int byte) {
	return get_bit(byte, 0b00000111,0);
} 

//usefull for sr
int get_middle_two_bits(int byte) {
	return get_bit(byte, 0b00011000,3);
}

//reads data byte, grabs additional byte if w == 1
int read_data_byte(int byte, int w, FILE * assembly_file) {
	if (w == 1) {
		//gotta read another byte and return a proper two byte int
		int second_byte = get_next_byte(assembly_file);
		return byte + (second_byte << 8); //this is suspect. also not clear if endianness will cause errors
	} else {
		//return a single byte int 
		return byte;
	}
}

//really only for direct addressing as far as I understand 
int read_address_byte(int byte, FILE * assembly_file) {
	//gotta read another byte and return a proper two byte int
	int second_byte = get_next_byte(assembly_file);
	return byte + (second_byte << 8); //this is suspect. also not clear if endianness will cause errors
}

void read_mem_displacement(instruction *current_instruction,FILE * assembly_file) {
	switch (current_instruction->mod) {
		case 0:
			//if RM is 110, 16 bit displacement else none.
			if (current_instruction->rm == 6) {
				current_instruction ->displacement =  get_next_byte(assembly_file) + (get_next_byte(assembly_file) << 8);
			}
			break;  
		case 1: 
			//8 bit displacement follows -- I belive that 8 bit values need a bit of help to get signs right
			current_instruction ->displacement = (int)(char)get_next_byte(assembly_file);
			break;
		case 2:
			//16 bit displacement follows
			current_instruction ->displacement =  get_next_byte(assembly_file) + (get_next_byte(assembly_file) << 8);
			break;
			//this math is a little bit suss
		case 3:
			break; //register mode (no disp)
		default:
			printf("Invalid memory mod field. Exiting.\n");
			exit(0);
	}
}

void set_instruction_type(instruction * new_instruction, instruction_type type, instruction_subtype subtype ) {
	new_instruction->type = type;
	new_instruction->subtype = subtype;
}

void split_mod_reg_rm(instruction * new_instruction, int second_byte) {
	new_instruction->mod = get_highest_two_bits(second_byte);
	new_instruction->reg= get_middle_three_bits(second_byte);
	new_instruction->rm= get_lowest_three_bits(second_byte);
}

void split_mod_sr_rm(instruction * new_instruction, int second_byte) {
	new_instruction->mod = get_highest_two_bits(second_byte);
	new_instruction->SR= get_middle_two_bits(second_byte);
	new_instruction->rm= get_lowest_three_bits(second_byte);
}

void print_instruction(instruction * current_instruction, FILE * output_file) {
	switch (current_instruction->type) {
		case (MOV):
			print_mov_instruction(current_instruction,output_file);
			break;
		default:
			printf("Invalid instruction type. Exiting.\n");
	}
}

int get_bit(int byte, int mask, int shift) { //this feels a bit too easy
	return (byte & mask) >> shift;	
}

int match_instructions(int byte, int count, int value) {
	if (byte >> (8-count) == value) {
		return 1;
	}
	return 0;
}

int get_next_byte(FILE * assembly_file) {
	int next_byte;
	if ((next_byte= getc(assembly_file)) == EOF) {
		printf("Invalid instruction stream, EOF encountered before instruction completed.\n");
		exit(0);
	}
	return next_byte;
}

int main(int argc, char * argv []) {

	if (argc != 2 && argc != 3) {
		printf("Usage: disassembler <filename> <output - optional>\n");
		exit(0);
	}

	FILE * assembly_file= fopen(argv[1], "r");

	if (assembly_file == NULL) {
		printf("Could not open file: %s.\n", argv[1]);
		exit(0);
	}

	FILE * output_stream = stdout;

	if (argc == 3) {
		output_stream = fopen(argv[2], "w");
	}

	if (output_stream == NULL) {
		printf("Could not open file: %s.\n", argv[2]);
		exit(0);
	}
	
	fprintf(output_stream,";NASM assembly file.\n");
	fprintf(output_stream,"bits 16\n\n");

	decode(assembly_file, output_stream);

	fclose(assembly_file);
	fclose(output_stream); //this may be sketchy but seems to work fine.

}


