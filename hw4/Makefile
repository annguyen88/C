CC=gcc
CFLAGS=-ansi -pedantic -Wall


fibonacci: fibonacci.c fib.s fib.h
	${CC} ${CFLAGS} -o fibonacci fibonacci.c fib.s 

tar:
	tar -cf pa2.tar fibonacci.c fib.h fib.s Makefile Readme.pdf

clean:
	rm -rf *.o

