disassembler: _main.o _decode.o _print.o _instruction_stream.o _decoder_battery.o _print_battery.o
	cc -g _main.o _decode.o _print.o _instruction_stream.o -o disassembler _print_battery.o _decoder_battery.o
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

_print_battery.o: print_battery.c
	cc -c -g print_battery.c -o _print_battery.o

_decoder_battery.o: decoder_battery.c
	cc -c -g decoder_battery.c -o _decoder_battery.o


clean: 
	rm *.o output
