#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

typedef enum {MOV} instruction_type;
typedef enum {RM_RM, I_RM, I_R, M_A, A_M, RM_S, SR_RM} instruction_subtype;
//how to read these: R-register, m-memory, i-imediate, a-accumilator, s-segment
//left is source, right is dest, multiple letters mean muliple options

//damn, this instruction set is wild
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
	char * text_form;
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
void print_register(instruction * current_instruction, FILE * output_file);
void print_value(instruction * current_instruction, FILE * output_file);
void print_segment_register(instruction * current_instruction, FILE * output_file);
void print_memory(instruction * current_instruction, FILE * output_file);
void print_mov_instruction(instruction * current_instruction, FILE * output_file);
void print_instruction(instruction * current_instruction, FILE * output_file);


void print_mov_instruction(instruction * current_instruction, FILE * output_file) {
	fprintf(output_file, "mov ");
	//figure out dest.
	fprintf(output_file, ", ");
	//figure out source.
	fprintf(output_file, "\n");
}
//this thing is a total pain. probably need a pretty intense function that can actually tell what is happening. potentially a really big switch statement 

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
			//8 bit displacement follows
			current_instruction ->displacement = get_next_byte(assembly_file);
		case 2:
			//16 bit displacement follows
			current_instruction ->displacement =  get_next_byte(assembly_file) + (get_next_byte(assembly_file) << 8);
			//this math is a little bit suss
		case 3:
			break; //register mode (no disp)
		default:
			printf("Invalid memory mod field. Exiting.\n");
			exit(0);
	}
}

void decode_instruction_type(instruction *new_instruction, int first_byte, FILE * assembly_file) {
	if (match_instructions(first_byte, 6, 0b00100010)) {
		new_instruction->type = MOV;
		new_instruction->subtype = RM_RM;
		new_instruction->direction = get_bit(first_byte, 2,1);
		new_instruction->word = get_bit(first_byte, 1,0);

		int second_byte = get_next_byte(assembly_file);
		new_instruction->mod = get_highest_two_bits(second_byte);
		new_instruction->reg= get_middle_three_bits(second_byte);
		new_instruction->rm= get_lowest_three_bits(second_byte);

		//determine if the last two data bits will be read or something.
		read_mem_displacement(new_instruction,assembly_file);
			

	} else if (match_instructions(first_byte, 7, 0b01100011)) {
		new_instruction->type = MOV;
		new_instruction->subtype = I_RM;
		new_instruction->word = get_bit(first_byte, 1,0);
		
		int second_byte = get_next_byte(assembly_file);
		new_instruction->mod = get_highest_two_bits(second_byte);
		new_instruction->rm= get_lowest_three_bits(second_byte);
		
		//determine if the last two data bits will be read or something.
		read_mem_displacement(new_instruction,assembly_file);

		//this case can actually have another couple of direct data bytes. Crazy ass instruction to be honest
		int first_data_byte = get_next_byte(assembly_file); 
		new_instruction->imediate_value = read_data_byte(first_data_byte, new_instruction->word, assembly_file);

	} else if (match_instructions(first_byte, 4, 0b00001011)) {
		new_instruction->type = MOV;
		new_instruction->subtype = I_R;
		new_instruction->word = get_bit(first_byte, 8,3);
		new_instruction->reg = get_lowest_three_bits(first_byte);

		//read data... this one is next
		int second_byte = get_next_byte(assembly_file); 
		new_instruction->imediate_value = read_data_byte(second_byte, new_instruction->word, assembly_file);

	} else if (match_instructions(first_byte, 7, 0b01010000)) {
		new_instruction->type = MOV;
		new_instruction->subtype = M_A;
		new_instruction->word = get_bit(first_byte, 1,0);

		//read address
		int second_byte = get_next_byte(assembly_file); 
		new_instruction->imediate_value = read_address_byte(second_byte, assembly_file);


	} else if (match_instructions(first_byte, 7, 0b01010001)) {
		new_instruction->type = MOV;
		new_instruction->subtype = A_M;
		new_instruction->word = get_bit(first_byte, 1,0);

		//read address
		int second_byte = get_next_byte(assembly_file); 
		new_instruction->imediate_value = read_address_byte(second_byte, assembly_file);

	} else if (match_instructions(first_byte, 8, 0b10001110)) {
		new_instruction->type = MOV;
		new_instruction->subtype = RM_S;

		int second_byte = get_next_byte(assembly_file);
		new_instruction->mod = get_highest_two_bits(second_byte);
		new_instruction->SR= get_middle_two_bits(second_byte);
		new_instruction->rm= get_lowest_three_bits(second_byte);

		//determine if the last two data bits will be read or something.
		read_mem_displacement(new_instruction,assembly_file);
		
	} else if (match_instructions(first_byte, 8, 0b10001100)) {
		new_instruction->type = MOV;
		new_instruction->subtype = SR_RM;

		int second_byte = get_next_byte(assembly_file);
		new_instruction->mod = get_highest_two_bits(second_byte);
		new_instruction->SR= get_middle_two_bits(second_byte);
		new_instruction->rm= get_lowest_three_bits(second_byte);
		
		//determine if the last two data bits will be read or something.
		read_mem_displacement(new_instruction,assembly_file);
		
	} else {
		printf("Encountered an unknown instruction code.\n");
		exit(0);
	}
}

void decode(FILE * assembly_file, FILE * output_file) {

	instruction *new_instruction = calloc(1,sizeof(instruction));
	int first_byte;

	if ((first_byte = getc(assembly_file)) == EOF) {
		return;
	}

	decode_instruction_type(new_instruction,first_byte,assembly_file);
	//question. Can this thing now be turned into a string

	free(new_instruction);

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

void print_register(instruction * current_instruction, FILE * output_file) {
	switch (current_instruction->word) {
		case 0:
			switch (current_instruction->reg) {
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
		case 1:
			switch (current_instruction->reg) {
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
		default:
			printf("Invalid word value. Exiting.\n");
			break;
	}		
}

void print_value(instruction * current_instruction, FILE * output_file) {
	//unsure if this thing should go in brackets or something
	fprintf(output_file,"%d",current_instruction->imediate_value); 
}

void print_memory(instruction * current_instruction, FILE * output_file) {
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
			//8 and 16 bit value variants are functionally identical
			switch (current_instruction->rm) {
				case 0: fprintf(output_file, "[BX + SI + %d]", current_instruction->displacement); break;
				case 1: fprintf(output_file, "[BX + DI + %d]", current_instruction->displacement); break;
				case 2: fprintf(output_file, "[BP + SI + %d]", current_instruction->displacement); break;
				case 3: fprintf(output_file, "[BP + DI + %d]", current_instruction->displacement); break;
				case 4: fprintf(output_file, "[SI + %d]", current_instruction->displacement); break;
				case 5: fprintf(output_file, "[DI + %d]", current_instruction->displacement); break;
				case 6: fprintf(output_file, "[BP + %d]", current_instruction->displacement); break;
				case 7: fprintf(output_file, "[BX + %d]", current_instruction->displacement); break;
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


