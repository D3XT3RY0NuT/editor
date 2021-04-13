#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structuri.h"
#include "utilitare.h"
#include "undo.h"
#include "interpretor_comenzi.h"

void executare(char *fisier_in, char *fisier_out, Editor *editorul_meu){
    FILE *f_in = fopen(fisier_in, "r");
    if (!f_in){
        printf("Fisierul de citire nu a putut fi deschis!\n");
        return;
    }

    char *sir = (char *) malloc(L_MAX);
    char *aux = (char *) malloc(L_MAX);
    char **comanda;
    int nr_argumente;
    int mod = 0; // mod == 0 ==> mod citire text / mod == 1 ==> mod editare text
    while(fgets(sir, L_MAX, f_in)){
        nr_argumente = 0;
        strcpy(aux, sir);
        comanda = interpretare_comanda(aux, &nr_argumente);
        if (!strcmp(comanda[0], "::i"))
            mod = 1 - mod;
        else if (mod)
            mod_comanda(sir, editorul_meu, fisier_out);
        else
            mod_citire(sir, editorul_meu);
        for (int i = 0; i < nr_argumente; i++)
        	free(comanda[i]);
        free(comanda);
    }
    free(sir);
    free(aux);
    fclose(f_in);
}

int main(int argc, char *argv[])
{
    Editor *editorul_meu = initializare_editor();
    executare("editor.in", "editor.out", editorul_meu);
    inchidere_editor(editorul_meu);

    return 0;
}
