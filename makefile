all	: tetris.o
	gcc tetris.c -lncurses

rundb : 
	gdb dd

debug:
	gcc -g -o dd tetris.c -lncurses

clean:
	rm a.out *.o
