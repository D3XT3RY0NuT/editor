#ifndef __UNDO__
#define __UNDO__

Stiva *initializare_stiva();
void inserare_stiva(Stiva **, NodText *, char *);
void eliminare_stiva(Stiva **);
void vidare_stiva(Stiva **);
void undo(Editor *);
void redo(Editor *);

#endif // __UNDO__
