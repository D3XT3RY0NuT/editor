#ifndef __STRUCTURI__
#define __STRUCTURI__
#define L_MAX 100 //Lungimea maxima a unei linii, fie de editor sau de comanda

//Structura NodText contine un caracter din textul editorului.
//Fiecare NodText este legat atat de caracterul precedent, cat si de urmatorul
typedef struct NodText{
    char c;
    struct NodText *urm, *prec;
}NodText;

//Cursorul se afla pe un caracter
//Coordonatele sale sunt linia pe care se afla si caracterul corespunzator de pe linie
typedef struct Cursor{
    NodText *caracter;
    int poz_caracter;
    int poz_linie;
}Cursor;

//Structura de stiva necesara implementarii functionalitatilor de undo si redo
typedef struct Stiva{
    NodText *inceput_comanda;
    char *comanda;
    struct Stiva *urm;
}Stiva;

//Editorul de text are un inceput, un cursor si doua stive pentru undo si redo
typedef struct Editor{
    NodText *inceput;
    Cursor cursor;
    Stiva *undo, *redo;
}Editor;

#endif // __STRUCTURI__
