#ifndef __UTILITARE__
#define __UTILITARE__

NodText *creare_caracter();
Editor *initializare_editor();
void inchidere_editor();
void pozitie_cursor(Editor *);
void imprimare(FILE *, Editor);
NodText *inserare_caracter(NodText *, char);
NodText *inserare_text(NodText *, char *);
NodText *eliminare_caracter(NodText *, Editor);
NodText *cautare_caracter(NodText *, int);
NodText *cautare_linie(Editor, int);
void reinitializare_sir(char *);
void deplasare_cursor(Editor *, int, int);
int lungime_linie(NodText *);
char *generare_subsir(char *, NodText *, int);
char *concatenare(char *, char *);
int convertire_sir_numar(char *);
char *convertire_numar_sir(int);

#endif // __UTILITARE__
