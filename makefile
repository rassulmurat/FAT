cc = gcc

default: compile
	$(cc) -o fat main.o
compile:
	$(cc) -c *.c
clean:
	rm -f *.o fat