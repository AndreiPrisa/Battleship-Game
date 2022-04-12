#ifndef __TEXT__
#define __TEXT__
void scrie_text(char *, WINDOW *, int , int );
// functie pentru scrierea textului artistic (ASCII art style)

int h_text(char *);
// functie pentru a afla inaltimea textului artistic

int l_text(char *);
// functie pentru a afla lungimea textului artistic( rezultat scrie_text)

#endif

#ifndef __FONT__
#define __FONT__
typedef struct FONT
{
	char nume[25];
	long int poz[62][2];
	int lungime[62];
} FONT;
#endif

#ifndef __C_TEXT__
#define __C_TEXT__
FONT initializare_font(char *);
// functie pt initializare font

void custom_text (FONT, char *, int , int , int , char *);
// functie pt a scrie textul in fontul ales

#endif

#ifndef __MENIU__
#define __MENIU__

int pop_up(int, int, char *, int , char *[]);
// functie pt fereastra de alegere

int meniu_config(WINDOW *, int , int ***, char *[]);
// functie pentru implementarea meniului de alegere a hartii

int meniu_gm();
// functie pentru implementarea meniului de selectie a gamemode ului

void meniu_joc(WINDOW *, char *[], int );
// functie pentru a afisa meniul jocului

void print_grid(WINDOW *);
// functie pentru afisarea gridului de 10 x 10

void afisare_nave(WINDOW *, int **);
// functie pentru afisarea navelor pe grid

int interfata_atac(WINDOW *, int [], int, int **, int **, int *);
// functie pentru implementarea interfetei de atac a jucatorului

#endif

