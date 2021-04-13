#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structuri.h"
#include "utilitare.h"
#include "undo.h"
#include "interpretor_comenzi.h"

Stiva *initializare_stiva(){
    Stiva *stiva = NULL;

    return stiva;
}

void inserare_stiva(Stiva **stiva, NodText *inceput_comanda, char *comanda){
    Stiva *aux = (Stiva *) malloc(sizeof(Stiva));
    aux->inceput_comanda = inceput_comanda;
    aux->comanda = comanda;
    aux->urm = *stiva;
    *stiva = aux;
}

void eliminare_stiva(Stiva **stiva){
    if (*stiva){
        Stiva *aux = *stiva;
        *stiva = (*stiva)->urm;
        free(aux->comanda);
        free(aux);
        aux = NULL;
    }
}

void vidare_stiva(Stiva **stiva){
    while (*stiva)
        eliminare_stiva(stiva);
}

void undo(Editor *editor){
    int nr_argumente = 0;
    char **comanda = interpretare_comanda(editor->undo->comanda, &nr_argumente);
    if (!strcmp(comanda[0], "ib")){ //Inversul comenzii de revenire
        //Executia comenzii
        editor->cursor.caracter = inserare_caracter(editor->cursor.caracter, comanda[1][0]);
        //Actualizare stiva redo
        char *comanda_redo = (char *) malloc(3);
        comanda_redo[0] = 'b';
        comanda_redo[1] = '\n';
        comanda_redo[2] = '\0';
        inserare_stiva(&(editor->redo), editor->cursor.caracter, comanda_redo);
    }
    else if (!strcmp(comanda[0], "u")){ //Inversul unei inserari in modul de citire
        if (editor->undo->urm){
            int nr_argumente_urm = 0;
            char **comanda_urm = interpretare_comanda(editor->undo->urm->comanda, &nr_argumente_urm);
            while(nr_argumente_urm && !strcmp(comanda_urm[0], "u")){
                char *comanda_redo = (char *) malloc(L_MAX);
                comanda_redo[0] = 'i';
                comanda_redo[1] = ' ';
                comanda_redo[2] = '"';
                int l = convertire_sir_numar(comanda[1]);
                generare_subsir(comanda_redo + 3, editor->undo->inceput_comanda->urm, l);
                comanda_redo[l + 3] = '"';
                comanda_redo[l + 4] = '\n';
                comanda_redo[l + 5] = '\0';
                inserare_stiva(&(editor->redo), editor->undo->inceput_comanda, comanda_redo);
                //Executia comenzii
                editor->cursor.caracter = editor->undo->inceput_comanda;
                NodText *aux = editor->undo->inceput_comanda->urm;
                while(l--)
                    aux = eliminare_caracter(aux, *editor);
                if (!editor->cursor.caracter)
                    editor->cursor.caracter = editor->undo->inceput_comanda;
                //Stergere recursiva pana la terminarea tuturor comenzilor "i"
                for (int i = 0; i < nr_argumente; i++)
                    free(comanda[i]);
                free(comanda);
                comanda = comanda_urm;
                nr_argumente = nr_argumente_urm;
                nr_argumente_urm = 0;
                eliminare_stiva(&(editor->undo));
                if (editor->undo->urm)
                    comanda_urm = interpretare_comanda(editor->undo->urm->comanda, &nr_argumente_urm);
            }
        }
        //Actualizare stiva redo
        char *comanda_redo = (char *) malloc(L_MAX);
        comanda_redo[0] = 'i';
        comanda_redo[1] = ' ';
        comanda_redo[2] = '"';
        int l = convertire_sir_numar(comanda[1]);
        generare_subsir(comanda_redo + 3, editor->undo->inceput_comanda->urm, l);
        comanda_redo[l + 3] = '"';
        comanda_redo[l + 4] = '\n';
        comanda_redo[l + 5] = '\0';
        inserare_stiva(&(editor->redo), editor->undo->inceput_comanda, comanda_redo);
        //Executia comenzii
        editor->cursor.caracter = editor->undo->inceput_comanda;
        NodText *aux = editor->undo->inceput_comanda->urm;
        while(l--)
            aux = eliminare_caracter(aux, *editor);
        if (!editor->cursor.caracter)
            editor->cursor.caracter = editor->undo->inceput_comanda;
    }
    else if (!strcmp(comanda[0], "i")){ //Inversul unei stergeri dinamice (cu modificarea pozitiei cursorului)
        //Actualizare stiva redo
        char *comanda_redo = (char *) malloc(L_MAX);
        comanda_redo[0] = 'd';
        comanda_redo[1] = ' ';
        char *aux = convertire_numar_sir(strlen(comanda[1]));
        int l = strlen(aux);
        for (int i = 0; i < l; i++)
            comanda_redo[i + 2] = aux[i];
        comanda_redo[l + 2] = '\n';
        comanda_redo[l + 3] = '\0';
        inserare_stiva(&(editor->redo), editor->undo->inceput_comanda, comanda_redo);
        free(aux);
        //Executia comenzii
        if (editor->cursor.caracter == editor->undo->inceput_comanda)
            editor->cursor.caracter = inserare_text(editor->undo->inceput_comanda, comanda[1]);
        else
            inserare_text(editor->undo->inceput_comanda, comanda[1]);
    }
    else if (!strcmp(comanda[0], "is")){ //Inversul unei stergeri statice (fara modificarea pozitiei cursorului
        //Actualizare stiva redo
        char *comanda_redo = (char *) malloc(L_MAX);
        comanda_redo[0] = 'd';
        comanda_redo[1] = ' ';
        char *aux = convertire_numar_sir(strlen(comanda[1]));
        int l = strlen(aux);
        for (int i = 0; i < l; i++)
            comanda_redo[i + 2] = aux[i];
        comanda_redo[l + 2] = '\n';
        comanda_redo[l + 3] = '\0';
        inserare_stiva(&(editor->redo), editor->undo->inceput_comanda, comanda_redo);
        free(aux);
        //Executia comenzii
        inserare_text(editor->undo->inceput_comanda, comanda[1]);
    }
    else if (!strcmp(comanda[0], "ir")){ //Inversul unei stergeri recursive
        int nr_argumente_urm = 0;
        char **comanda_urm = interpretare_comanda(editor->undo->urm->comanda, &nr_argumente_urm);
        while(!strcmp(comanda_urm[0], "ir")){
            //Actualizare stiva redo
            char *comanda_redo = (char *) malloc(L_MAX);
            comanda_redo[0] = 'd';
            comanda_redo[1] = 'r';
            comanda_redo[2] = ' ';
            char *aux = convertire_numar_sir(strlen(comanda[1]));
            int l = strlen(aux);
            for (int i = 0; i < l; i++)
                comanda_redo[i + 3] = aux[i];
            comanda_redo[l + 3] = '\n';
            comanda_redo[l + 4] = '\0';
            inserare_stiva(&(editor->redo), editor->undo->inceput_comanda, comanda_redo);
            free(aux);
            //Executia comenzii
            inserare_text(editor->undo->inceput_comanda, comanda[1]);
            //Inserare recursiva pana la terminarea tuturor comenzilor "ir"
            for (int i = 0; i < nr_argumente; i++)
                free(comanda[i]);
            free(comanda);
            eliminare_stiva(&(editor->undo));
            comanda = comanda_urm;
            nr_argumente = nr_argumente_urm;
            nr_argumente_urm = 0;
            comanda_urm = interpretare_comanda(editor->undo->urm->comanda, &nr_argumente_urm);
        }
        //Ultima inserare
        //Actualizare stiva redo
        char *comanda_redo = (char *) malloc(L_MAX);
        comanda_redo[0] = 'd';
        comanda_redo[1] = 'r';
        comanda_redo[2] = ' ';
        char *aux = convertire_numar_sir(strlen(comanda[1]));
        int l = strlen(aux);
        for (int i = 0; i < l; i++)
            comanda_redo[i + 3] = aux[i];
        comanda_redo[l + 3] = '\n';
        comanda_redo[l + 4] = '\0';
        inserare_stiva(&(editor->redo), editor->undo->inceput_comanda, comanda_redo);
        free(aux);
        //Executia comenzii
        inserare_text(editor->undo->inceput_comanda, comanda[1]);
    }
    else if (!strcmp(comanda[0], "r")){ //Inversul unei inlocuire simple
        int l1 = strlen(comanda[1]);
        int l2 = strlen(comanda[2]);
        //Actualizare stiva redo
        char *comanda_redo = (char *) malloc(L_MAX);
        comanda_redo[0] = 'r';
        comanda_redo[1] = ' ';
        strcpy(comanda_redo + 2, comanda[2]);
        comanda_redo[l2 + 2] = ' ';
        strcpy(comanda_redo + l2 + 3, comanda[1]);
        inserare_stiva(&(editor->redo), editor->undo->inceput_comanda, comanda_redo);
        //Inlocuirea cuvantului
        NodText *aux = editor->undo->inceput_comanda->urm;
        for (int i = 0; i < l1; i++)
            aux = eliminare_caracter(aux, *editor);
        inserare_text(editor->undo->inceput_comanda, comanda[2]);
    }
    else if (!strcmp(comanda[0], "rr")){ //Inversul unei inlocuiri recursive
        int nr_argumente_urm = 0;
        char **comanda_urm = interpretare_comanda(editor->undo->urm->comanda, &nr_argumente_urm);
        while(!strcmp(comanda_urm[0], "rr")){
            int l1 = strlen(comanda[1]);
            int l2 = strlen(comanda[2]);
            //Actualizare stiva redo
            char *comanda_redo = (char *) malloc(L_MAX);
            comanda_redo[0] = 'r';
            comanda_redo[1] = 'r';
            comanda_redo[2] = ' ';
            strcpy(comanda_redo + 3, comanda[2]);
            comanda_redo[l2 + 3] = ' ';
            strcpy(comanda_redo + l2 + 4, comanda[1]);
            inserare_stiva(&(editor->redo), editor->undo->inceput_comanda, comanda_redo);
            //Executia comenzii
            NodText *aux = editor->undo->inceput_comanda->urm;
            for (int i = 0; i < l1; i++)
                aux = eliminare_caracter(aux, *editor);
            inserare_text(editor->undo->inceput_comanda, comanda[2]);
            //Inlocuirea recursiva pana la terminarea tuturor comenzilor "rr"
            eliminare_stiva(&(editor->undo));
            for (int i = 0; i < nr_argumente; i++)
                free(comanda[i]);
            free(comanda);
            nr_argumente = nr_argumente_urm;
            comanda = comanda_urm;
            comanda_urm = interpretare_comanda(editor->undo->urm->comanda, &nr_argumente_urm);
        }
        int l1 = strlen(comanda[1]);
        int l2 = strlen(comanda[2]);
        //Actualizare stiva redo
        char *comanda_redo = (char *) malloc(L_MAX);
        comanda_redo[0] = 'r';
        comanda_redo[1] = 'r';
        comanda_redo[2] = ' ';
        strcpy(comanda_redo + 3, comanda[2]);
        comanda_redo[l2 + 3] = ' ';
        strcpy(comanda_redo + l2 + 4, comanda[1]);
        inserare_stiva(&(editor->redo), editor->undo->inceput_comanda, comanda_redo);
        //Executia comenzii
        NodText *aux = editor->undo->inceput_comanda->urm;
        for (int i = 0; i < l1; i++)
            aux = eliminare_caracter(aux, *editor);
        inserare_text(editor->undo->inceput_comanda, comanda[2]);
    }
    else if (!strcmp(comanda[0], "m")){ //Inversul unei deplasari de cursor
        //Crearea comenzii redo
        editor->cursor.caracter = editor->undo->inceput_comanda;
        pozitie_cursor(editor);
        char *aux = convertire_numar_sir(editor->cursor.poz_linie);
        int l = strlen(aux);
        char *comanda_redo = (char *) malloc(L_MAX);
        comanda_redo[0] = 'm';
        comanda_redo[1] = ' ';
        strcpy(comanda_redo + 2, aux);
        free(aux);
        aux = convertire_numar_sir(editor->cursor.poz_caracter);
        comanda_redo[l + 2] = ' ';
        strcpy(comanda_redo + l + 3, aux);
        free(aux);
        //Executia comenzii
        int poz_linie = convertire_sir_numar(comanda[1]);
        int poz_caracter = convertire_sir_numar(comanda[2]);
        deplasare_cursor(editor, poz_linie, poz_caracter);
        //Actualizare stiva redo
        inserare_stiva(&(editor->redo), editor->cursor.caracter, comanda_redo);
    }
    else{ // Comanda NULL, corespunzatoare unei comenzi fara efect, ex: inlocuirea unui cuvant inexistent
        char *comanda_redo = (char *) malloc(L_MAX);
        comanda_redo[0] = 'n';
        comanda_redo[1] = '\0';
        inserare_stiva(&(editor->redo), editor->cursor.caracter, comanda_redo);
    }

    for (int i = 0; i < nr_argumente; i++)
        free(comanda[i]);
    free(comanda);
    eliminare_stiva(&(editor->undo));
}

void redo(Editor *editor){
    int nr_argumente = 0;
    char **comanda = interpretare_comanda(editor->redo->comanda, &nr_argumente);
    if (!strcmp(comanda[0], "b")) //Comanda de revenire
        revenire(editor);
    else if (!strcmp(comanda[0], "i")){ //Inserare
        if (editor->redo->urm){
            int nr_argumente_urm = 0;
            char **comanda_urm = interpretare_comanda(editor->redo->urm->comanda, &nr_argumente_urm);
            while (nr_argumente_urm && !strcmp(comanda_urm[0], "i")){
                //Actualizare stivei undo
                char *comanda_undo = (char *) malloc(L_MAX);
                comanda_undo[0] = 'u';
                comanda_undo[1] = ' ';
                char *aux = convertire_numar_sir(strlen(comanda[1]));
                int l = strlen(aux);
                strcpy(comanda_undo + 2, aux);
                comanda_undo[l + 2] = '\0';
                inserare_stiva(&(editor->undo), editor->redo->inceput_comanda, comanda_undo);
                //Executia comenzii
                editor->cursor.caracter = inserare_text(editor->cursor.caracter, comanda[1]);
                free(aux);
                //Inserare recursiva
                for (int i = 0; i < nr_argumente; i++)
                    free(comanda[i]);
                free(comanda);
                comanda = comanda_urm;
                nr_argumente = nr_argumente_urm;
                nr_argumente_urm = 0;
                eliminare_stiva(&(editor->redo));
                if (editor->redo->urm)
                    comanda_urm = interpretare_comanda(editor->redo->urm->comanda, &nr_argumente_urm);
            }
        }
        //Actualizare stivei undo
        char *comanda_undo = (char *) malloc(L_MAX);
        comanda_undo[0] = 'u';
        comanda_undo[1] = ' ';
        char *aux = convertire_numar_sir(strlen(comanda[1]));
        int l = strlen(aux);
        strcpy(comanda_undo + 2, aux);
        comanda_undo[l + 2] = '\0';
        inserare_stiva(&(editor->undo), editor->redo->inceput_comanda, comanda_undo);
        //Executia comenzii
        editor->cursor.caracter = inserare_text(editor->cursor.caracter, comanda[1]);
        free(aux);
    }
    else if (!strcmp(comanda[0], "d")){ //Stergere
        NodText *aux = editor->cursor.caracter;
        editor->cursor.caracter = editor->redo->inceput_comanda;
        stergere(editor, comanda[1], nr_argumente);
        if (aux)
            editor->cursor.caracter = aux;
    }
    else if (!strcmp(comanda[0], "dr")){ //Stergere recursiva
        if (editor->redo->urm){
            int nr_argumente_urm = 0;
            char **comanda_urm = interpretare_comanda(editor->redo->urm->comanda, &nr_argumente_urm);
            while(!strcmp(comanda_urm[0], "dr")){
                NodText *aux = editor->cursor.caracter;
                editor->cursor.caracter = editor->redo->inceput_comanda;
                stergere(editor, comanda[1], nr_argumente);
                if (aux)
                    editor->cursor.caracter = aux;
                //Se continua verificarea comenzii urmatoare
                for (int i = 0; i < nr_argumente; i++)
                    free(comanda[i]);
                free(comanda);
                eliminare_stiva(&(editor->redo));
                nr_argumente = nr_argumente_urm;
                comanda = comanda_urm;
                nr_argumente_urm = 0;
                if (editor->redo->urm)
                    comanda_urm = interpretare_comanda(editor->redo->urm->comanda, &nr_argumente);
                else
                    comanda_urm[0] = "NU EXISTA\n";
            }
        }
        //Ultima stergere
        NodText *aux = editor->cursor.caracter;
        editor->cursor.caracter = editor->redo->inceput_comanda;
        stergere(editor, comanda[1], nr_argumente);
        if (aux)
            editor->cursor.caracter = aux;
    }
    else if (!strcmp(comanda[0], "r")){ //Inlocuire
        int l1 = strlen(comanda[1]);
        int l2 = strlen(comanda[2]);
        //Actualizare stiva undo
        char *comanda_undo = (char *) malloc(L_MAX);
        comanda_undo[0] = 'r';
        comanda_undo[1] = ' ';
        strcpy(comanda_undo + 2, comanda[2]);
        comanda_undo[l2 + 2] = ' ';
        strcpy(comanda_undo + l2 + 3, comanda[1]);
        inserare_stiva(&(editor->undo), editor->redo->inceput_comanda, comanda_undo);
        //Executia comenzii
        NodText *aux = editor->redo->inceput_comanda->urm;
        for (int i = 0; i < l1; i++)
            aux = eliminare_caracter(aux, *editor);
        inserare_text(editor->redo->inceput_comanda, comanda[2]);
    }
    else if (!strcmp(comanda[0], "rr")){ //Inlocuire recursiva
        if (editor->redo->urm){
            int nr_argumente_urm = 0;
            char **comanda_urm = interpretare_comanda(editor->redo->urm->comanda, &nr_argumente_urm);
            while(editor->redo && !strcmp(comanda_urm[0], "rr")){
                int l1 = strlen(comanda[1]);
                int l2 = strlen(comanda[2]);
                //Actualizare stiva undo
                char *comanda_undo = (char *) malloc(L_MAX);
                comanda_undo[0] = 'r';
                comanda_undo[1] = 'r';
                comanda_undo[2] = ' ';
                strcpy(comanda_undo + 3, comanda[2]);
                comanda_undo[l2 + 3] = ' ';
                strcpy(comanda_undo + l2 + 4, comanda[1]);
                inserare_stiva(&(editor->undo), editor->redo->inceput_comanda, comanda_undo);
                //Executia comenzii
                NodText *aux = editor->redo->inceput_comanda->urm;
                for (int i = 0; i < l1; i++)
                    aux = eliminare_caracter(aux, *editor);
                inserare_text(editor->redo->inceput_comanda, comanda[2]);
                //Inlocuire recursiva
                for (int i = 0; i < nr_argumente; i++)
                    free(comanda[i]);
                free(comanda);
                eliminare_stiva(&(editor->redo));
                nr_argumente = nr_argumente_urm;
                comanda = comanda_urm;
                nr_argumente_urm = 0;
                if (editor->redo->urm)
                    comanda_urm = interpretare_comanda(editor->redo->urm->comanda, &nr_argumente_urm);
                else
                    comanda_urm[0] = "NU EXISTA\n";
            }
        }
        int l1 = strlen(comanda[1]);
        int l2 = strlen(comanda[2]);
        //Actualizare stiva undo
        char *comanda_undo = (char *) malloc(L_MAX);
        comanda_undo[0] = 'r';
        comanda_undo[1] = 'r';
        comanda_undo[2] = ' ';
        strcpy(comanda_undo + 3, comanda[2]);
        comanda_undo[l2 + 3] = ' ';
        strcpy(comanda_undo + l2 + 4, comanda[1]);
        inserare_stiva(&(editor->undo), editor->redo->inceput_comanda, comanda_undo);
        //Inlocuirea cuvantului
        NodText *aux = editor->redo->inceput_comanda->urm;
        for (int i = 0; i < l1; i++)
            aux = eliminare_caracter(aux, *editor);
        inserare_text(editor->redo->inceput_comanda, comanda[2]);
    }
    else if (!strcmp(comanda[0], "m")){ //Deplasarea cursorului
        //Crearea comenzii undo
        editor->cursor.caracter = editor->redo->inceput_comanda;
        pozitie_cursor(editor);
        char *aux = convertire_numar_sir(editor->cursor.poz_linie);
        char *comanda_undo = (char *) malloc(L_MAX);
        comanda_undo[0] = 'm';
        comanda_undo[1] = ' ';
        int l = strlen(aux);
        strcpy(comanda_undo + 2, aux);
        comanda_undo[l + 2] = ' ';
        free(aux);
        aux = convertire_numar_sir(editor->cursor.poz_caracter);
        strcpy(comanda_undo + l + 3, aux);
        free(aux);
        //Executia comenzii
        int poz_linie = convertire_sir_numar(comanda[1]);
        int poz_caracter = convertire_sir_numar(comanda[2]);
        deplasare_cursor(editor, poz_linie, poz_caracter);
        //Actualizare stiva undo
        inserare_stiva(&(editor->undo), editor->cursor.caracter, comanda_undo);
    }
    else{ //Comanda NULL, fara efect. Ex: inlocuirea unui cuvant inexistent
        char *comanda_undo = (char *) malloc(L_MAX);
        comanda_undo[0] = 'n';
        comanda_undo[1] = '\0';
        inserare_stiva(&(editor->undo), editor->cursor.caracter, comanda_undo);
    }

    for (int i = 0; i < nr_argumente; i++)
        free(comanda[i]);
    free(comanda);
    eliminare_stiva(&(editor->redo));
}
