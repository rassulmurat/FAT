cc = gcc

default: compile
	$(cc) -o fat main.o
compile:
	$(cc) -c *.c
debug: compiled
	$(cc) -o fat main.o
compiled:
	$(cc) -c *.c -Ddebug
clean:
	rm -f *.o fat