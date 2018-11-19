all	: tetris.o
	gcc tetris.c -lncurses

debug:
	gcc -g -o dd tetris.c -lncurses

clean:
	rm a.out *.o
