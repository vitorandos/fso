main : main.c
	gcc -c libseno.c
	ar -cvq libseno.a libseno.o
	gcc -o main main.c -I. -L. -lseno
