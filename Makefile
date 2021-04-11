build:
	gcc -Wall -std=c99 -o editor main.c utilitare.c undo.c interpretor_comenzi.c
clean:
	rm -f editor
