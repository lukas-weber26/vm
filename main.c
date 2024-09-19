#include "disassembler.h"

//I SUSPECT THAT THERE ARE SOME ISSUES WITH DATA TYPES. SPECIFICALLY SIGNS ON THE SHORTER INTEGER TYPES
//ALSO PRINGTING THE SHORT INTEGER TYPES IS WEIRD.

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


