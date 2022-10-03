assembler: assembler.o first_pass.o second_pass.o
	gcc -g -ansi -Wall -pedantic assembler.o first_pass.o second_pass.o -o assembler
assembler.o: assembler.c assembler.h utils.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o	
first_pass.o: first_pass.c first_pass.h utils.h
	gcc -c -ansi -Wall -pedantic first_pass.c -o first_pass.o
second_pass.o: second_pass.c second_pass.h utils.h
	gcc -c -ansi -Wall -pedantic second_pass.c -o second_pass.o
