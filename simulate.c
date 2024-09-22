#include "disassembler.h"

typedef struct virtual_machine {
	//main registers
	int8_t A[2];
	int8_t B[2];
	int8_t C[2];
	int8_t D[2];
	//index registers
	int8_t SI[2];
	int8_t DI[2];
	int8_t BP[2];
	int8_t SP[2];
	//program counter
	int8_t IP[2];
	//segment registers
	int8_t CS[2];
	int8_t DS[2];
	int8_t ES[2];
	int8_t SS[2];
	//flags
	int8_t flag_overflow;
	int8_t flag_direction;
	int8_t flag_interupt;
	int8_t flag_sign;
	int8_t flag_zero;
	int8_t flag_half_carry;
	int8_t flag_parity;
	int8_t flag_carry;
	//main memory
	int8_t memory[1048576]; 
} virtual_machine;

//note: at first I want to just run the instructions. In a later stage, the instructions should be stored in the 
//vm's memory and then be executed from there..
void initialize_virtual_machine() {

}

void print_vm_state() {

}

void run_instruction() {

}
