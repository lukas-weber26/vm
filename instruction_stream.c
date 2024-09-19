#include "disassembler.h"

instruction_byte get_next_instruction_byte(FILE * assembly_file) {
	instruction_byte new_byte;
	int readable_int;

	if ((readable_int = getc(assembly_file)) == EOF) {
		new_byte.valid = INVALID;
		new_byte.byte = 0;
		assembly_file -= sizeof(char);
		return new_byte;
	}

	new_byte.valid = VALID;
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
