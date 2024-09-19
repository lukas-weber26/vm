disassembler: _main.o _decode.o _print.o _instruction_stream.o
	cc -g _main.o _decode.o _print.o _instruction_stream.o -o disassembler
	rm *.o
	cp ./disassembler ./tests/

_main.o: main.c
	cc -c -g main.c -o _main.o

_decode.o: decode.c
	cc -c -g decode.c -o _decode.o

_print.o: print.c
	cc -c -g print.c -o _print.o

_instruction_stream.o: instruction_stream.c
	cc -c -g instruction_stream.c -o _instruction_stream.o

clean: 
	rm *.o output
