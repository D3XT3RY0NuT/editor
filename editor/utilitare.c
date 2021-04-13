#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structuri.h"
#include "undo.h"

//Creeaza un caracter nelegat
NodText *creare_caracter(){
    NodText *caracter = (NodText *) malloc(sizeof(NodText));
    caracter->c = '\0';
    caracter->prec = NULL;
    caracter->urm = NULL;

    return caracter;
}

//Creeaza un editor vid sub forma de lista dublu inlantuita
Editor *initializare_editor(){
    Editor *editor = (Editor *) malloc(sizeof(Editor));
    editor->inceput = creare_caracter();
    editor->cursor.caracter = editor->inceput;
    editor->cursor.poz_caracter = 0;
    editor->cursor.poz_linie = 1;
    editor->undo = initializare_stiva();
    editor->redo = initializare_stiva();

    return editor;
}

//Elibereaza intreaga memorie continuta de editor
void inchidere_editor(Editor *editor){
    NodText *aux;
    while(editor->inceput){
        aux = editor->inceput;
        editor->inceput = editor->inceput->urm;
        free(aux);
    }
    vidare_stiva(&(editor->undo));
    vidare_stiva(&(editor->redo));
    free(editor);
}

//Determina coordonatele cursorului
void pozitie_cursor(Editor *editor){
    NodText *elem = editor->inceput;
    int poz_caracter = 0;
    int poz_linie = 1;
    while(elem != editor->cursor.caracter){
        if (elem->c == '\n'){
            poz_caracter = 1;
            poz_linie++;
        }
        else
            poz_caracter++;
        elem = elem->urm;
    }
    if (editor->cursor.caracter->c == '\n'){
        poz_caracter = 0;
        poz_linie++;
    }

    editor->cursor.poz_caracter = poz_caracter;
    editor->cursor.poz_linie = poz_linie;
}

//Imprima continutul editorului
void imprimare(FILE *fisier, Editor editor){
    NodText *elem = editor.inceput->urm;
    while(elem){
        if (elem->c != '\n' || elem->prec->c != '\n')
            fputc(elem->c, fisier);
        elem = elem->urm;
    }
}

//Insereaza un caracter si il returneaza
NodText *inserare_caracter(NodText *caracter_curent, char c){
    NodText *aux = creare_caracter();
    aux->c = c;
    aux->prec = caracter_curent;
    aux->urm = caracter_curent->urm;
    if (caracter_curent->urm)
        caracter_curent->urm->prec = aux;
    caracter_curent->urm = aux;

    return aux;
}

//Insereaza text si returneaza ultimul caracter inserat
NodText *inserare_text(NodText *caracter, char *sir){
    int l = strlen(sir);
    for (int i = 0; i < l; i++)
        caracter = inserare_caracter(caracter, sir[i]);

    return caracter;
}

//Elimina un caracter si returneaza caracterul urmator
NodText *eliminare_caracter(NodText *caracter, Editor editor){
    if (caracter != editor.inceput){
        NodText *aux = caracter;
        caracter->prec->urm = caracter->urm;
        if (caracter->urm)
            caracter->urm->prec = caracter->prec;
        caracter = caracter->urm;
        free(aux);
        aux = NULL; //Marcheaza caracterul ca fiind sters

        return caracter;
    }

    return NULL;
}

//Determinarea celui de al n-lea caracter pornind de la nodul de text dat
NodText *cautare_caracter(NodText *caracter, int indice_caracter){
    while(indice_caracter--)
        caracter = caracter->urm;

    return caracter;
}

//Determina randul de indice dat si returneaza primul caracter de pe acesta.
NodText *cautare_linie(Editor editor, int indice_linie){
    NodText *elem = editor.inceput->urm;
    indice_linie--;
    while(indice_linie){
        if (elem->c == '\n' && elem->prec->c != '\n')
            indice_linie--;
        elem = elem->urm;
    }

    return elem;
}

//Videaza sirul dat ca argument
void reinitializare_sir(char *sir){
    for (int i = 0; i < L_MAX; i++)
        sir[i] = '\0';
}

//Pune cursorul imediat dupa caracterul de coordonate date
void deplasare_cursor(Editor *editor, int indice_linie, int indice_caracter){
    NodText *aux = cautare_linie(*editor, indice_linie);
    aux = aux->prec;
    aux = cautare_caracter(aux, indice_caracter);
    editor->cursor.caracter = aux;
    editor->cursor.poz_linie = indice_linie;
    editor->cursor.poz_caracter = indice_caracter;
}

//Calculul lungimii liniei pe care se afla caracterul dat
int lungime_linie(NodText *caracter){
    NodText *aux = caracter;
    aux = aux->prec;
    while(aux->c != '\n')
        aux = aux->prec;
    int l = 0;
    aux = aux->urm;
    while(aux->c != '\n'){
        l++;
        aux = aux->urm;
    }

    return l;
}

//Generarea unui subsir de lungime maxima l pornind de la un anumit caracter
//La capat de linie se opreste
void generare_subsir(char *subsir, NodText *caracter, int l_max){
    NodText *aux = caracter;
    int l_curent = 0;
    while (l_max && aux){
        subsir[l_curent++] = aux->c;
        l_max--;
        aux = aux->urm;
    }

    subsir[l_curent] = '\0';
}

//Functie ce transforma un sir de caractere numerice in numarul corespunzator.
int convertire_sir_numar(char *sir){
    int l = strlen(sir);
    int rez = 0;
    for (int i = 0; i < l; i++){
        if (sir[i] < '0' || sir[i] > '9'){
            printf("Eroare in functie convertire_sir_numar()!\n");
            return 0;
        }
        rez = rez * 10 + sir[i] - '0';
    }

    return rez;
}

//Functie ce transforma un numar in sirul de caractere corespunzator.
char *convertire_numar_sir(int nr){
    char cifre[10];
    int nr_cifre = 0;
    do{
        cifre[nr_cifre++] = '0' + nr % 10;
        nr /= 10;
    }while(nr);

    char *sir = (char *) malloc(nr_cifre + 1);
    for (int i = 0; i < nr_cifre; i++)
        sir[i] = cifre[nr_cifre - i - 1];
    sir[nr_cifre] = '\0';

    return sir;
}
