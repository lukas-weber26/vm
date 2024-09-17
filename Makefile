disassembler: _disassembler.o
	cc -g _disassembler.o -o disassembler
	rm *.o

_disassembler.o: main.c
	cc -c -g main.c -o _disassembler.o

.SILENT:
test_basic: disassembler
	nasm ./test1.asm
	./disassembler ./test1 test1_output.txt
	echo "Test one errors:"
	diff ./test1.asm test1_output.txt
	rm ./test1_output.txt
	rm ./test1

test_mov_reg_reg: disassembler
	nasm ./test2.asm
	./disassembler ./test2 test2_output.txt
	echo "Test two errors:"
	diff ./test2.asm test2_output.txt
	rm ./test2_output.txt
	rm ./test2

test: test_basic test_mov_reg_reg
	echo "testing_complete"	

clean: 
	rm *.o output
