main : main.c
	gcc -c -Wall -Werror -fpic libseno.c
	gcc -shared -o libseno.so libseno.o
	gcc -I. -L.  -Wl,-rpath=. -Wall -o main main.c -lseno
