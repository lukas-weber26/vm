#include <locale.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

//OVERALL, I THINK THIS IS THE VERION THAT I AM HAPPY TO STICK WITH. ALL THE CORE FEATURES ARE THERE AND I DON'T THINK THERE ARE OBVIOUS DESTRUCTIVE ISSUES GOING ON. 
//I WANT TO SPLIT PRINTING, DECODING, AND RUNNING INTO SEPERATE FILES ONCE POSSIBLE
//I SUSPECT THAT THERE ARE SOME ISSUES WITH DATA TYPES. SPECIFICALLY SIGNS ON THE SHORTER INTEGER TYPES
//ALSO PRINGTING THE SHORT INTEGER TYPES IS WEIRD.
//NEXT STEP IS MAKING SURE THAT THE FIRST INSTRUCTION IS FUNCTIONAL IN ALL THE WAYS IT HAS TO BE 
//AFTER THAT ADD AS MANY INSTRUCTIONS AS YOU CAN.
//THIS SHOULD BE A PRETTY SOLID API.

typedef enum {MOV} instruction_type; //,PUSH, POP
typedef enum {MEM, MEM_8, MEM_16, REG, SEG, ACC, IM8, IM16, DIRECT} target;
typedef enum {EXIT_ON_EOF, ERROR_ON_EOF} read_behaviour;
typedef enum {INVALID = 0, VALID = 1} validity;
typedef enum {NON_INVERTED, INVERTED} source_inversion;
typedef enum {NA,ARG_1_SOURCE, ARG_2_SOURCE} arg_order;
typedef enum {SOURCE, DEST} print_type;

typedef struct instruction_byte {
	uint8_t byte;
	validity valid;
} instruction_byte;

typedef struct instruction_stream {
	instruction_byte instruction_bytes[6];
} instruction_stream;

typedef struct instruction {
	instruction_type type;
	arg_order order;	

	target arg_one_type; 	
	target arg_two_type; 
	
	uint16_t register_one;
	uint16_t register_two;

	uint16_t data_one;
	uint16_t data_two;

	uint16_t mod;
	uint16_t d;
	uint16_t w;
	uint16_t s;
	uint16_t v;
	uint16_t z;
} instruction;

void decode(instruction_stream * instructions, FILE * assembly_file, FILE * output_stream);
instruction_byte get_next_instruction_byte(FILE * assembly_file);
instruction_stream * initialize_instruction_stream(FILE * assembly_file);
int match_instruction_to_stream(char * first_byte, char * second_byte, instruction_stream * stream);
void instruction_stream_pop_byte(instruction_stream * stream, FILE * assembly_file);
void instructin_stream_pop_n_bytes(instruction_stream * stream, FILE * assembly_file, int n_increments);
int match_byte(instruction_byte byte, char * match_string);
uint8_t mask(uint8_t byte, uint8_t mask, uint8_t shift);
void decode_regmem_to_regmem(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);

instruction_byte get_next_instruction_byte(FILE * assembly_file) {
	instruction_byte new_byte;
	int readable_int;

	if ((readable_int = getc(assembly_file)) == -1) {
		new_byte.valid = INVALID;
		new_byte.byte = 0;
		assembly_file -= sizeof(char);
	}

	new_byte.valid = INVALID;
	new_byte.byte = (uint8_t) readable_int;

	return new_byte;
}

instruction_stream * initialize_instruction_stream(FILE * assembly_file) {
	instruction_stream * new_stream = calloc(1, sizeof(instruction_stream));

	for (int i = 0; i < 6; i ++) {
		new_stream->instruction_bytes[i] = get_next_instruction_byte(assembly_file);
	}

	return new_stream;
}

void instruction_stream_pop_byte(instruction_stream * stream, FILE * assembly_file) {
	for (int i = 0; i < 5; i ++) {
		stream->instruction_bytes[i] = stream->instruction_bytes[i+1];
	}
	stream->instruction_bytes[5] = get_next_instruction_byte(assembly_file);
}

void instructin_stream_pop_n_bytes(instruction_stream * stream, FILE * assembly_file, int n_increments) {
	for (int i = 0; i < n_increments; i ++)	{
		instruction_stream_pop_byte(stream, assembly_file);
	}
}

int match_byte(instruction_byte byte, char * match_string) {

	if (byte.valid == INVALID) {
		return 0;
	}

	for (int i = 0; i < 8; i++) {
		uint8_t bit_value = (byte.byte >> (7-i)) & ((uint8_t) 0b00000001);
		if ((bit_value == 1 && match_string[i] == '0') || (bit_value == 0 && match_string[i] == '1')) {
			return 0;
		}
	}

	return 1;

}

//cool in my opinion. matches based on "11x01x01", "xxx11xxxx"
int match_instruction_to_stream(char * first_byte, char * second_byte, instruction_stream * stream) {
	int first_byte_valid = match_byte(stream->instruction_bytes[0], first_byte);

	if (second_byte == NULL) {
		return first_byte_valid;
	}

	int second_byte_valid = match_byte(stream->instruction_bytes[1], second_byte);

	return first_byte_valid && second_byte_valid;
}

uint8_t mask(uint8_t byte, uint8_t mask, uint8_t shift) {
	return (byte & mask) >> shift;
}

//this should be the hardest decode!
void decode_regmem_to_regmem(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file) {
	int instruction_length = 2;

	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	uint8_t byte_two = instructions->instruction_bytes[1].byte;

	new_instruction->type = type;
	new_instruction->d = mask(byte_one, 0b00000010, 1);
	new_instruction->w = mask(byte_one, 0b00000001, 0);
	new_instruction->mod = mask(byte_two, 0b11000000, 6); 
	new_instruction->register_one= mask(byte_two, 0b00111000, 3);
	new_instruction->register_two= mask(byte_two, 0b00000111, 0);
	new_instruction->arg_one_type = REG;

	//order is a bit uggly
	if (new_instruction->d == 0) {
		new_instruction->order = ARG_1_SOURCE;
	} else {
		new_instruction->order = ARG_2_SOURCE;
	}

	//1 THIS IS A MESS
	//2 THIS PROBABLY HAS A BUG: THE 8BIT REPRESENTATION IS PROBABLY NOT GOING TO PLAY NICE FOR NEGATIVE NUMBERS
	if (new_instruction->mod == 0 && new_instruction->register_two!= 6) {
		new_instruction->arg_two_type= MEM;
	} else if (new_instruction->mod == 1) {
		instruction_length = 3;
		new_instruction->data_two = (instructions->instruction_bytes[2].byte << 8) + (instructions->instruction_bytes[1].byte);
		new_instruction->arg_two_type= MEM_8;
	} else if (new_instruction->mod == 2) {
		instruction_length = 4;
		new_instruction->data_two= instructions->instruction_bytes[3].byte;
		new_instruction->arg_two_type= MEM_16;
	} else if ((new_instruction->mod == 0 && new_instruction->register_two == 6)) {
		instruction_length = 4;
		new_instruction->data_two= instructions->instruction_bytes[3].byte;
		new_instruction->arg_two_type= DIRECT;
	} else if (new_instruction->mod == 3) {
		new_instruction->arg_two_type= REG;
	}

	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
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

void print_direct_address(uint16_t reg, uint16_t data, FILE * output_stream) {
	fprintf(output_stream, "[%d]", data);
}

void print_mem(uint16_t reg, FILE * output_stream) {
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

void print_mem_8(uint16_t reg, uint16_t data, FILE * output_stream) {
	uint8_t signed_data = data;	

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
	} else {
		fprintf(output_stream, " - %d]", -signed_data);
	}
}

void print_mem_16(uint16_t reg, uint16_t data, FILE * output_stream) {
	uint8_t signed_data = data;	

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
	} else {
		fprintf(output_stream, " - %d]", -signed_data);
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
	int print_imediate_length = 0;	
	if (print_target == SOURCE && opposite_type== MEM && (print_type == IM8 || print_type == IM16)) {
		print_imediate_length = 1;	
	}

	switch (print_type) {
		case REG: print_reg(print_register, print_data, new_instruction->w, output_stream); break;
		case MEM: print_mem(print_register, output_stream); break;
		case MEM_8: print_mem_8(print_register, print_data, output_stream); break;
		case MEM_16: print_mem_16(print_register, print_data, output_stream); break;
		case DIRECT: print_direct_address(print_register, print_data, output_stream); break;
		default: printf("Error"); break;
	}	

}

void print_move(instruction * new_instruction, FILE * output_stream) {
	fprintf(output_stream, "mov ");

	print_instruction_half(new_instruction, output_stream, DEST);
	fprintf(output_stream, ", ");
	print_instruction_half(new_instruction, output_stream, SOURCE);

	fprintf(output_stream, "\n");
}

void decode(instruction_stream * instructions, FILE * assembly_file, FILE * output_stream) {

	//note that some of these instructions specify inversion or not while others calculate inversion themselves.
	instruction * new_instruction = calloc(1, sizeof(instruction));

	//mov register/memory to/from register
	if (match_instruction_to_stream("100010xx", NULL, instructions)) {
		decode_regmem_to_regmem(MOV,instructions, new_instruction, assembly_file); 
	//mov imediate to register/memory
	} else if (match_instruction_to_stream("1100011x", "xx000xxx", instructions)) {
		//decode_im_to_regmem(MOV,instructions, new_instruction, assembly_file); 
	//mov imediate to register
	} else if (match_instruction_to_stream("1011xxxx", NULL, instructions)) {
		//decode_im_to_reg(MOV,instructions, new_instruction, assembly_file); 
	//mov memory to accumulator
	} else if (match_instruction_to_stream("1010000x", NULL, instructions)) {
		//decode_im_to_reg(MOV,instructions, INVERTED); 
	//mov accumilator to memory 
	} else if (match_instruction_to_stream("1010001x", NULL, instructions)) {
		//decode_acc_to_mem(MOV,instructions); 
	//mov register/memory to segment register
	} else if (match_instruction_to_stream("10001110", "xx0xxxxx", instructions)) {
		//decode_regmem_to_seg(MOV,instructions, NON_INVERTED); 
	//mov segment register to register/memory
	} else if (match_instruction_to_stream("10001100", "xx0xxxxx", instructions)) {
		//decode_regmem_to_seg(MOV,instructions, INVERTED); 
	}

	switch (new_instruction->type) {
		case MOV:
			print_move(new_instruction, output_stream);
		break;
	}
	//print the instruction
	//execute the instruciton
	
	free(new_instruction);

	//at this point the instruction stream has been updated.
	if (instructions->instruction_bytes[0].valid == VALID) {
		decode(instructions, assembly_file, output_stream);
	}
}

//now that bytes can be matched with ease, identifying instructions should be easy. From there on decoding can be solved!
//First, orgonize and test some things!
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

	instruction_stream * instructions = initialize_instruction_stream(assembly_file);

	decode(instructions, assembly_file,output_stream);

	free(instructions); //don't think the array needs to be seperately feed

	fclose(assembly_file);
	fclose(output_stream); //this may be sketchy but seems to work fine.

}


