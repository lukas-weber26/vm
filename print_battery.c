#include "disassembler.h"

void print_battery(instruction * new_instruction, FILE * output_stream) {
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
		case CLC: 
			print_special_instruction(CLC, new_instruction, output_stream);
			break;
		case CMC: 
			print_special_instruction(CMC, new_instruction, output_stream);
			break;
		case STC: 
			print_special_instruction(STC, new_instruction, output_stream);
			break;
		case CLD: 
			print_special_instruction(CLD, new_instruction, output_stream);
			break;
		case STD: 
			print_special_instruction(STD, new_instruction, output_stream);
			break;
		case CLI: 
			print_special_instruction(CLI, new_instruction, output_stream);
			break;
		case STI: 
			print_special_instruction(STI, new_instruction, output_stream);
			break;
		case HLT: 
			print_special_instruction(HLT, new_instruction, output_stream);
			break;
		case WAIT: 
			print_special_instruction(WAIT, new_instruction, output_stream);
			break;
		case LOCK: 
			print_special_instruction(LOCK, new_instruction, output_stream);
			break;
		case INTO: 
			print_special_instruction(INTO, new_instruction, output_stream);
			break;
		case IRET: 
			print_special_instruction(IRET, new_instruction, output_stream);
			break;
		case INT:
			print_interupt(INT, new_instruction, output_stream);
			break;
		case SEGMENT:
			print_one_arg_instruction(SEGMENT, new_instruction, output_stream);
			break;
		default: printf("No print instruction could be found. Exiting. \n"); exit(0);
	}
}
