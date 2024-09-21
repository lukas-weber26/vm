#include "disassembler.h"
#include <stdio.h>
#define FALSE 0
#define TRUE 1

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

void decode_imediate_to_acc_short(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file) {
	int instruction_length = 1;

	uint8_t byte_one = instructions->instruction_bytes[0].byte;

	new_instruction->type = type;
	new_instruction->w = mask(byte_one, 0b00000001, 0);
	new_instruction->arg_two_type= ACC;
	new_instruction->order = ARG_1_SOURCE;

	//8 bit
	instruction_length += 1;
	new_instruction->data_one = (instructions->instruction_bytes[instruction_length-1].byte);
	new_instruction->arg_one_type = IM8;
	
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}
		
void decode_regmem(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file, int v_present) {
	int instruction_length = 2;

	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	uint8_t byte_two = instructions->instruction_bytes[1].byte;

	new_instruction->type = type;
	new_instruction->w = mask(byte_one, 0b00000001, 0);

	if (v_present) {
		new_instruction->v = mask(byte_one, 0b00000010, 1);
		new_instruction->arg_one_type = CL;
	} else {
		new_instruction->v = 0;
		new_instruction->arg_one_type = NONE;
	}

	new_instruction->mod = mask(byte_two, 0b11000000, 6); 
	new_instruction->register_two= mask(byte_two, 0b00000111, 0);
	new_instruction->order = ARG_2_SOURCE; //this is quite likely to throw things off but it's necessary since these instructions sometimes have two args
	
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

void decode_z(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file) {
	int instruction_length = 1;
	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	new_instruction->v = mask(byte_one, 0b00000001, 0);
	new_instruction->type = type;
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}

void decode_w(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file) {
	int instruction_length = 1;
	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	new_instruction->w = mask(byte_one, 0b00000001, 0);
	new_instruction->type = type;
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}

void decode_none(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file) {
	int instruction_length = 1;
	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	new_instruction->type = type;
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}

void decode_jump(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file ) {
	int instruction_length = 2;
	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	uint8_t byte_two = instructions->instruction_bytes[1].byte;
	new_instruction->type = type;
	new_instruction->data_one = byte_two;
	new_instruction->arg_one_type = IP_INC8;
	new_instruction->order = ARG_1_SOURCE;
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}

void decode_data(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file ) {
	int instruction_length = 3;
	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	uint8_t byte_two = instructions->instruction_bytes[1].byte;
	uint8_t byte_three = instructions->instruction_bytes[1].byte;
	new_instruction->type = type;
	new_instruction->data_one = (byte_three<< 8) + byte_two;
	new_instruction->arg_one_type = IM16;
	new_instruction->order = ARG_1_SOURCE;
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}

void decode_ip_inc(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file, int short_inc){
	new_instruction->type = type;
	int instruction_length = 3;
	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	uint8_t byte_two = instructions->instruction_bytes[1].byte;
	new_instruction->order = ARG_1_SOURCE;

	if (short_inc == FALSE) {
		instruction_length = 3;
		uint8_t byte_three = instructions->instruction_bytes[1].byte;
		new_instruction->data_one = (byte_three<< 8) + byte_two;
		new_instruction->arg_one_type = IP_INC16;
	} else {
		instruction_length = 2;
		new_instruction->data_one = byte_two;
		new_instruction->arg_one_type = IP_INC8;
	}
	
	instructin_stream_pop_n_bytes(instructions, assembly_file, instruction_length);
}
	
void decode_data_8(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file){
	new_instruction->type = type;
	int instruction_length = 2;
	uint8_t byte_one = instructions->instruction_bytes[0].byte;
	uint8_t byte_two = instructions->instruction_bytes[1].byte;
	new_instruction->order = ARG_1_SOURCE;
	new_instruction->w = mask(byte_one, 0b00000001, 0);
	new_instruction->arg_one_type = IM8;
	new_instruction->data_one = byte_two;
	
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

	//and regmem to regmem
	} else if (match_instruction_to_stream("001000xx", NULL, instructions)) {
		decode_regmem_to_regmem(AND,instructions, new_instruction, assembly_file); 
	//and immediate to regmem
	} else if (match_instruction_to_stream("1000000x", "xx100xxx", instructions)) {
		decode_imediate_to_regmem(SBB,instructions, new_instruction, assembly_file); 
	//and immediate to accumilator
	} else if (match_instruction_to_stream("0010010x", NULL, instructions)) {
		decode_imediate_to_acc(SBB, instructions, new_instruction, assembly_file); 
	
	//test regmem to regmem
	} else if (match_instruction_to_stream("000100xx", NULL, instructions)) {
		decode_regmem_to_regmem(TEST,instructions, new_instruction, assembly_file); 
	//test immediate to regmem
	} else if (match_instruction_to_stream("1111011x", "xx000xxx", instructions)) {
		decode_imediate_to_regmem(TEST,instructions, new_instruction, assembly_file); 
	//test immediate to accumilator
	} else if (match_instruction_to_stream("1010100x", NULL, instructions)) {
		decode_imediate_to_acc_short(TEST, instructions, new_instruction, assembly_file); 
	
	//or regmem to regmem
	} else if (match_instruction_to_stream("000010xx", NULL, instructions)) {
		decode_regmem_to_regmem(OR,instructions, new_instruction, assembly_file); 
	//or immediate to regmem
	} else if (match_instruction_to_stream("1000000x", "xx001xxx", instructions)) {
		decode_imediate_to_regmem(OR,instructions, new_instruction, assembly_file); 
	//or immediate to accumilator
	} else if (match_instruction_to_stream("0000110x", NULL, instructions)) {
		decode_imediate_to_acc(OR, instructions, new_instruction, assembly_file); 
	
	//xor regmem to regmem
	} else if (match_instruction_to_stream("001100xx", NULL, instructions)) {
		decode_regmem_to_regmem(XOR,instructions, new_instruction, assembly_file); 
	//xor immediate to regmem
	} else if (match_instruction_to_stream("0011010x", "xxxxxxxx", instructions)) {
		decode_imediate_to_regmem(XOR,instructions, new_instruction, assembly_file); 
	//xor immediate to accumilator
	} else if (match_instruction_to_stream("0011010x", NULL, instructions)) {
		decode_imediate_to_acc(XOR, instructions, new_instruction, assembly_file); 

	//logic not 
	} else if (match_instruction_to_stream("1111011x", "xx010xxx", instructions)) {
		decode_regmem(NOT, instructions, new_instruction, assembly_file, FALSE); 
	//logic	shl 
	} else if (match_instruction_to_stream("110100xx", "xx100xxx", instructions)) {
		decode_regmem(SHL, instructions, new_instruction, assembly_file, TRUE); 
	//logic shr	
	} else if (match_instruction_to_stream("110100xx", "xx101xxx", instructions)) {
		decode_regmem(SHR, instructions, new_instruction, assembly_file, TRUE); 
	//logic sar	
	} else if (match_instruction_to_stream("110100xx", "xx111xxx", instructions)) {
		decode_regmem(SAR, instructions, new_instruction, assembly_file, TRUE); 
	//logic rol 
	} else if (match_instruction_to_stream("110100xx", "xx000xxx", instructions)) {
		decode_regmem(ROL, instructions, new_instruction, assembly_file, TRUE); 
	//logic ror 
	} else if (match_instruction_to_stream("110100xx", "xx001xxx", instructions)) {
		decode_regmem(ROR, instructions, new_instruction, assembly_file, TRUE); 
	//logic rcl 
	} else if (match_instruction_to_stream("110100xx", "xx010xxx", instructions)) {
		decode_regmem(RCL, instructions, new_instruction, assembly_file, TRUE); 
	//logic rcr 
	} else if (match_instruction_to_stream("110100xx", "xx011xxx", instructions)) {
		decode_regmem(RCR, instructions, new_instruction, assembly_file, TRUE); 

	//rep
	} else if (match_instruction_to_stream("1111001x", NULL, instructions)) {
		decode_z(REP, instructions, new_instruction, assembly_file); 
	//movs
	} else if (match_instruction_to_stream("1010010x", NULL, instructions)) {
		decode_w(MOVS, instructions, new_instruction, assembly_file); 
	//cmps
	} else if (match_instruction_to_stream("1010011x", NULL, instructions)) {
		decode_w(CMPS, instructions, new_instruction, assembly_file); 
	//scas
	} else if (match_instruction_to_stream("1010111x", NULL, instructions)) {
		decode_w(SCAS, instructions, new_instruction, assembly_file); 
	//lods
	} else if (match_instruction_to_stream("1010110x", NULL, instructions)) {
		decode_w(LODS, instructions, new_instruction, assembly_file); 
	//stds
	} else if (match_instruction_to_stream("1010101x", NULL, instructions)) {
		decode_w(STDS, instructions, new_instruction, assembly_file); 

	//increment regmem
	} else if (match_instruction_to_stream("1111111x", "xx000xxx", instructions)) {
		decode_regmem(INC, instructions, new_instruction, assembly_file, NON_INVERTED); 
	//increment reg 
	} else if (match_instruction_to_stream("01000xxx", NULL, instructions)) {
		decode_reg(INC, instructions, new_instruction, assembly_file); 
	//decrement regmem
	} else if (match_instruction_to_stream("1111111x", "xx000xxx", instructions)) {
		decode_regmem(DEC, instructions, new_instruction, assembly_file, NON_INVERTED); 
	//increment reg 
	} else if (match_instruction_to_stream("01000xxx", NULL, instructions)) {
		decode_reg(DEC, instructions, new_instruction, assembly_file); 
	//neg regmem
	} else if (match_instruction_to_stream("1111011x", "xx011xxx", instructions)) {
		decode_regmem(NEG, instructions, new_instruction, assembly_file, NON_INVERTED); 

	//AAA
	} else if (match_instruction_to_stream("00110111", NULL, instructions)) {
		decode_none(AAA, instructions, new_instruction, assembly_file); 
	//DAA
	} else if (match_instruction_to_stream("00100111", NULL, instructions)) {
		decode_none(DAA, instructions, new_instruction, assembly_file); 
	//AAS
	} else if (match_instruction_to_stream("00111111", NULL, instructions)) {
		decode_none(AAS, instructions, new_instruction, assembly_file); 
	//DAS
	} else if (match_instruction_to_stream("00101111", NULL, instructions)) {
		decode_none(DAS, instructions, new_instruction, assembly_file); 
	//CBW
	} else if (match_instruction_to_stream("10011000", NULL, instructions)) {
		decode_none(CBW, instructions, new_instruction, assembly_file); 
	//CWD
	} else if (match_instruction_to_stream("10011001", NULL, instructions)) {
		decode_none(CWD, instructions, new_instruction, assembly_file); 

	//cmp regmem to regmem
	} else if (match_instruction_to_stream("001110xx", NULL, instructions)) {
		decode_regmem_to_regmem(CMP,instructions, new_instruction, assembly_file); 
	//cmp immediate to regmem
	} else if (match_instruction_to_stream("100000xx", "xx111xxx", instructions)) {
		decode_imediate_to_regmem(CMP,instructions, new_instruction, assembly_file); 
	//cmp immediate to accumilator
	} else if (match_instruction_to_stream("0011110x", NULL, instructions)) {
		decode_imediate_to_acc_short(CMP, instructions, new_instruction, assembly_file); 

	//MUL
	} else if (match_instruction_to_stream("1111011x", "xx100xxx", instructions)) {
		decode_regmem_to_regmem(MUL, instructions, new_instruction, assembly_file); 
	//IMUL
	} else if (match_instruction_to_stream("1111011x", "xx101xxx", instructions)) {
		decode_regmem_to_regmem(IMUL, instructions,new_instruction, assembly_file); 
	//DIV
	} else if (match_instruction_to_stream("1111011x", "xx110xxx", instructions)) {
		decode_regmem_to_regmem(DIV, instructions, new_instruction, assembly_file); 
	//IDIV
	} else if (match_instruction_to_stream("1111011x", "xx111xxx", instructions)) {
		decode_regmem_to_regmem(IDIV, instructions, new_instruction, assembly_file); 
	//AAD
	} else if (match_instruction_to_stream("11010101", "00001010", instructions)) {
		decode_regmem(AAD, instructions, new_instruction, assembly_file, NON_INVERTED); 
	//AAM
	} else if (match_instruction_to_stream("1111011x", "00001010", instructions)) {
		decode_regmem(AAM, instructions, new_instruction, assembly_file, NON_INVERTED); 
	
	//JE
	} else if (match_instruction_to_stream("01110100", NULL, instructions)) {
		decode_jump(JE, instructions, new_instruction, assembly_file); 
	//JL
	} else if (match_instruction_to_stream("01111100", NULL, instructions)) {
		decode_jump(JL, instructions, new_instruction, assembly_file); 
	//JLE
	} else if (match_instruction_to_stream("01111110", NULL, instructions)) {
		decode_jump(JLE, instructions, new_instruction, assembly_file); 
	//JB
	} else if (match_instruction_to_stream("01110010", NULL, instructions)) {
		decode_jump(JB, instructions, new_instruction, assembly_file); 
	//JBE
	} else if (match_instruction_to_stream("01110110", NULL, instructions)) {
		decode_jump(JBE, instructions, new_instruction, assembly_file); 
	//JP
	} else if (match_instruction_to_stream("01111010", NULL, instructions)) {
		decode_jump(JE, instructions, new_instruction, assembly_file); 
	//JO
	} else if (match_instruction_to_stream("01110000", NULL, instructions)) {
		decode_jump(JO, instructions, new_instruction, assembly_file); 
	//JS
	} else if (match_instruction_to_stream("01111000", NULL, instructions)) {
		decode_jump(JS, instructions, new_instruction, assembly_file); 
	//JNE
	} else if (match_instruction_to_stream("01110101", NULL, instructions)) {
		decode_jump(JNE, instructions, new_instruction, assembly_file); 
	//JNL
	} else if (match_instruction_to_stream("01111101", NULL, instructions)) {
		decode_jump(JNL, instructions, new_instruction, assembly_file); 
	//JNLE
	} else if (match_instruction_to_stream("01111111", NULL, instructions)) {
		decode_jump(JNLE, instructions, new_instruction, assembly_file); 
	//JNB
	} else if (match_instruction_to_stream("01110011", NULL, instructions)) {
		decode_jump(JNB, instructions, new_instruction, assembly_file); 
	//JNBE
	} else if (match_instruction_to_stream("01110111", NULL, instructions)) {
		decode_jump(JNBE, instructions, new_instruction, assembly_file); 
	//JNP
	} else if (match_instruction_to_stream("01111011", NULL, instructions)) {
		decode_jump(JNP, instructions, new_instruction, assembly_file); 
	//JNO
	} else if (match_instruction_to_stream("01110001", NULL, instructions)) {
		decode_jump(JNO, instructions, new_instruction, assembly_file); 
	//JNS
	} else if (match_instruction_to_stream("01111001", NULL, instructions)) {
		decode_jump(JNS, instructions, new_instruction, assembly_file); 
	//LOOP
	} else if (match_instruction_to_stream("11100010", NULL, instructions)) {
		decode_jump(LOOP, instructions, new_instruction, assembly_file); 
	//LOOPZ
	} else if (match_instruction_to_stream("11100001", NULL, instructions)) {
		decode_jump(LOOPZ, instructions, new_instruction, assembly_file); 
	//LOOPNZ
	} else if (match_instruction_to_stream("11100000", NULL, instructions)) {
		decode_jump(LOOPNZ, instructions, new_instruction, assembly_file); 
	//JCXZ
	} else if (match_instruction_to_stream("11100011", NULL, instructions)) {
		decode_jump(JCXZ, instructions, new_instruction, assembly_file); 

	//RET within seg
	} else if (match_instruction_to_stream("11000011", NULL, instructions)) {
		decode_none(RET, instructions, new_instruction, assembly_file); 
	//Ret intersegment
	} else if (match_instruction_to_stream("11001011", NULL, instructions)) {
		decode_none(RET, instructions, new_instruction, assembly_file); 
	//ret seg (large)
	} else if (match_instruction_to_stream("11001011", NULL, instructions)) {
		decode_data(RET, instructions, new_instruction, assembly_file); 
	//ret intersegment (large)
	} else if (match_instruction_to_stream("11001011", NULL, instructions)) {
		decode_data(RET, instructions, new_instruction, assembly_file); 

	//call direct within segment 
	} else if (match_instruction_to_stream("11101000", NULL, instructions)) {
		decode_ip_inc(CALL, instructions, new_instruction, assembly_file, FALSE); 
	//indirect within segment
	} else if (match_instruction_to_stream("11111111", "xx010xxx", instructions)) {
		decode_regmem(CALL, instructions, new_instruction, assembly_file, NON_INVERTED); 
	//direct intersegment
	} else if (match_instruction_to_stream("10011010", NULL, instructions)) {
		decode_ip_inc(CALL, instructions, new_instruction, assembly_file, FALSE); 
	//indirect intersegment 
	} else if (match_instruction_to_stream("11111111", "xx011xxx", instructions)) {
		decode_regmem(CALL, instructions, new_instruction, assembly_file, NON_INVERTED); 
	
	//jmp direct within segment 
	} else if (match_instruction_to_stream("11101001", NULL, instructions)) {
		decode_ip_inc(JMP, instructions, new_instruction, assembly_file, FALSE); 
	//jmp direct within segment short
	} else if (match_instruction_to_stream("11101011", NULL, instructions)) {
		decode_ip_inc(JMP, instructions, new_instruction, assembly_file, TRUE); 
	//jmp within segment
	} else if (match_instruction_to_stream("11111111", "xx100xxx", instructions)) {
		decode_regmem(JMP, instructions, new_instruction, assembly_file, NON_INVERTED); 
	//jmp intersegment
	} else if (match_instruction_to_stream("11101010", NULL, instructions)) {
		decode_ip_inc(JMP, instructions, new_instruction, assembly_file, FALSE); 
	//jmp intersegment 
	} else if (match_instruction_to_stream("11111111", "xx101xxx", instructions)) {
		decode_regmem(JMP, instructions, new_instruction, assembly_file, NON_INVERTED); 
	

	//In fixed 
	} else if (match_instruction_to_stream("1110010x", NULL, instructions)) {
		decode_data_8(IN, instructions, new_instruction, assembly_file); 
	//In variable
	} else if (match_instruction_to_stream("1110110x", NULL, instructions)) {
		decode_w(IN, instructions, new_instruction, assembly_file); 
	//out fixed 
	} else if (match_instruction_to_stream("1110011x", NULL, instructions)) {
		decode_data_8(OUT, instructions, new_instruction, assembly_file); 
	//out variable
	} else if (match_instruction_to_stream("1110111x", NULL, instructions)) {
		decode_w(OUT, instructions, new_instruction, assembly_file); 
	
	//xlat variable
	} else if (match_instruction_to_stream("11010111", NULL, instructions)) {
		decode_none(XLAT, instructions, new_instruction, assembly_file); 
	//lahf variable
	} else if (match_instruction_to_stream("10011111", NULL, instructions)) {
		decode_none(LAHF, instructions, new_instruction, assembly_file); 
	//sahf variable
	} else if (match_instruction_to_stream("10011110", NULL, instructions)) {
		decode_none(SAHF, instructions, new_instruction, assembly_file); 
	//pushf variable
	} else if (match_instruction_to_stream("10011100", NULL, instructions)) {
		decode_none(PUSHF, instructions, new_instruction, assembly_file); 
	//popf variable
	} else if (match_instruction_to_stream("10011101", NULL, instructions)) {
		decode_none(POPF, instructions, new_instruction, assembly_file); 

	//lea
	} else if (match_instruction_to_stream("10001101", NULL, instructions)) {
		decode_regmem_to_regmem(LEA, instructions, new_instruction, assembly_file); 
	//lds
	} else if (match_instruction_to_stream("11000101", NULL, instructions)) {
		decode_regmem_to_regmem(LDS, instructions, new_instruction, assembly_file); 
	//les
	} else if (match_instruction_to_stream("11000100", NULL, instructions)) {
		decode_regmem_to_regmem(LES, instructions, new_instruction, assembly_file); 

	} else {
		printf("Opcode not understood.\n");
		exit(0);
	}

	//lea lds les

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
		case AND: 
			print_two_arg_instruction(AND, new_instruction, output_stream);
			break;
		case TEST: 
			print_two_arg_instruction(TEST, new_instruction, output_stream);
			break;
		case CMP: 
			print_two_arg_instruction(CMP, new_instruction, output_stream);
			break;
		case OR: 
			print_two_arg_instruction(OR, new_instruction, output_stream);
			break;
		case XOR: 
			print_two_arg_instruction(XOR, new_instruction, output_stream);
			break;
		case NOT: 
			print_v_arg_instruction(NOT, new_instruction, output_stream);
			break;
		case SHL: 
			print_v_arg_instruction(SHL, new_instruction, output_stream);
			break;
		case SHR: 
			print_v_arg_instruction(SHR, new_instruction, output_stream);
			break;
		case SAR: 
			print_v_arg_instruction(SAR, new_instruction, output_stream);
			break;
		case ROL: 
			print_v_arg_instruction(ROL, new_instruction, output_stream);
			break;
		case ROR: 
			print_v_arg_instruction(ROR, new_instruction, output_stream);
			break;
		case RCL: 
			print_v_arg_instruction(RCL, new_instruction, output_stream);
			break;
		case RCR: 
			print_v_arg_instruction(RCR, new_instruction, output_stream);
			break;
		case REP: 
			print_special_instruction(REP, new_instruction, output_stream);
			break;
		case MOVS: 
			print_special_instruction(MOVS, new_instruction, output_stream);
			break;
		case CMPS: 
			print_special_instruction(CMPS, new_instruction, output_stream);
			break;
		case SCAS: 
			print_special_instruction(SCAS, new_instruction, output_stream);
			break;
		case LODS: 
			print_special_instruction(LODS, new_instruction, output_stream);
			break;
		case STDS: 
			print_special_instruction(STDS, new_instruction, output_stream);
			break;
		case INC: 
			print_one_arg_instruction(INC, new_instruction, output_stream);
			break;
		case DEC: 
			print_one_arg_instruction(DEC, new_instruction, output_stream);
			break;
		case NEG: 
			print_one_arg_instruction(NEG, new_instruction, output_stream);
			break;
		case AAA: 
			print_special_instruction(AAA, new_instruction, output_stream);
			break;
		case DAA: 
			print_special_instruction(DAA, new_instruction, output_stream);
			break;
		case AAS: 
			print_special_instruction(AAS, new_instruction, output_stream);
			break;
		case DAS: 
			print_special_instruction(DAS, new_instruction, output_stream);
			break;
		case CBW: 
			print_special_instruction(CBW, new_instruction, output_stream);
			break;
		case CWD: 
			print_special_instruction(CWD, new_instruction, output_stream);
			break;
		case MUL:
			print_two_arg_instruction(MUL, new_instruction, output_stream);
			break;
		case IMUL:
			print_two_arg_instruction(IMUL, new_instruction, output_stream);
			break;
		case DIV:
			print_two_arg_instruction(DIV, new_instruction, output_stream);
			break;
		case IDIV:
			print_two_arg_instruction(IDIV, new_instruction, output_stream);
			break;
		case AAD:
			print_one_arg_instruction(AAD, new_instruction, output_stream);
			break;
		case AAM:
			print_one_arg_instruction(AAM, new_instruction, output_stream);
			break;
		case JE:
			print_one_arg_instruction(JE, new_instruction, output_stream);
			break;
		case JL:
			print_one_arg_instruction(JL, new_instruction, output_stream);
			break;
		case JLE:
			print_one_arg_instruction(JLE, new_instruction, output_stream);
			break;
		case JB:
			print_one_arg_instruction(JB, new_instruction, output_stream);
			break;
		case JBE:
			print_one_arg_instruction(JBE, new_instruction, output_stream);
			break;
		case JP:
			print_one_arg_instruction(JP, new_instruction, output_stream);
			break;
		case JO:
			print_one_arg_instruction(JO, new_instruction, output_stream);
			break;
		case JS:
			print_one_arg_instruction(JS, new_instruction, output_stream);
			break;
		case JNE:
			print_one_arg_instruction(JNE, new_instruction, output_stream);
			break;
		case JNL:
			print_one_arg_instruction(JNL, new_instruction, output_stream);
			break;
		case JNLE:
			print_one_arg_instruction(JNLE, new_instruction, output_stream);
			break;
		case JNB:
			print_one_arg_instruction(JNB, new_instruction, output_stream);
			break;
		case JNBE:
			print_one_arg_instruction(JNBE, new_instruction, output_stream);
			break;
		case JNP:
			print_one_arg_instruction(JNP, new_instruction, output_stream);
			break;
		case JNO:
			print_one_arg_instruction(JNO, new_instruction, output_stream);
			break;
		case JNS:
			print_one_arg_instruction(JNS, new_instruction, output_stream);
			break;
		case LOOP:
			print_one_arg_instruction(LOOP, new_instruction, output_stream);
			break;
		case LOOPZ:
			print_one_arg_instruction(LOOPZ, new_instruction, output_stream);
			break;
		case LOOPNZ:
			print_one_arg_instruction(LOOPNZ, new_instruction, output_stream);
			break;
		case JCXZ:
			print_one_arg_instruction(JCXZ, new_instruction, output_stream);
			break;
		case RET:
			print_one_or_zero_arg1(RET, new_instruction,output_stream);
			break;
		case CALL:
			print_one_arg_instruction(CALL, new_instruction,output_stream);
			break;
		case JMP:
			print_one_arg_instruction(CALL, new_instruction,output_stream);
			break;
		case IN:
			print_one_or_zero_arg1(IN, new_instruction, output_stream);
			break;
		case OUT:
			print_one_or_zero_arg1(OUT, new_instruction, output_stream);
			break;
		case XLAT:
			print_special_instruction(XLAT, new_instruction, output_stream);
			break;
		case LAHF:
			print_special_instruction(LAHF, new_instruction, output_stream);
			break;
		case SAHF:
			print_special_instruction(SAHF, new_instruction, output_stream);
			break;
		case PUSHF:
			print_special_instruction(PUSHF, new_instruction, output_stream);
			break;
		case POPF:
			print_special_instruction(POPF, new_instruction, output_stream);
			break;
		case LEA:
			print_two_arg_instruction(LEA, new_instruction, output_stream);
			break;
		case LDS:
			print_two_arg_instruction(LDS, new_instruction, output_stream);
			break;
		case LES:
			print_two_arg_instruction(LES, new_instruction, output_stream);
			break;
		default: printf("No print instruction could be found. Exiting. \n"); exit(0);
	}


	free(new_instruction);

	//at this point the instruction stream has been updated.
	if (instructions->instruction_bytes[0].valid == VALID) {
		decode(instructions, assembly_file, output_stream);
	}
}
