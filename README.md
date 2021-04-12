# Editor de text în linie de comandă
***
## Cuprins
1. [Informații generale](#informații-generale)
2. [Conținutul proiectului](#conținutul-proiectului)
3. [Tehnologii folosite](#tehnologii-folosite)
4. [Instalare](#instalare)
5. [Probleme](#probleme)

### Informații generale
***
În cadrul acestui proiect a fost implementat un simplu editor de text în linie de comandă 
ce conține toate funcționalitățile de bază.

### Conținutul proiectului
***
Acest proiect conține 8 fișiere sursă și un Makefile:
1. **main.c**: Fișierul de bază ce lansează funcția principală „executare”. 
Prin intermediul acesteia, programul devine operațional.
2. **interpretor_comenzi.h**: Conține definițiile funcțiilor apelate direct 
de către alte module.
3. **interpretor_comenzi.h**: Conține toate funcțiile cerute în enunțul temei, 
precum și alte funcții ce ajută la interpretarea comenzilor din modul de inserare
sau de comandă.
4. **undo.h**: Include definițiile principalelor funcții ce operează cu stivele
de undo sau redo, cât și funcțiile de undo și redo.
5. **undo.c**: Implementarea tuturor operațiilor necesare pentru crearea, modificarea
și ștergerea stivelor de undo și redo. De asemenea, sunt implementate funcționalitățile
de undo și redo.
6. **utilitare.h**: Definițille funcților auxiliare folosite foarte des în alte module.
7. **utilitare.c**: Conține o multitudine de funcții de mare folos (ștergerea unui caracter,
inserția de text de la o anumită poziție), folosite foarte des în undo.c și 
interpretor_comenzi.c
8. **structuri.h**: Definirea principalelor structuri folosite în program: o listă
dublu înlănțuită pentru memorarea textului, un editor ce conține un cursor, începutul
textului și stivele de undo și redo, o structură pentru cursor și stivele de undo și redo.
9. **Makefile**: conține o regulă de build și una de clean necesare compilării proiectului.

### Tehnologii folosite
***
În realizarea proiectului s-a folosit mediul de dezvoltare integrat open-source CodeBlocks, versiunea 20.03. Drept compilator, gcc version 9.3.0, iar C ca limbaj de programare.

### Instalare
***
După descărcarea arhivei editor.zip, se dezarhivează toate fișierele. Se folosește comanda make pentru compilarea proiectului după care se rulează executabilul creat: editor. Executabilul rulează atât pe Linux, cât și pe Windows.

### Probleme
***
Nu sunt cunoscute probleme la momentul actual. Programul a fost testat doar pe Windows 10, 64 biți și pe distribuția de Linux Ubuntu 20.04. Orice problemă poate fi semnalată printr-o cerere pe GitHub la adresa https://github.com/D3XT3RY0NuT/editor sau la adresa de e-mail ioan.cirstea@stud.acs.upb.ro.
***
Notă: Repertoriul GitHub a devenit public la data de 16 aprilie 2021.
