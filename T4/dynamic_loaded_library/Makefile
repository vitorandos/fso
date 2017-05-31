main : main.c
	gcc -c -Wall -Werror -fpic libseno.c
	gcc -shared -o libseno.so libseno.o
	gcc -I. -L. -rdynamic -Wall -o main main.c -ldl
