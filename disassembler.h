#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#define FALSE 0
#define TRUE 1

typedef enum {MOV, PUSH, POP, XCHG, ADD, ADC, SUB, SBB, AND, TEST, OR, XOR, NOT, SHL, SHR, SAR, ROL, ROR, RCL, RCR, REP, MOVS, CMPS, SCAS, LODS, STDS, INC, DEC, NEG, AAA, DAA, CMP, AAS, DAS, CBW, CWD, MUL, IMUL, AAM, DIV, IDIV, AAD, RET, JE, JL, JLE, JB, JBE, JP, JO, JS, JNE, JNL, JNLE, JJNB, JNBE, JNP, JNO, JNS, LOOP, LOOPZ, LOOPNZ, JCXZ, JNB, JMP, CALL, IN, OUT, XLAT, LEA, LDS, LES, LAHF, SAHF, PUSHF, POPF, INT, INTO, IRERT, CLC, CMC, STC, CLD, STD, CLI, STI, HLT, WAIT, ESC, LOCK, SEGMENT, IRET} instruction_type; //,PUSH, POP
typedef enum {MEM, MEM_8, MEM_16, REG, SEG, ACC, IM8, IM16, DIRECT, NONE, CL, IP_INC8, IP, IP_INC16} target;
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

//stream and byte manipulation
instruction_byte get_next_instruction_byte(FILE * assembly_file);
instruction_stream * initialize_instruction_stream(FILE * assembly_file);
int match_instruction_to_stream(char * first_byte, char * second_byte, instruction_stream * stream);
void instruction_stream_pop_byte(instruction_stream * stream, FILE * assembly_file);
void instructin_stream_pop_n_bytes(instruction_stream * stream, FILE * assembly_file, int n_increments);
int match_byte(instruction_byte byte, char * match_string);
uint8_t mask(uint8_t byte, uint8_t mask, uint8_t shift);

//printing
void print_battery(instruction * new_instruction, FILE * output_stream);
void print_two_arg_instruction(instruction_type type,instruction * new_instruction, FILE * output_stream);
void print_one_arg_instruction(instruction_type type,instruction * new_instruction, FILE * output_stream);
void print_zero_arg_instruction(instruction_type type,instruction * new_instruction, FILE * output_stream);
void print_v_arg_instruction(instruction_type type,instruction * new_instruction, FILE * output_stream);
void print_special_instruction(instruction_type type,instruction * new_instruction, FILE * output_stream);
void print_one_or_zero_arg1(instruction_type type, instruction * new_instruction , FILE * output_stream);
void print_esc(instruction_type type, instruction * new_instruction , FILE * output_stream);
void print_interupt(instruction_type type, instruction * new_instruction , FILE * output_stream);

//decoding
void decoder_battery(instruction * new_instruction, instruction_stream *instructions, FILE * assembly_file);
void decode(instruction_stream * instructions, FILE * assembly_file, FILE * output_stream);
void decode_regmem_to_regmem(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);

//decoders...
void decode_regmem_to_regmem(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);
void decode_imediate_to_regmem(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);
void decode_imediate_to_reg(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);
void decode_mem_to_acc(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file, source_inversion inverted);
void decode_rm_to_seg(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file, source_inversion inverted);
void decode_rm(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);
void decode_reg(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);
void decode_seg(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);
void decode_reg_to_acc(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file, source_inversion inversion);
void decode_signed_imediate_to_regmem(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);
void decode_imediate_to_acc(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);
void decode_imediate_to_acc_short(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);
void decode_regmem(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file, int v_present);
void decode_z(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file) ;
void decode_w(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);
void decode_none(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);
void decode_jump(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file );
void decode_data(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file );
void decode_ip_inc(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file, int short_inc);
void decode_data_8(instruction_type type, instruction_stream * instructions, instruction * new_instruction, FILE * assembly_file);
