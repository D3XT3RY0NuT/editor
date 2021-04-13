#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structuri.h"
#include "utilitare.h"
#include "undo.h"
#include "interpretor_comenzi.h"

//Adaugarea sirului de caracter in modul de inserare
void mod_citire(char *sir, Editor *editor){
    if (editor->undo && editor->undo->comanda[0] != 'u'){
        if (editor->cursor.caracter != editor->inceput && editor->cursor.caracter->c != '\n')
            sir[strlen(sir) - 1] = '\0';
    }
    else{
        if (editor->cursor.caracter != editor->inceput && editor->cursor.caracter->c != '\n'){
            int l = strlen(sir);
            for (int i = l; i >= 0; i--)
                sir[i + 1] = sir[i];
            sir[0] = '\n';
            sir[l + 1] = '\0';
        }
    }
    //Actualizarea stivei undo
    char *aux = convertire_numar_sir(strlen(sir));
    int l = strlen(aux);
    char *comanda = (char *) malloc(L_MAX);
    comanda[0] = 'u';
    comanda[1] = ' ';
    strcpy(comanda + 2, aux);
    comanda[l + 2] = '\n';
    comanda[l + 3] = '\0';
    inserare_stiva(&(editor->undo), editor->cursor.caracter, comanda);
    //Executarea comenzii de inserare
    editor->cursor.caracter = inserare_text(editor->cursor.caracter, sir);

    free(aux);
}

char **interpretare_comanda(char *sir, int *nr_argumente){
    //O comanda contine numele sau si maxim trei argumente
    //Primul argument al comenzii va fi intotdeauna numele sau
    char **comanda = (char **) malloc(4 * sizeof(char *));
    char *aux = (char *) malloc(L_MAX);
    int reper = 0; // Lungimea lui aux
    int ghilimele = 0; //Daca s-a intalnit o pereche de ghilimele, se asteapta si intalnirea celei de a doua
    int l = strlen(sir);

    //Curatarea caracterului '\n' de la finalul sirului
    //si adaugarea unui spatiu alb pentru prelucrare corecta a argumentelor
    if (sir[l - 1] == '\n')
        sir[--l] = '\0';
    sir[l] = ' ';
    sir[++l] = '\0';
    for (int i = 0; i < l; i++){
        if (sir[i] == ' ' && !ghilimele){
            aux[reper] = '\0';
            comanda[*nr_argumente] = (char *) malloc(strlen(aux) + 1);
            strcpy(comanda[(*nr_argumente)++], aux);
            reinitializare_sir(aux);
            reper = 0;
        }
        else if (sir[i] == '"'){
            ghilimele++;
            if (ghilimele == 2){
                ghilimele = 0;
                aux[reper] = '\0';
                comanda[*nr_argumente] = (char *) malloc(strlen(aux) + 1);
                strcpy(comanda[(*nr_argumente)++], aux);
                reinitializare_sir(aux);
                reper = 0;
            }
        }
        else
            aux[reper++] = sir[i];
    }
    free(aux);

    return comanda;
}

//Functia de revenire (backspace)
void revenire(Editor *editor){
    if (editor->cursor.caracter->c < 32)
        editor->cursor.caracter = editor->cursor.caracter->prec;
    //Actualizare stiva undo
    char *comanda_undo = (char *) malloc(6);
    comanda_undo[0] = 'i';
    comanda_undo[1] = 'b';
    comanda_undo[2] = ' ';
    comanda_undo[3] = editor->cursor.caracter->c;
    comanda_undo[4] = '\0';
    inserare_stiva(&(editor->undo), editor->cursor.caracter->prec, comanda_undo);
    //Comanda de revenire
    editor->cursor.caracter = eliminare_caracter(editor->cursor.caracter, *editor);
    editor->cursor.caracter = editor->undo->inceput_comanda;
}

//Functia de stergere a unei linii (delete line)
void stergere_linie(Editor *editor, char *argument, int nr_argumente){
    pozitie_cursor(editor);
    int indice_linie = editor->cursor.poz_linie;
    if (nr_argumente == 2)
        indice_linie = convertire_sir_numar(argument);
    NodText *elem = cautare_linie(*editor, indice_linie); //Inceputul randului care va fi sters.
    NodText *aux = elem->prec;
    //Actualizarea stivei undo
    char *comanda_undo = (char *) malloc(L_MAX);
    comanda_undo[0] = 'i';
    comanda_undo[1] = ' ';
    comanda_undo[2] = '"';
    int l = 0;
    inserare_stiva(&(editor->undo), aux, comanda_undo);
    //Executia comenzii de stergere a unei linii
    while(elem && elem->c != '\n'){
        comanda_undo[l + 3] = elem->c;
        l++;
        elem = eliminare_caracter(elem, *editor);
    }
    comanda_undo[l + 3] = '\n';
    comanda_undo[l + 4] = '"';
    comanda_undo[l + 5] = '\0';
    elem = eliminare_caracter(elem, *editor);
    //Daca cursorul se afla pe linia stearsa, acesta va fi pozitionat pe linia urmatoare
    if (editor->cursor.poz_linie == indice_linie)
        editor->cursor.caracter = aux;
}

//Functia de deplasare a cursorului la linie (goto line)
void deplasare_la_linie(Editor *editor, char *argument){
    //Crearea comenzii undo
    pozitie_cursor(editor);
    char *aux = convertire_numar_sir(editor->cursor.poz_linie);
    int l = strlen(aux);
    char *comanda_undo = (char *) malloc(L_MAX);
    comanda_undo[0] = 'm';
    comanda_undo[1] = ' ';
    strcpy(comanda_undo + 2, aux);
    free(aux);
    aux = convertire_numar_sir(editor->cursor.poz_caracter);
    comanda_undo[l + 2] = ' ';
    strcpy(comanda_undo + l + 3, aux);
    free(aux);
    //Executarea comenzii
    int indice_linie = convertire_sir_numar(argument);
    deplasare_cursor(editor, indice_linie, 0);
    //Actualizare stivei undo
    inserare_stiva(&(editor->undo), editor->cursor.caracter, comanda_undo);
}

//Functia de deplasare a cursorului la caracter (goto character)
void deplasare_la_caracter(Editor *editor, char **argumente, int nr_argumente){
    //Actualizare stiva undo
    pozitie_cursor(editor);
    char *aux = convertire_numar_sir(editor->cursor.poz_linie);
    int l = strlen(aux);
    char *comanda_undo = (char *) malloc(L_MAX);
    comanda_undo[0] = 'm';
    comanda_undo[1] = ' ';
    strcpy(comanda_undo + 2, aux);
    free(aux);
    aux = convertire_numar_sir(editor->cursor.poz_caracter);
    comanda_undo[l + 2] = ' ';
    strcpy(comanda_undo + l + 3, aux);
    free(aux);
    //Executarea comenzii
    pozitie_cursor(editor);
    int indice_caracter = convertire_sir_numar(argumente[0]);
    int indice_linie = editor->cursor.poz_linie;
    if (nr_argumente == 3)
        indice_linie = convertire_sir_numar(argumente[1]);
    deplasare_cursor(editor, indice_linie, indice_caracter);
    pozitie_cursor(editor);
    inserare_stiva(&(editor->undo), editor->cursor.caracter, comanda_undo);
}

//Functia de stergere (delete)
void stergere(Editor *editor, char *argument, int nr_argumente){
    int nr_caractere = 1;
    if (nr_argumente == 2)
        nr_caractere = convertire_sir_numar(argument);
    //Actualizare stiva undo
    char *comanda_undo = malloc(L_MAX);
    comanda_undo[0] = 'i';
    comanda_undo[1] = ' ';
    comanda_undo[2] = '"';
    generare_subsir(comanda_undo + 3, editor->cursor.caracter->urm, nr_caractere);
    comanda_undo[nr_caractere + 3] = '"';
    comanda_undo[nr_caractere + 4] = '\0';
    inserare_stiva(&(editor->undo), editor->cursor.caracter, comanda_undo);
    //Executia comenzii de stergere
    NodText *aux = editor->cursor.caracter->urm;
    while(nr_caractere--)
        aux = eliminare_caracter(aux, *editor);
}

//Functia de inlocuire a primei aparitii a cuvantului (replace)
void inlocuire_cuvant(Editor *editor, char **argumente){
    int gasit = 0; //Marcheaza daca a fost gasit cel putin un cuvant pentru a fi inlocuit
    int l1 = strlen(argumente[0]);
    int l2 = strlen(argumente[1]);
    char *subsir = (char *) malloc(L_MAX);
    NodText *aux = editor->cursor.caracter;
    while(!gasit && aux){
        generare_subsir(subsir, aux, l1);
        if (!strcmp(argumente[0], subsir)){
            //Actualizare stiva undo
            char *comanda_undo = (char *) malloc(L_MAX);
            comanda_undo[0] = 'r';
            comanda_undo[1] = ' ';
            strcpy(comanda_undo + 2, argumente[1]);
            comanda_undo[l2 + 2] = ' ';
            strcpy(comanda_undo + l2 + 3, argumente[0]);
            inserare_stiva(&(editor->undo), aux->prec, comanda_undo);
            //Inlocuirea cuvantului
            NodText *aux2 = aux;
            aux = aux->prec;
            for (int i = 0; i < l1; i++)
                aux2 = eliminare_caracter(aux2, *editor);
            inserare_text(aux, argumente[1]);
            gasit = 1;
        }
        aux = aux->urm;
    }
    if (!gasit){
        char *comanda_undo = (char *) malloc(L_MAX);
        comanda_undo[0] = 'n';
        comanda_undo[1] = '\0';
        inserare_stiva(&(editor->undo), editor->cursor.caracter, comanda_undo);
    }

    free(subsir);
}

//Functia de inlocuire a tuturor aparitiilor cuvantului (replace all)
void inlocuire_cuvant_totala(Editor *editor, char **argumente){
    int gasit = 0;
    int l1 = strlen(argumente[0]);
    int l2 = strlen(argumente[1]);
    char *subsir = (char *) malloc(L_MAX);
    NodText *aux = editor->inceput->urm;
    while(aux){
        generare_subsir(subsir, aux, l1);
        if (!strcmp(argumente[0], subsir)){
            //Actualizare stiva undo
            char *comanda_undo = (char *) malloc(L_MAX);
            comanda_undo[0] = 'r';
            comanda_undo[1] = 'r';
            comanda_undo[2] = ' ';
            strcpy(comanda_undo + 3, argumente[1]);
            comanda_undo[l2 + 3] = ' ';
            strcpy(comanda_undo + l2 + 4, argumente[0]);
            inserare_stiva(&(editor->undo), aux->prec, comanda_undo);
            //Inlocuirea totala a cuvantului
            NodText *aux2 = aux;
            aux = aux->prec;
            for (int i = 0; i < l1; i++)
                aux2 = eliminare_caracter(aux2, *editor);
            inserare_text(aux, argumente[1]);
            gasit = 1;
        }
        aux = aux->urm;
    }
    if (!gasit){
        char *comanda_undo = (char *) malloc(L_MAX);
        comanda_undo[0] = 'n';
        comanda_undo[1] = '\0';
        inserare_stiva(&(editor->undo), editor->cursor.caracter, comanda_undo);
    }

    free(subsir);
}

//Functia de stergere a primei aparitii a cuvantului (delete word)
void stergere_cuvant(Editor *editor, char *argument){
    int gasit = 0;
    int l = strlen(argument);
    char *subsir = (char *) malloc(L_MAX);
    NodText *aux = editor->cursor.caracter;
    while(!gasit && aux){
        generare_subsir(subsir, aux, l);
        if (!strcmp(argument, subsir)){
            //Actualizare stiva undo
            char *comanda_undo = (char *) malloc(L_MAX);
            comanda_undo[0] = 'i';
            comanda_undo[1] = 's';
            comanda_undo[2] = ' ';
            strcpy(comanda_undo + 3, subsir);
            inserare_stiva(&(editor->undo), aux->prec, comanda_undo);
            //Stergere cuvant
            NodText *aux2 = aux;
            aux = aux->prec;
            for (int i = 0; i < l; i++)
                aux2 = eliminare_caracter(aux2, *editor);
            gasit = 1;
        }
        aux = aux->urm;
    }
    if (!gasit){
        char *comanda_undo = (char *) malloc(L_MAX);
        comanda_undo[0] = 'n';
        comanda_undo[1] = '\0';
        inserare_stiva(&(editor->undo), editor->cursor.caracter, comanda_undo);
    }

    free(subsir);
}

//Functia de stergere a tuturor aparitiei cuvantului (delete all)
void stergere_cuvant_totala(Editor *editor, char *argument){
    int gasit = 0;
    int l = strlen(argument);
    char *subsir = (char *) malloc(L_MAX);
    NodText *aux = editor->inceput->urm;
    while(aux){
        generare_subsir(subsir, aux, l);
        if (!strcmp(argument, subsir)){
            //Actualizare stiva undo
            char *comanda_undo = (char *) malloc(L_MAX);
            comanda_undo[0] = 'i';
            comanda_undo[1] = 'r';
            comanda_undo[2] = ' ';
            strcpy(comanda_undo + 3, subsir);
            inserare_stiva(&(editor->undo), aux->prec, comanda_undo);
            //Stergerea totala a cuvantului
            NodText *aux2 = aux;
            aux = aux->prec;
            for (int i = 0; i < l; i++)
                aux2 = eliminare_caracter(aux2, *editor);
            gasit = 1;
        }
        aux = aux->urm;
    }
    if (!gasit){
        char *comanda_undo = (char *) malloc(L_MAX);
        comanda_undo[0] = 'n';
        comanda_undo[1] = '\0';
        inserare_stiva(&(editor->undo), editor->cursor.caracter, comanda_undo);
    }

    free(subsir);
}

void mod_comanda(char *sir, Editor *editor, char *fisier_out){
    int nr_argumente = 0;
    char **comanda = interpretare_comanda(sir, &nr_argumente);
    if (!strcmp(comanda[0], "u"))
        undo(editor);
    else if (!strcmp(comanda[0], "r"))
        redo(editor);
    else if (!strcmp(comanda[0], "s")){
        FILE *f_out = fopen(fisier_out, "w");
        if (!f_out){
            printf("Fisierul de scriere nu a putut fi deschis!\n");
            return;
        }
        imprimare(f_out, *editor);
        fclose(f_out);
        vidare_stiva(&(editor->undo));
        vidare_stiva(&(editor->redo));
    }
    else if (!strcmp(comanda[0], "q")){
        pozitie_cursor(editor);
        //Toate comenzile nesalvate sunt revocate
        while(editor->undo)
            undo(editor);
        FILE *f_out = fopen(fisier_out, "w");
        if (!f_out){
            printf("Fisierul de scriere nu a putut fi deschis!\n");
            return;
        }
        imprimare(f_out, *editor);
        fclose(f_out);
        vidare_stiva(&(editor->redo));

    }
    else if (!strcmp(comanda[0], "b"))
        revenire(editor);
    else if (!strcmp(comanda[0], "dl"))
        stergere_linie(editor, comanda[1], nr_argumente);
    else if (!strcmp(comanda[0], "gl"))
        deplasare_la_linie(editor, comanda[1]);
    else if (!strcmp(comanda[0], "gc"))
        deplasare_la_caracter(editor, comanda + 1, nr_argumente);
    else if (!strcmp(comanda[0], "d"))
        stergere(editor, comanda[1], nr_argumente);
    else if (!strcmp(comanda[0], "re"))
        inlocuire_cuvant(editor, comanda + 1);
    else if (!strcmp(comanda[0], "ra"))
        inlocuire_cuvant_totala(editor, comanda + 1);
    else if (!strcmp(comanda[0], "dw"))
        stergere_cuvant(editor, comanda[1]);
    else if (!strcmp(comanda[0], "da"))
        stergere_cuvant_totala(editor, comanda[1]);

    for (int i = 0; i < nr_argumente; i++)
        free(comanda[i]);
    free(comanda);
}

