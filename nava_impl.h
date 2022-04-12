#ifndef __NAVA__
#define __NAVA__

typedef struct NAVA{
	int x[10],y[10]; //coordonate nava
	int orizontal[10]; // directia barcii : 1 orizontal, 0 vertical
	int lungime[10]; // lungime nava
	int stare[10][4]; // 1 bucata din nava functionala
				// 2 bucata din nava distrusa
				// 0 bucata din nava inexistenta(lungimea mai mica
				// decat 4)
	int activ[10]; // nava distrusa complet = 0, altfel 1
} NAVA;

#endif

#ifndef __GEN__
#define __GEN__

#ifndef __SCOR__
#define __SCOR__
typedef struct SCOR{
	int total_nave_distruse;
	float total_eficienta;
	int jocuri_standard_castigate;
	int jocuri_expert_castigate;
} SCOR;

#endif

int deschidere_fisiere(int , FILE **,char *[]);
// functie pentru a verifica daca argumentele sunt date corect
// si pentru a deschide mapele

int **sablon_harta(FILE *);
//functie pentru initializarea unei harti goale

void harta_dupa_randomizare(NAVA *, int **);
// functie utilizata pentru randomize din joc

void generator_harta(NAVA *, int **);
// functie pentru generarea hartii

void scan_harta(int **, NAVA *);
// functie pentru a actualiza datele de tip NAVA 

void initializare_set_nave(NAVA *);
// functie pentru initializarea datelor de tip NAVA

int nave_ramase(NAVA);
//void scrie_text(char *text, WINDOW *display, int y, int x);

int comp_atac(int **, int **, int *, NAVA *);
// functie pentru atac inteligent utilizata pentru computer

int atac_random(int **, int **);
// functie pentru atac la intamplare

int nr_nave_lungime(NAVA , int, int);
// functie pentru a afla cate nave de acea lungime au fost distruse/ sunt active

void calc_lovituri(int **, float []);
// functie pentru a calcula cate lovituri reusite si totale are un player

void free_harta(int ***);
// functie pt a sterge harta

int verifica_harta(char *, char *);
// verifica daca harta este construita corespunzator

char **verifica_fisier(int *);
// functie pt a obtine lista hartilor corecte din Maps/
#endif



