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

void decode_rm_to_seg(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file, source_inversion inverted) {
	int instruction_length = 2;

	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	uint8_t byte_two = instructions->instruction_bytes[1].byte;

	new_instruction->type = type;
	new_instruction->d = mask(byte_one, 0b00000010, 1);
	new_instruction->w = mask(byte_one, 0b00000001, 0);
	new_instruction->mod = mask(byte_two, 0b11000000, 6); 

	new_instruction->register_one= mask(byte_two, 0b00011000, 3);
	new_instruction->arg_one_type= SEG;

	new_instruction->register_two= mask(byte_two, 0b00000111, 0);

	//order is a bit uggly
	if (inverted == NON_INVERTED) {
		new_instruction->order = ARG_2_SOURCE;
	} else {
		new_instruction->order = ARG_1_SOURCE;
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

void decode_rm(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file) {
	int instruction_length = 2;

	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	uint8_t byte_two = instructions->instruction_bytes[1].byte;

	new_instruction->type = type;
	new_instruction->mod = mask(byte_two, 0b11000000, 6); 
	new_instruction->register_one= mask(byte_two, 0b00000111, 0);
	new_instruction->arg_one_type = REG;
	new_instruction->arg_two_type = NONE;
	new_instruction->order = ARG_1_SOURCE;
	new_instruction->w= 1;
	
	if (new_instruction->mod == 0 && new_instruction->register_one!= 6) {
		new_instruction->arg_one_type= MEM;
	} else if (new_instruction->mod == 1) {
		instruction_length += 1;
		new_instruction->data_one= (instructions->instruction_bytes[instruction_length-1].byte);
		new_instruction->arg_one_type= MEM_8;
	} else if (new_instruction->mod == 2) {
		instruction_length += 2;
		new_instruction->data_one= (instructions->instruction_bytes[instruction_length-1].byte << 8) + (instructions->instruction_bytes[instruction_length -2].byte);
		new_instruction->data_one= MEM_16;
	} else if ((new_instruction->mod == 0 && new_instruction->register_one== 6)) {
		instruction_length += 2;
		new_instruction->data_one= (instructions->instruction_bytes[instruction_length-1].byte << 8) + (instructions->instruction_bytes[instruction_length -2].byte);
		new_instruction->arg_one_type= DIRECT;
	} else if (new_instruction->mod == 3) {
		new_instruction->arg_one_type= REG;
	}
	
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}

void decode_reg(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file) {
	int instruction_length = 1;
	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	new_instruction->type = type;
	new_instruction->order = ARG_1_SOURCE;
	new_instruction->arg_one_type = REG;
	new_instruction->arg_two_type = NONE;
	new_instruction->register_one= mask(byte_one, 0b00000111, 0);
	new_instruction->w= 1;
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}

void decode_seg(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file) {
	
	int instruction_length = 1;
	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	new_instruction->type = type;
	new_instruction->order = ARG_1_SOURCE;
	new_instruction->arg_one_type = SEG;
	new_instruction->arg_two_type = NONE;
	new_instruction->register_one= mask(byte_one, 0b00011000, 3);
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}

void decode_reg_to_acc(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file, source_inversion inversion) {
	int instruction_length = 1;
	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	new_instruction->type = type;
	if (inversion == NON_INVERTED) {
		new_instruction->order = ARG_1_SOURCE;
		new_instruction->arg_one_type = REG;
		new_instruction->arg_two_type = ACC;
		new_instruction->register_one= mask(byte_one, 0b00000111, 0);
	} else {
		new_instruction->order = ARG_1_SOURCE;
		new_instruction->arg_two_type = REG;
		new_instruction->arg_one_type= ACC;
		new_instruction->register_two= mask(byte_one, 0b00000111, 0);
	}

	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}

void decode_signed_imediate_to_regmem(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file) {
	int instruction_length = 2;

	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	uint8_t byte_two = instructions->instruction_bytes[1].byte;

	new_instruction->type = type;
	new_instruction->w = mask(byte_one, 0b00000001, 0);
	new_instruction->s = mask(byte_one, 0b00000010, 1);
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

	if (new_instruction->w == 1 && new_instruction->s == 0) {
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

void decode_imediate_to_acc(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file) {
	int instruction_length = 1;

	uint8_t byte_one = instructions->instruction_bytes[0].byte;

	new_instruction->type = type;
	new_instruction->w = mask(byte_one, 0b00000001, 0);
	new_instruction->arg_two_type= ACC;
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
	//mov register/memory to segment register
	} else if (match_instruction_to_stream("10001110", "xx0xxxxx", instructions)) {
		decode_rm_to_seg(MOV,instructions, new_instruction, assembly_file, NON_INVERTED); 
	//mov segment register to register/memory
	} else if (match_instruction_to_stream("10001100", "xx0xxxxx", instructions)) {
		decode_rm_to_seg(MOV,instructions, new_instruction, assembly_file, INVERTED); 

	
	//push regsiter/memory	
	} else if (match_instruction_to_stream("11111111", "xx110xxx", instructions)) {
		decode_rm(PUSH,instructions, new_instruction, assembly_file); 
	//push register
	} else if (match_instruction_to_stream("01010xxx", NULL, instructions)) {
		decode_reg(PUSH,instructions, new_instruction, assembly_file); 
	//push segment register 
	} else if (match_instruction_to_stream("000xx110", NULL, instructions)) {
		decode_seg(PUSH,instructions, new_instruction, assembly_file); 


	//pop register/memory
	} else if (match_instruction_to_stream("10001111", "xx000xxx", instructions)) {
		decode_rm(POP,instructions, new_instruction, assembly_file); 
	//pop register
	} else if (match_instruction_to_stream("01011xxx", NULL, instructions)) {
		decode_reg(POP,instructions, new_instruction, assembly_file); 
	//pop segment register 
	} else if (match_instruction_to_stream("000xx111", NULL, instructions)) {
		decode_seg(POP,instructions, new_instruction, assembly_file); 
	//exchg regmem reg
	} else if (match_instruction_to_stream("1000011x", "xxxxxxxx", instructions)) {
		decode_regmem_to_regmem(XCHG,instructions, new_instruction, assembly_file); 
	//exchg reg acc
	} else if (match_instruction_to_stream("10010xxx", NULL, instructions)) {
		decode_reg_to_acc(XCHG,instructions, new_instruction, assembly_file, NON_INVERTED); 


	//similar instructions: add, adc, sub sbb and test or xor **note that some use the easier version of the second portion...

	//add regmem_regmem
	} else if (match_instruction_to_stream("000000xx", NULL, instructions)) {
		decode_regmem_to_regmem(ADD,instructions, new_instruction, assembly_file); 
	//add immediate to regmem
	} else if (match_instruction_to_stream("100000xx", "xx000xxx", instructions)) {
		decode_signed_imediate_to_regmem(ADD,instructions, new_instruction, assembly_file); 
	//add immediate to accumilator
	} else if (match_instruction_to_stream("0000010x", NULL, instructions)) {
		decode_imediate_to_acc(ADD, instructions, new_instruction, assembly_file); 
	
	//adc regmem_regmem
	} else if (match_instruction_to_stream("000100xx", NULL, instructions)) {
		decode_regmem_to_regmem(ADC,instructions, new_instruction, assembly_file); 
	//adc immediate to regmem
	} else if (match_instruction_to_stream("100000xx", "xx010xxx", instructions)) {
		decode_signed_imediate_to_regmem(ADC,instructions, new_instruction, assembly_file); 
	//adc immediate to accumilator
	} else if (match_instruction_to_stream("0001010x", NULL, instructions)) {
		decode_imediate_to_acc(ADC, instructions, new_instruction, assembly_file); 

	//sub regmem_regmem
	} else if (match_instruction_to_stream("001010xx", NULL, instructions)) {
		decode_regmem_to_regmem(SUB,instructions, new_instruction, assembly_file); 
	//sub immediate to regmem
	} else if (match_instruction_to_stream("100000xx", "xx101xxx", instructions)) {
		decode_signed_imediate_to_regmem(SUB,instructions, new_instruction, assembly_file); 
	//sub immediate to accumilator
	} else if (match_instruction_to_stream("0010110x", NULL, instructions)) {
		decode_imediate_to_acc(SUB, instructions, new_instruction, assembly_file); 

	//sbb regmem_regmem
	} else if (match_instruction_to_stream("000110xx", NULL, instructions)) {
		decode_regmem_to_regmem(SBB,instructions, new_instruction, assembly_file); 
	//sub immediate to regmem
	} else if (match_instruction_to_stream("100000xx", "xx011xxx", instructions)) {
		decode_signed_imediate_to_regmem(SBB,instructions, new_instruction, assembly_file); 
	//sub immediate to accumilator
	} else if (match_instruction_to_stream("0001110x", NULL, instructions)) {
		decode_imediate_to_acc(SBB, instructions, new_instruction, assembly_file); 

	} else {
		printf("Opcode not understood.\n");
		exit(0);
	}

	//print the instruction
	switch (new_instruction->type) {
		case MOV:
			print_two_arg_instruction(MOV,new_instruction, output_stream);
			break;
		case POP:
			print_one_arg_instruction(POP,new_instruction, output_stream);
			break;
		case PUSH:
			print_one_arg_instruction(PUSH,new_instruction, output_stream);
			break;
		case XCHG:
			print_two_arg_instruction(XCHG, new_instruction, output_stream);
			break;
		case ADD:
			print_two_arg_instruction(ADD, new_instruction, output_stream);
			break;
		case ADC:
			print_two_arg_instruction(ADC, new_instruction, output_stream);
			break;
		case SUB:
			print_two_arg_instruction(SUB, new_instruction, output_stream);
			break;
		case SBB:
			print_two_arg_instruction(SBB, new_instruction, output_stream);
			break;
	}

	//execute the instruciton
	
	free(new_instruction);

	//at this point the instruction stream has been updated.
	if (instructions->instruction_bytes[0].valid == VALID) {
		decode(instructions, assembly_file, output_stream);
	}
}
