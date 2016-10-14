cc = gcc

default: compile
	$(cc) -o fat main.o -lfuse 
compile:
	$(cc) -c *.c -D_FILE_OFFSET_BITS=64
debug: compiled
	$(cc) -o fat main.o
compiled:
	$(cc) -c *.c -Ddebug
clean:
	rm -f *.o fat\n