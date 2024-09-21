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
	decoder_battery(new_instruction, instructions, assembly_file);
	print_battery(new_instruction, assembly_file);
	//print the instruction
	
	free(new_instruction);

	//at this point the instruction stream has been updated.
	if (instructions->instruction_bytes[0].valid == VALID) {
		decode(instructions, assembly_file, output_stream);
	}
}
