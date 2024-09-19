#include "disassembler.h"

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
	
	if (new_instruction->mod == 0 && new_instruction->register_two!= 6) {
		new_instruction->arg_two_type= MEM;
	} else if (new_instruction->mod == 1) {
		instruction_length += 1;
		new_instruction->data_two = (instructions->instruction_bytes[instruction_length-1].byte);
		new_instruction->arg_two_type= MEM_8;
	} else if (new_instruction->mod == 2) {
		instruction_length += 2;
		new_instruction->data_two = (instructions->instruction_bytes[instruction_length-1].byte << 8) + (instructions->instruction_bytes[instruction_length -2].byte);
		new_instruction->arg_two_type= MEM_16;
	} else if ((new_instruction->mod == 0 && new_instruction->register_two == 6)) {
		instruction_length += 2;
		new_instruction->data_two = (instructions->instruction_bytes[instruction_length-1].byte << 8) + (instructions->instruction_bytes[instruction_length -2].byte);
		new_instruction->arg_two_type= DIRECT;
	} else if (new_instruction->mod == 3) {
		new_instruction->arg_two_type= REG;
	}
	
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}

void decode_imediate_to_regmem(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file) {
	int instruction_length = 2;

	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	uint8_t byte_two = instructions->instruction_bytes[1].byte;

	new_instruction->type = type;
	new_instruction->w = mask(byte_one, 0b00000001, 0);
	new_instruction->mod = mask(byte_two, 0b11000000, 6); 

	new_instruction->register_two= mask(byte_two, 0b00000111, 0);
	new_instruction->order = ARG_1_SOURCE;
	
	if (new_instruction->mod == 0 && new_instruction->register_two!= 6) {
		new_instruction->arg_two_type= MEM;
	} else if (new_instruction->mod == 1) {
		instruction_length += 1;
		new_instruction->data_two = (instructions->instruction_bytes[instruction_length-1].byte);
		new_instruction->arg_two_type= MEM_8;
	} else if (new_instruction->mod == 2) {
		instruction_length += 2;
		new_instruction->data_two = (instructions->instruction_bytes[instruction_length-1].byte << 8) + (instructions->instruction_bytes[instruction_length -2].byte);
		new_instruction->arg_two_type= MEM_16;
	} else if ((new_instruction->mod == 0 && new_instruction->register_two == 6)) {
		instruction_length += 2;
		new_instruction->data_two = (instructions->instruction_bytes[instruction_length-1].byte << 8) + (instructions->instruction_bytes[instruction_length -2].byte);
		new_instruction->arg_two_type= DIRECT;
	} else if (new_instruction->mod == 3) {
		new_instruction->arg_two_type= REG;
	}

	if (new_instruction->w == 1) {
		//16 bit
		instruction_length += 2;
		new_instruction->data_one= (instructions->instruction_bytes[instruction_length-1].byte << 8) + (instructions->instruction_bytes[instruction_length -2].byte);
		new_instruction->arg_one_type = IM16;
	} else {
		//8 bit
		instruction_length += 1;
		new_instruction->data_one = (instructions->instruction_bytes[instruction_length-1].byte);
		new_instruction->arg_one_type = IM8;
	}
	
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}

void decode_imediate_to_reg(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file) {
	int instruction_length = 1;

	uint8_t byte_one = instructions->instruction_bytes[0].byte;

	new_instruction->type = type;
	new_instruction->w = mask(byte_one, 0b00001000, 3);
	new_instruction->register_two= mask(byte_one, 0b00000111, 0);
	new_instruction->arg_two_type= REG;
	new_instruction->order = ARG_1_SOURCE;

	if (new_instruction->w == 1) {
		//16 bit
		instruction_length += 2;
		new_instruction->data_one= (instructions->instruction_bytes[instruction_length-1].byte << 8) + (instructions->instruction_bytes[instruction_length -2].byte);
		new_instruction->arg_one_type = IM16;
	} else {
		//8 bit
		instruction_length += 1;
		new_instruction->data_one = (instructions->instruction_bytes[instruction_length-1].byte);
		new_instruction->arg_one_type = IM8;
	}
	
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}

void decode_mem_to_acc(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file, source_inversion inverted) {
	int instruction_length = 1;

	uint8_t byte_one = instructions->instruction_bytes[0].byte;

	new_instruction->type = type;
	new_instruction->w = mask(byte_one, 0b00000001, 0);
	new_instruction->arg_one_type= ACC;
	new_instruction->arg_two_type= DIRECT;

	instruction_length += 2;
	new_instruction->data_two = (instructions->instruction_bytes[instruction_length-1].byte << 8) + (instructions->instruction_bytes[instruction_length -2].byte);
	
	//flip if source inversion is toggled
	if (inverted == NON_INVERTED) {
		new_instruction->order = ARG_2_SOURCE;
	} else {
		new_instruction->order = ARG_1_SOURCE;
	}
	
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}

void decode(instruction_stream * instructions, FILE * assembly_file, FILE * output_stream) {

	//note that some of these instructions specify inversion or not while others calculate inversion themselves.
	instruction * new_instruction = calloc(1, sizeof(instruction));

	//mov register/memory to/from register
	if (match_instruction_to_stream("100010xx", NULL, instructions)) {
		decode_regmem_to_regmem(MOV,instructions, new_instruction, assembly_file); 
	//mov imediate to register/memory
	} else if (match_instruction_to_stream("1100011x", "xx000xxx", instructions)) {
		decode_imediate_to_regmem(MOV,instructions, new_instruction, assembly_file); 
	//mov imediate to register
	} else if (match_instruction_to_stream("1011xxxx", NULL, instructions)) {
		decode_imediate_to_reg(MOV,instructions, new_instruction, assembly_file); 
	//mov memory to accumulator
	} else if (match_instruction_to_stream("1010000x", NULL, instructions)) {
		decode_mem_to_acc(MOV,instructions, new_instruction, assembly_file, NON_INVERTED); 
	//mov accumilator to memory 
	} else if (match_instruction_to_stream("1010001x", NULL, instructions)) {
		decode_mem_to_acc(MOV,instructions, new_instruction, assembly_file, INVERTED); 
		//decode_acc_to_mem(MOV,instructions); 
	//mov register/memory to segment register
	} else if (match_instruction_to_stream("10001110", "xx0xxxxx", instructions)) {
		//decode_regmem_to_seg(MOV,instructions, NON_INVERTED); 
	//mov segment register to register/memory
	} else if (match_instruction_to_stream("10001100", "xx0xxxxx", instructions)) {
		//decode_regmem_to_seg(MOV,instructions, INVERTED); 
	}

	//print the instruction
	switch (new_instruction->type) {
		case MOV:
			print_move(new_instruction, output_stream);
		break;
	}

	//execute the instruciton
	
	free(new_instruction);

	//at this point the instruction stream has been updated.
	if (instructions->instruction_bytes[0].valid == VALID) {
		decode(instructions, assembly_file, output_stream);
	}
}
