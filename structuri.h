#ifndef __STRUCTURI__
#define __STRUCTURI__
#define L_MAX 100 //Lungimea maxima a unei linii, fie de editor sau de comanda

//Structura NodText contine un caracter dintr-o linie.
typedef struct NodText{
    char c;
    struct NodText *urm, *prec;
}NodText;

//Cursorul editorului se afla pe un caracter
typedef struct Cursor{
    NodText *caracter;
    int poz_caracter;
    int poz_linie;
}Cursor;

typedef struct Stiva{
    NodText *inceput_comanda;
    char *comanda;
    struct Stiva *urm;
}Stiva;

//Editorul este complet definit prin inceput, sfarsit si cursor
typedef struct Editor{
    NodText *inceput;
    Cursor cursor;
    Stiva *undo, *redo;
}Editor;

#endif // __STRUCTURI__
