#include "disassembler.h"

void decoder_battery(instruction * new_instruction, instruction_stream *instructions, FILE * assembly_file) {
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

	//clc
	} else if (match_instruction_to_stream("11111000", NULL, instructions)) {
		decode_none(CLC, instructions, new_instruction, assembly_file); 
	//cmc
	} else if (match_instruction_to_stream("11110101", NULL, instructions)) {
		decode_none(CMC, instructions, new_instruction, assembly_file); 
	//stc
	} else if (match_instruction_to_stream("11111001", NULL, instructions)) {
		decode_none(STC, instructions, new_instruction, assembly_file); 
	//cld
	} else if (match_instruction_to_stream("11111100", NULL, instructions)) {
		decode_none(CLD, instructions, new_instruction, assembly_file); 
	//std
	} else if (match_instruction_to_stream("11111101", NULL, instructions)) {
		decode_none(STD, instructions, new_instruction, assembly_file); 
	//cli
	} else if (match_instruction_to_stream("11111010", NULL, instructions)) {
		decode_none(CLI, instructions, new_instruction, assembly_file); 
	//sti
	} else if (match_instruction_to_stream("11111011", NULL, instructions)) {
		decode_none(STI, instructions, new_instruction, assembly_file); 
	//hlt
	} else if (match_instruction_to_stream("11110100", NULL, instructions)) {
		decode_none(HLT, instructions, new_instruction, assembly_file); 
	//wait
	} else if (match_instruction_to_stream("10011011", NULL, instructions)) {
		decode_none(WAIT, instructions, new_instruction, assembly_file); 
	//lock
	} else if (match_instruction_to_stream("11110000", NULL, instructions)) {
		decode_none(LOCK, instructions, new_instruction, assembly_file); 
	//into
	} else if (match_instruction_to_stream("11001110", NULL, instructions)) {
		decode_none(INTO, instructions, new_instruction, assembly_file); 
	//IRET
	} else if (match_instruction_to_stream("11001111", NULL, instructions)) {
		decode_none(IRET, instructions, new_instruction, assembly_file); 

	//int specified type
	} else if (match_instruction_to_stream("11001101", NULL, instructions)) {
		decode_data_8(INT, instructions, new_instruction, assembly_file); 
	//int type 3
	} else if (match_instruction_to_stream("11001100", NULL, instructions)) {
		decode_none(INT, instructions, new_instruction, assembly_file); 
	//segment
	} else if (match_instruction_to_stream("001xx110", NULL, instructions)) {
		decode_seg(SEGMENT, instructions, new_instruction, assembly_file); 
	//make the top three here work first 
	
	//esc
	} else if (match_instruction_to_stream("11011xxx", NULL, instructions)) {
		//decode_esc(ESC, instructions, new_instruction, assembly_file);  //this one is an L
	
	} else {
		printf("Opcode not understood.\n");
		exit(0);
	}
}
