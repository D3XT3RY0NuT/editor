#ifndef __INTERPRETOR_COMENZI__
#define __INTERPRETOR_COMENZI__
#include "structuri.h"

char **interpretare_comanda(char *, int *);
void mod_citire(char *, Editor *);
void mod_comanda(char *, Editor *, char*);
void revenire(Editor *);
void stergere(Editor *, char *, int);

#endif // __INTERPRETOR_COMENZI__
