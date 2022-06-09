spdef: main.o oggetti.o menu.o start.o
	gcc main.c oggetti.c menu.c start.c -o spdef -lncurses -lpthread
main.o: main.c
	gcc -c main.c
oggetti.o: oggetti.c oggetti.h
	gcc -c oggetti.c
menu.o: menu.c menu.h
	gcc -c menu.c
start.o: start.c start.h
	gcc -c start.c
clean:
	rm *.o
