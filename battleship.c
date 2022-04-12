#include "librarii.h"
#include "nava_impl.h"
#include "grafica.h"
#define L_COLORAT 15 //cat sa coloram in menu
#define OPTIUNI 5 // cate optiuni avem in menu
#define BOUNDS 10 //limita hartii
#define TITLE_L 99 // cat are titlul jocului lungimea
#define FRAMERATE 24
#define PATHTEXT "Texts/"
#define PATHMAP "Maps/"
#define PATHBIN "Bin/scor.bin"

typedef struct HOLD
{
	int lungime;
	int x,y;
	int orizontal;
	int activ;
	int coresp;
} HOLD;

void delay(float secunde)
{
        clock_t delay = secunde * CLOCKS_PER_SEC;
       clock_t start = clock();
        while(clock() - start < delay);
}
// functie pentru delay

void free_harta(int ***harta)
{
	int i;

	for(i = 0; i < 12; i++)
		if((*harta)[i] !=NULL)
			free((*harta)[i]);
	if((*harta) != NULL)
		free(*harta);
}
// functie pentru eliberarea memoriei

int deschidere_fisiere(int n, FILE **harti,char *argv[])
{
	int i,j;

	for(i = 1; i <= n; i++)
	{
		harti[i - 1] = fopen(argv[i], "r");
		if( harti[i - 1] == NULL )
		{
			printf("[Eroare]:Fisierul %s nu poate fi deschis.\n", argv[i]);
			for(j = 1; j < i; j++)
				fclose(harti[j - 1]); //inchidem hartile dechise cu succes
			return 0;
		}
	
	}
	return 1;

}
// functie pentru a verifica daca argumentele sunt date corect
// si pentru a deschide mapele

void meniu_joc(WINDOW *meniu, char *opt[], int poz)
{
	int top; // linia pt New Game
	int i;
	int row, col;
	
	wclear(meniu);
        getmaxyx(meniu,row,col);
	top = row / 2;
	scrie_text("title.txt", meniu, row / 11, (col - TITLE_L) / 2);
	box(meniu, 0, 0);
	for(i = 0; i < OPTIUNI ; i++)
	{
		mvwprintw(meniu, top + i, (col- strlen(opt[i]) ) / 2, opt[i]);
		mvwchgat(meniu, top + i, (col - L_COLORAT) / 2, L_COLORAT, A_NORMAL, 0 , NULL);
	}
	mvwchgat(meniu, top + poz - 1, (col - L_COLORAT) / 2, L_COLORAT, A_REVERSE, 0 , NULL);
	refresh();
}
// functie pentru a afisa meniul jocului

int **sablon_harta(FILE *conf)
{
	int **harta;
	int i,j;
	char aux[23], *p;

	harta = malloc((BOUNDS + 2) * sizeof(int*)); //alocam +2 si pt margini
        for(i = 0; i < BOUNDS + 2; i++)
	{	
		harta[i] = malloc((BOUNDS + 2) * sizeof(int));		
		for(j = 0; j < BOUNDS + 2; j++)
                	harta[i][j] = 0;
	}
        if(conf != NULL)
		for(i = 1; i <= BOUNDS; i++)
		{
			j = 1;
			fgets(aux, 23,conf);
			aux[strlen(aux) - 1] = '\0';
			p = strtok(aux, "|");
			while(p != NULL)
			{
				//printf("%s %d\n", p, strcmp(p, "X"));
				if(strcmp(p, "X") == 0)
					harta[i][j] = 1;
				j++;
				p = strtok(NULL, "|");
			}
		}
	return harta;
} //functie pentru initializarea unei harti goale

void harta_dupa_randomizare(NAVA *flota, int **harta)
{
	int i,k,j,contor;
	int x,y;
	for(i = 0; i < 10; i++)
		if(flota->activ[i])
		{
			contor = 0;
			for(j = 0; j < flota->lungime[i]; j++)
				if(flota->stare[i][j] == 1)
					contor++;
			flota->lungime[i] = contor;
			for(k = 0; k < contor; k++)
				flota->stare[i][k] = 1;
		}

	for(i = 1; i <= 10; i++)
		for(j = 1; j <= 10; j++)
			harta[i][j] = 0;
	for(contor = 0; contor < 10; contor++)
                if(flota->activ[contor] == 0)
                {
                        x = flota->x[contor];
                        y = flota->y[contor];
                        if(flota->orizontal[contor])
                                for(k = 0; k < flota->lungime[contor]; k++)
                                        harta[x][y + k] = 2;
                        else
                                for(k = 0; k < flota->lungime[contor]; k++)
                                        harta[x + k][y] = 2;
                }

}
// functie utilizata pentru randomize din joc

void generator_harta(NAVA *flota, int **harta)
{
	int i;
	int contor;
	int nava_plasata; // verificam daca am plasat corec piesa
	int x,y; //coordonate random pt nava
	int orizontal; // nava va fi plasata orizontal daca este 1, altfel vertical

	srand(time(0));
	for( contor = 0; contor < 10; contor++)
		if(flota->activ[contor])
			{
				nava_plasata = 0;
				while(!nava_plasata)
				{
					nava_plasata = 1;
					if(flota->lungime[contor] > 1)
						orizontal = random() % 2;
					else
						orizontal = 1;
					if(!orizontal)
					{
						x = rand() % (BOUNDS - flota->lungime[contor] + 1) + 1; // ne asiguram ca teoretic nava ar incapea in harta
						y = rand() % BOUNDS + 1;
					}
					else
	  				{
                                        	x = rand() % BOUNDS + 1;
                                        	y = rand() % (BOUNDS - flota->lungime[contor] + 1) + 1;
                                	}
					//printf("coordonate: %d %d\n",x,y);

					if(orizontal)
						for(i = y - 1; i <= y + flota->lungime[contor]; i++)
							if(harta[x][i] || harta[x - 1][i] || harta[x + 1][i])
							{
								nava_plasata = 0;
								break;
							}
					if(!orizontal)
						for(i = x - 1; i <= x + flota->lungime[contor]; i++)
                                                	if(harta[i][y] || harta[i][y - 1] || harta[i][y + 1])
                                                	{
                                                        	nava_plasata = 0;
                                                        	break;
                                                	}


				}
				if(orizontal)
					for(i = y; i < y + flota->lungime[contor];i++)
						harta[x][i] = flota->stare[contor][i - y];
				else
					for(i = x; i < x + flota->lungime[contor]; i++)
						harta[i][y] = flota->stare[contor][i -x];
				flota->x[contor] = x;
				flota->y[contor] = y;
				flota->orizontal[contor] = orizontal;
			}
}
// functie pentru generarea hartii

void scan_harta(int **harta, NAVA *flota)
{
        int i,j,l;
        int nr = 0;
        int lungime;
        int activ;

        for(i = 1 ; i <= 10; i++)
                for(j = 1; j <= 10; j++)
                        if((harta[i][j] == 1 || harta[i][j] == 2) && (harta[i-1][j] == 0 || harta[i - 1][j] == 3))
                        {
                                activ = 0;
                                flota->x[nr] = i;
                                flota->y[nr] = j;
				flota->orizontal[nr] = 1;
                                flota->stare[nr][0] = harta[i][j];
                                if(harta[i][j] == 1)
                                        activ = 1;
                                lungime = 1;
                                if(harta[i+1][j] == 1 || harta[i + 1][j] == 2)
                                {
                                        flota->orizontal[nr] = 0;
                                        l = i+1;
                                        while(harta[l][j] == 1 || harta[l][j] == 2)
                                        {
                                                if(harta[l][j] == 1)
                                                        activ = 1;
                                                flota->stare[nr][lungime++] = harta[l++][j];
                                        }
                                }
                                else
                                        if(harta[i][j+1] == 1 || harta[i][j + 1] == 2)
                                        {
                                                flota->orizontal[nr] = 1;
                                                j++;
                                                while(harta[i][j] == 1 || harta[i][j] == 2)
                                                {
                                                        if(harta[i][j] == 1)
                                                                activ = 1;
                                                        flota->stare[nr][lungime++] = harta[i][j++];
                                                }

                                        }
                        flota->activ[nr] = activ;
                        flota->lungime[nr] = lungime;
			nr++;
                        }
}
// functie pentru a actualiza datele de tip NAVA 


void initializare_set_nave(NAVA *flota)
{
	int i,j,k = 0, l;

	for(i = 4; i >= 1; i--)
		for(j = 1; j <= 5 - i; j++)
		{
			flota->activ[k] = 1;
			flota->lungime[k] = i;
			for(l = 0; l < flota->lungime[k]; l++)
				flota->stare[k][l] = 1;
			k++;
		}
}
// functie pentru initializarea datelor de tip NAVA

void print_grid(WINDOW *grid)
{
	int i;

	wattron(grid, A_UNDERLINE);
	for(i = 1; i <= 21; i += 2)
		mvwvline(grid, 1, i, ACS_VLINE, 10);
	wattroff(grid, A_UNDERLINE);
	wrefresh(grid);

}
// functie pentru afisarea gridului de 10 x 10

void afisare_nave(WINDOW *grid, int **harta)
{
	int i,j;
	for(j = 1; j <= 10; j++)
		for(i = 2; i <= 20; i+= 2)
		{
			if(harta[j][i / 2] == 1)
				mvwaddch(grid, j, i, ' ' | COLOR_PAIR(1));
			else
				if(harta[j][i / 2] == 2)
					mvwaddch(grid, j, i, 'X' | COLOR_PAIR(1));
				else
					if(harta[j][i / 2] == 3)
						mvwaddch(grid, j, i, 'O' | COLOR_PAIR(3));
					else
						mvwaddch(grid, j, i, ' ');
			if(i < 20 && harta[j][i / 2] >= 1 && harta[j][i / 2 + 1] >= 1  && harta[j][i / 2] < 3 && harta[j][i / 2 + 1] < 3)
				mvwaddch(grid, j, i + 1, ' ' | COLOR_PAIR(1));
			else
				if(i < 20 && harta[j][i / 2] && harta[j][i / 2 + 1])
					mvwaddch(grid, j, i + 1, ' ' | COLOR_PAIR(3));
		}
	wrefresh(grid);
}
// functie pentru afisarea navelor pe grid

int interfata_atac(WINDOW *grid, int coord[], int tasta, int **harta_atac, int **harta_calculator, int *nr_nave)
{
	int y,x;
	int i;

	switch(tasta)
	{
	case KEY_UP:
		if(coord[0] > 1)
			coord[0]--;
		break;
	case KEY_DOWN:
		if(coord[0] < 10)
			coord[0]++;
		break;
	case KEY_LEFT:
		if(coord[1] > 2)
			coord[1] -= 2;
		break;
	case KEY_RIGHT:
		if(coord[1] < 20)
			coord[1] += 2;
		break;
	}
	mvwaddch(grid, coord[0], coord[1], ACS_CKBOARD | COLOR_PAIR(2) | A_BLINK);
	if(tasta == '\n')
	{
		if(harta_atac[coord[0]][coord[1] / 2] == 3 || harta_atac[coord[0]][coord[1] / 2] == 2)
		{
			getmaxyx(stdscr, y, x);
			attron(A_BLINK | COLOR_PAIR(1) | A_BOLD);
			mvprintw(y / 2 + 8, (x - 35) / 2, "THIS SPOT HAS BEEN ALREADY ATTACKED");
			refresh();
			for(i = 1; i <= 3; i++)
			{
				beep();
				delay(0.2);
			}
			attroff(A_BLINK | COLOR_PAIR(1) | A_BOLD);
			delay(1.8);
                        mvprintw(y / 2 + 8, (x - 35) / 2, "                                   ");
			refresh();
			return 1;
		}
		if(harta_calculator[coord[0]][coord[1] / 2] == 1)
		{
			beep();
			delay(0.1);
			beep();
			getmaxyx(stdscr, y, x);
			harta_atac[coord[0]][coord[1] / 2] = 2;
			harta_calculator[coord[0]][coord[1] / 2] = 2;
			nr_nave--;
                        mvprintw(y / 2 + 8, (x - 15) / 2, "               ");
			attron(COLOR_PAIR(5) | A_BLINK);
                        mvprintw(y / 2 + 8, (x - 14) / 2, "SUCCESSFUL HIT");
			attroff(COLOR_PAIR(5) | A_BLINK);
			refresh();
			delay(1.5);
                        mvprintw(y / 2 + 8, (x - 15) / 2, "               ");
		}
		else
			if(harta_calculator[coord[0]][coord[1] / 2] == 0)
			{
				beep();
				attron(A_BLINK | COLOR_PAIR(1) | A_BOLD);
				getmaxyx(stdscr, y, x);
				harta_atac[coord[0]][coord[1] / 2] = 3;
                                mvprintw(y / 2 + 8, (x - 15) / 2, "               ");
				mvprintw(y / 2 + 8, (x - 11) / 2, "M I S S E D");
				refresh();
				delay(1.5);
		    		mvprintw(y / 2 + 8, (x - 15) / 2, "               ");
				attroff(A_BLINK | COLOR_PAIR(1) | A_BOLD);
				attron(A_BOLD | COLOR_PAIR(4));
				mvprintw(y / 2 + 8, (x - 15) / 2, "COMPUTER'S TURN");
				attroff(A_BOLD | COLOR_PAIR(4));
				refresh();
				return 0;
			}

	}
	return 1;

}
// functie pentru implementarea interfetei de atac a jucatorului

int comp_atac(int **harta_atac, int **harta_jucator, int *random, NAVA *flota_jucator)
{
	static int x;
	static int y;
	static int lovituri = 0;
	static int orizontal = 2;
	int v[4] ={0, 0, 0, 0}; 
		// v[i] semnifica pozitia in care se 
		//poate ataca: 1 - sus, 2 - dreapta, 3 - jos, 4 - stanga
	int contor;
	int i,j;
	int verificare;
	int tinta;
	int sens[2] ={-1, 1}; 
		// sus-jos si stanga-drepta
	int dim[2] = {0, 0};
	int l_max; 
		// lungimea maxima a navei active

	srand(time(0));
	scan_harta(harta_jucator, flota_jucator);
	if(nave_ramase(*flota_jucator) == 0)
		return 0;
	l_max = 0;
	for(i = 0; i < 10; i++)
		if(flota_jucator->activ[i] && (flota_jucator->lungime[i] > l_max))
			l_max = flota_jucator->lungime[i];
	if(*random)
	{
		while(1)
		{	
			x = rand() % BOUNDS + 1;
			y = rand() % BOUNDS + 1;
			verificare = 1;
			if(harta_atac[x][y] == 3 || harta_atac[x][y] == 2)
				verificare = 0;
			for(i = -1 ; i <= 1 && verificare; i++)
				for(j = -1; j <= 1 && verificare; j++)
					if(harta_atac[x + i][y + j] == 2)
						verificare = 0;
			if(verificare)
				if(harta_jucator[x][y] == 1)
				{
				*random = 0;
				harta_atac[x][y] = 2;
				harta_jucator[x][y] = 2;
				lovituri = 1;
				orizontal = 2;
				return 1;
				}
				else
				{
				harta_jucator[x][y] = 3;
				harta_atac[x][y] = 3;
				return 0;
				}

		}
	}
	else
	{
		if(lovituri == l_max)
		{
			*random = 1;
                        return comp_atac(harta_atac, harta_jucator, random, flota_jucator);

		}
		if(orizontal == 2)
		{
			contor = 0;
			if(x - 1 > 0 && harta_atac[x - 1][y] == 0 && harta_atac[x - 2][y] != 2 && harta_atac[x - 2][y - 1] != 2 && harta_atac[x - 2][y + 1] != 2)
			{
				v[contor] = 1;
				contor++;
			}
			if(y + 1 < 11 && harta_atac[x][y + 1] == 0 && harta_atac[x][y + 2] != 2 && harta_atac[x + 1][y + 2] != 2 && harta_atac[x - 1][y + 2] != 2)
			{
				v[contor] = 2;
				contor++;
			}
			if(x + 1 < 11 && harta_atac[x + 1][y] == 0 && harta_atac[x + 2][y] != 2 && harta_atac[x + 2][y - 1] != 2 && harta_atac[x + 2][y + 1] != 2)
			{
				v[contor] = 3;
				contor++;
			}
			if(y - 1 > 0 && harta_atac[x][y - 1] == 0 && harta_atac[x][y - 2] != 2 && harta_atac[x - 1][y - 2] != 2 && harta_atac[x + 1][y - 2] != 2)
			{
				v[contor] = 4;
				contor++;
			}
			if(contor)
			{
				tinta = rand() % contor;
				if(v[tinta] == 1)
					if(harta_jucator[x - 1][y] == 1)
					{
						harta_jucator[x - 1][y] = 2;
						harta_atac[x - 1][y] = 2;
						orizontal = 0;
						lovituri++;
						return 1;
					}
					else
					{
						harta_jucator[x - 1][y] = 3;
						harta_atac[x - 1][y] = 3;
						return 0;
					}
				if(v[tinta] == 2)
					if(harta_jucator[x][y + 1] == 1)
					{
						harta_jucator[x][y + 1] = 2;
						harta_atac[x][y + 1] = 2;
						orizontal = 1;
						lovituri++;
						return 1;
					}
					else
					{
                                                harta_jucator[x][y + 1] = 3;
                                                harta_atac[x][y + 1] = 3;
						return 0;
					}
                                if(v[tinta] == 3)
                                        if(harta_jucator[x + 1][y] == 1)
                                        {
                                                harta_jucator[x + 1][y] = 2;
                                                harta_atac[x + 1][y] = 2;
                                                orizontal = 0;
						lovituri++;
                                                return 1;
                                        }
                                        else
                                        {
                                                harta_jucator[x + 1][y] = 3;
                                                harta_atac[x + 1][y] = 3;
                                                return 0;
                                        }
                                if(v[tinta] == 4)
                                        if(harta_jucator[x][y - 1] == 1)
                                        {
                                                harta_jucator[x][y -1] = 2;
                                                harta_atac[x][y - 1] = 2;
                                                orizontal = 1;
						lovituri++;
                                                return 1;
                                        }
                                        else
                                        {
                                                harta_jucator[x][y - 1] = 3;
                                                harta_atac[x][y - 1] = 3;
                                                return 0;
                                        }

			}
			else
			{
				*random = 1;
				return comp_atac(harta_atac, harta_jucator, random, flota_jucator);
			}
		}
		else
		{
			if(lovituri == 4)
			{
				*random = 1;
				return comp_atac(harta_atac, harta_jucator, random, flota_jucator);
			}
			contor = 0;
			if(orizontal == 1)
			{
				for(i = 0; i < 2; i++)
				{
					j = 1;
					while(harta_atac[x][y + j * sens[i]] == 2)
						j++;
					if((y + j * sens[i]) > 0 && (y + j * sens[i]) < 11 && harta_atac[x][y + j * sens[i]] == 0 && harta_atac[x][y + (j + 1) * sens[i]] != 2 && harta_atac[x + 1][y + (j + 1) * sens[i]] != 2 && harta_atac[x - 1][y + (j + 1) * sens[i]] != 2)
					{
						dim[contor] = j;
						v[contor] = sens[i];
						contor++;
					}
				}
				if(contor)
				{
					tinta = rand() % contor;
					if(harta_jucator[x][y + dim[tinta] * v[tinta]] == 1)
					{
						harta_jucator[x][y + dim[tinta] * v[tinta]] = 2;
						harta_atac[x][y + dim[tinta] * v[tinta]] = 2;
						lovituri++;
						return 1;
					}
					else
					{
						harta_jucator[x][y + dim[tinta] * v[tinta]] = 3;
                                                harta_atac[x][y + dim[tinta] * v[tinta]] = 3;
                                                return 0;
					}


				}
				else
				{
				*random = 1;
				return comp_atac(harta_atac, harta_jucator, random, flota_jucator);
				}
			}
                        if(orizontal == 0)
                        {
                                for(i = 0; i < 2; i++)
                                {
                                        j = 1;
                                        while(harta_atac[x + j * sens[i]][y] == 2)
                                                j++;
                                        if((x + j * sens[i]) > 0 && (x + j * sens[i]) < 11 && !harta_atac[x + j * sens[i]][y] && harta_atac[x + (j + 1) * sens[i]][y] != 2 && harta_atac[x + (j + 1) * sens[i]][y + 1] != 2 && harta_atac[x + (j + 1) * sens[i]][y - 1] != 2)
					{
						dim[contor] = j;
						v[contor] = sens[i];
						contor++;
					}
                                }
                                if(contor)
                                {
                                        tinta = rand() % contor;
                                        if(harta_jucator[x + dim[tinta] * v[tinta]][y] == 1)
                                        {
                                                harta_jucator[x + dim[tinta] * v[tinta]][y] = 2;
                                                harta_atac[x + dim[tinta] * v[tinta]][y] = 2;
						lovituri++;
                                                return 1;
                                        }       
                                        else
                                        {
                                                harta_jucator[x + dim[tinta] * v[tinta]][y] = 3;
                                                harta_atac[x + dim[tinta] * v[tinta]][y] = 3;
                                                return 0;
                                        }


                                }
                                else
                                {
                                *random = 1;
                                return comp_atac(harta_atac, harta_jucator, random, flota_jucator);
                                }
                        }


		}
	}
	return 0;
}
// functie pentru atac inteligent utilizata pentru computer

int atac_random(int **harta_atac, int **harta_jucator)
{
	int x,y;
	while(1)
	{
	x = rand() % BOUNDS + 1;
	y = rand() % BOUNDS + 1;
	if(harta_atac[x][y] == 0)
		break;
	}
	if(harta_jucator[x][y] == 1)
	{
		harta_atac[x][y] = 2;
		harta_jucator[x][y] = 2;
		return 1;
	}
	else
	{
		harta_atac[x][y] = 3;
		harta_jucator[x][y] = 3;
		return 0;
	}

}
// functie pentru atac la intamplare

int nave_ramase(NAVA flota)
{
	int i, contor = 0;

	for(i = 0; i < 10; i++)
		if(flota.activ[i])
			contor++;
	return contor;
}
// functie pentru calcularea numarului de nave ramase activ

void scrie_text(char *text, WINDOW *display, int y, int x)
{
	FILE *ftext;
	char linie_de_afisat[200];
	char aux[200];
	
	strcpy(aux, PATHTEXT);
	strcat(aux, text);
	ftext = fopen(aux, "r");
	while(fgets(linie_de_afisat, 200, ftext) != NULL)
	{
		mvwprintw(display, y, x, "%s", linie_de_afisat);
		y++;
	}
	refresh();
	fclose(ftext);
}
// functie pentru scrierea textului artistic (ASCII art style)

int meniu_config(WINDOW *grid, int nr, int ***harta, char *nume_harti[])
{
	int pozitie = 0;
	int tasta;
	int lungime_nume;

	tasta = 'a';
	while(tasta != '\n')
	{
		wclear(grid);
		wclear(stdscr);
		if(tasta == KEY_LEFT && pozitie > 0)
			pozitie--;
		if(tasta == KEY_RIGHT && pozitie < nr)
			pozitie++;
		if(tasta == 'q')
		{
			pozitie = -1;
			break;
		}
		attroff(A_BOLD);
		mvprintw(LINES / 2 + 8, (COLS - 27) / 2, "PRESS <Q> FOR PREVIOUS MENU");
		mvprintw(LINES / 2 + 9, (COLS - 23) / 2, "PRESS <ENTER> TO SELECT");
		mvprintw(LINES / 2 + 10, (COLS - 25) / 2, "USE <- AND -> TO NAVIGATE");
		attron(A_BOLD);
        	scrie_text("ch_map.txt", stdscr, 5, (COLS - 61) / 2);
		if(pozitie > 0)
		{
			attron(A_BLINK);
			scrie_text("sag_st.txt", stdscr, (LINES - 5) / 2, (COLS - 41) / 2 - 12);
			attroff(A_BLINK);
		}
		else
		{
		        scrie_text("random.txt", stdscr, (LINES - 9) / 2, (COLS - 41) / 2);
		}

		if(pozitie < nr)
		{
			attron(A_BLINK);
			scrie_text("sag_dr.txt", stdscr, (LINES -5) / 2, (COLS - 41) / 2 + 48);
			attroff(A_BLINK);
		}
		box(stdscr, 0, 0);
		refresh();
		if(pozitie > 0)
		{
			lungime_nume = strlen(nume_harti[pozitie - 1]);
			mvprintw((LINES - 12) / 2 - 1, (COLS - 23) / 2 + (23 - lungime_nume) / 2, nume_harti[pozitie - 1]);
			refresh();
			mvwin(grid, (LINES - 12) / 2, (COLS - 23) / 2);
			print_grid(grid);
			afisare_nave(grid, harta[pozitie - 1]);
		}
		if(pozitie == 0)
                {
                        tasta = getch();
                }
                else
                {
                        wrefresh(grid);
                        tasta = wgetch(grid);
                }

	}
	wclear(grid);
	clear();
	wrefresh(grid);
	refresh();
	return pozitie;


}
// functie pentru implementarea meniului de alegere a hartii

int l_text(char *text)
{
	FILE *ftext;
	char buf[200];
	int l_max = 0;
	char aux[200];
	strcpy(aux, PATHTEXT);
	strcat(aux, text);
	ftext = fopen(aux, "r");
	while(fgets(buf, 200, ftext) != NULL)
		if(l_max < strlen(buf))
			l_max = strlen(buf);
	fclose(ftext);
	return l_max;
}
// functie pentru a afla lungimea textului artistic( rezultat scrie_text)

int h_text(char *text)
{
	FILE *ftext;
	char buf[200];
	char aux[200];
	strcpy(aux, PATHTEXT);
	strcat(aux, text);
	int contor = 0;
	ftext = fopen(aux, "r");
	while(fgets(buf, 200, ftext) != NULL)
		contor++;
	fclose(ftext);
	return contor;
}
// functie pentru a afla inaltimea textului artistic
int meniu_gm()
{
	int tasta = 'a';
	int pozitie = 0;
	int lungime = l_text("standard.txt");

	while(tasta != '\n')
	{
		scrie_text("gamemode.txt", stdscr, 5, (COLS - l_text("gamemode.txt")) / 2);
		if(pozitie == 0)
		{
			scrie_text("standard.txt", stdscr, (LINES - h_text("standard.txt")) / 2, (COLS - lungime) / 2);
			mvprintw((LINES + h_text("standard.txt")) / 2 + 2, (COLS - 48) / 2, "Destroy in advance and randomize map are enabled");
			mvprintw((LINES + h_text("standard.txt")) / 2 + 3, (COLS - 30) / 2, "and computer plays only random");
			attron(A_BLINK);
			scrie_text("sag_dr.txt", stdscr, (LINES - 5) / 2, (COLS + lungime) / 2 + 4);
			attroff(A_BLINK);
		}
		else
		{
			attron(A_BLINK);
			scrie_text("sag_st.txt", stdscr, (LINES - 5) / 2, (COLS - lungime) / 2 - 4);
			attroff(A_BLINK);
			scrie_text("expert.txt", stdscr, (LINES - h_text("expert.txt")) / 2, (COLS - l_text("expert.txt")) / 2);
			mvprintw((LINES + h_text("expert.txt")) / 2 + 2,(COLS - 49) / 2, "Destroy in advance and randomize map are disabled");
			mvprintw((LINES + h_text("expert.txt")) / 2 + 3,(COLS - 24) / 2,"and computer plays smart");
		}
		attroff(A_BOLD);
                mvprintw((LINES * 3) / 4 , (COLS - 27) / 2, "PRESS <Q> FOR PREVIOUS MENU");
                mvprintw((LINES * 3) / 4 + 1, (COLS - 23) / 2, "PRESS <ENTER> TO SELECT");
                mvprintw((LINES * 3) / 4 + 2, (COLS - 25) / 2, "USE <- AND -> TO NAVIGATE");
		attron(A_BOLD);
		box(stdscr, 0, 0);
		refresh();
		tasta = getch();
		if(tasta == KEY_LEFT && pozitie)
			pozitie--;
		if(tasta == KEY_RIGHT && pozitie == 0)
			pozitie++;
		if(tasta == 'q')
		{
			clear();
			return -1;
		}
		clear();
	}
	refresh();
	return pozitie;

}
// functie pentru implementarea meniului de selectie a gamemode ului

int nr_nave_lungime(NAVA flota, int l, int dis)
{
	int i;
	int contor = 0;
	for(i = 0; i < 10; i++)
	{
		if(flota.activ[i] == dis && flota.lungime[i] == l)
			{
				contor++;
			}
	}
	return contor;
}
// functie pentru a afla cate nave de acea lungime au fost distruse

void calc_lovituri(int **harta_atac, float lovituri[])
{
	int i,j;

	lovituri[0] = 0;
	lovituri[1] = 0;
	for(i = 1; i <= 10; i++)
		for(j = 1; j <= 10; j++)
			if(harta_atac[i][j])
			{
				lovituri[1] += 1;
				if(harta_atac[i][j] == 2)
					lovituri[0] += 1;
			}
}
void border_win(WINDOW *win)
{
	int rows, cols;
	getmaxyx(win, rows, cols);
	mvwvline(win, 1, 1, ACS_VLINE, cols);
        mvwvline(win, rows - 1, 1, ACS_VLINE, cols);
        mvwhline(win, 1, 1, ACS_HLINE, rows);
        mvwhline(win, 1, cols - 1, ACS_HLINE, rows);
	
        mvwaddch(win, 1, 1, ACS_ULCORNER);
        mvwaddch(win, rows - 1, 1, ACS_LLCORNER);
        mvwaddch(win, 1, cols - 1, ACS_URCORNER);
        mvwaddch(win, rows - 1, cols - 1, ACS_LRCORNER);

	
}
// functie pentru a calcula cate lovituri reusite si totale are un player

char *pop_up_write(int rows, int cols, char *mesaj, int n, char *optiuni[], char *path)
{
	WINDOW *pop = newwin(rows, cols, (LINES - rows) / 2, (COLS - cols) / 2);
	int pozitie = 0, tasta = 'a';
	int y, x;
	int l_sp;
	int i, contor, lungime = strlen(mesaj);
	//char *optiuni[] = {
	//			"Cancel",
	//			"Accept"
	//		};
	char aux[200][200] = {0};
	char input[200];
	char filename[200];
	//border_win(pop);
	keypad(pop, FALSE);
	contor = 0;
	while(lungime > (contor + 1) * (cols - 2))
	{
		strncat(aux[contor], mesaj + contor * (cols - 3), cols - 3);
		l_sp = 0;
		mvwprintw(stdscr, 1 , 1, "%c %d", aux[contor][strlen(aux[contor]) - 1], strlen(aux[contor]));
		while(aux[contor][strlen(aux[contor]) - l_sp - 1] != ' ')
			l_sp++;
		mvprintw(2 , 1 ,"%d", l_sp);
		if(l_sp)
		{
			strcat(aux[contor + 1], aux[contor] + strlen(aux[contor]) - l_sp);
			aux[contor][strlen(aux[contor]) - l_sp] = '\0';
		}
			//aux[(contor + 1) * (rows - 2)] = '\n';
		contor++;
	}
	strcat(aux[contor], mesaj + contor * (cols - 3));
	contor++;
	refresh();
	getmaxyx(pop, y, x);
        for(i = 0; i < contor; i++)
                mvwprintw(pop, (y - 1 - (contor + 1)) / 2 + i, 1, aux[i]);
	box(pop, 0, 0);
	mvwprintw(pop, y - 2, 1, "FILENAME:");
	wrefresh(pop);
		keypad(pop, FALSE);
		echo();
		curs_set(1);
		mvwgetnstr(pop, y - 2, 11, input, cols - 13);
		keypad(pop, TRUE);
		i = 0;
		while(input[i] != '\n')
			i++;
		input[i] = '\0';
		//mvprintw(6, 1, input);
		curs_set(0);
		strcpy(filename, path);
		strcat(filename, input);
		noecho();
		if( access(filename, F_OK) != -1)
		{
			//wclear(pop);
			//wrefresh(pop);
			if(pop_up(rows, cols + 1, "A map with the name you typed already exists. Would you like to overwrite it?", n, optiuni))
			{
				wclear(pop);
				wrefresh(pop);
				return strdup(filename);			
			}
			else
				return NULL;
		}
		else
			{
				wclear(pop);
				wrefresh(pop);
				return strdup(filename);
			}

	wclear(pop);
	clear();
	mvwin(pop, (LINES - rows) / 2, (COLS - cols) / 2);
	wresize(pop, rows, cols);
	//border_win(pop);
	
}

int pop_up(int rows, int cols, char *mesaj, int n, char *optiuni[])
{
	WINDOW *pop = newwin(rows, cols, (LINES - rows) / 2, (COLS - cols) / 2);
	int pozitie = 0, tasta = 'a';
	int y, x;
	int l_sp;
	int i, contor, lungime = strlen(mesaj);
	//char *optiuni[] = {
	//			"Cancel",
	//			"Accept"
	//		};
	char aux[200][200] = {0};
	char input[200];
	//border_win(pop);
	keypad(pop, TRUE);
	contor = 0;
	while(lungime > (contor + 1) * (cols - 2))
	{
		strncat(aux[contor], mesaj + contor * (cols - 3), cols - 3);
		l_sp = 0;
		mvwprintw(stdscr, 1 , 1, "%c %d", aux[contor][strlen(aux[contor]) - 1], strlen(aux[contor]));
		while(aux[contor][strlen(aux[contor]) - l_sp - 1] != ' ')
			l_sp++;
		mvprintw(2 , 1 ,"%d", l_sp);
		if(l_sp)
		{
			strcat(aux[contor + 1], aux[contor] + strlen(aux[contor]) - l_sp);
			aux[contor][strlen(aux[contor]) - l_sp] = '\0';
		}
			//aux[(contor + 1) * (rows - 2)] = '\n';
		contor++;
	}
	strcat(aux[contor], mesaj + contor * (cols - 3));
	contor++;
	while(1)
	{
		if(tasta == KEY_LEFT && pozitie > 0)
			pozitie--;
		if(tasta == KEY_RIGHT && pozitie < n - 1)
			pozitie++;
		if(tasta == '\n')
			return pozitie;
	box(pop, 0, 0);
	getmaxyx(pop, y, x);
		for(i = 0; i < n; i++)
		{
	        	mvwprintw(pop, y - 2, ((2 * i + 1) * x - n * strlen(optiuni[i]) ) / (2 * n), optiuni[i]);
			if(i)
				mvwaddch(pop, y - 2, (i * x) / n, '|');
		}        
		mvwchgat(pop, y - 2, i * (x / 2) + (i + 1) % n, x - 2, A_NORMAL, 0 , NULL);
        	mvwchgat(pop, y - 2, (pozitie * x) / n + 1, x / n - 1, A_REVERSE, 0 , NULL);
        for(i = 0; i < contor; i++)
		mvwprintw(pop, (y - 1 - (contor + 1)) / 2 + i, 1, aux[i]);
	refresh();
	wrefresh(pop);
	tasta = wgetch(pop);
	wclear(pop);
	clear();
	refresh();
	mvwin(pop, (LINES - rows) / 2, (COLS - cols) / 2);
	wresize(pop, rows, cols);
	//border_win(pop);

	}

}
void Editor_harta(WINDOW *grid, int ***harti, char **nume_harti, int n)
{
	int tasta = '1';
	int **harta_new;
	int pozitie = 0;
	int lungime_nume;
	HOLD nava; // specifica datele navei tinute in mana de player
	int i,j;
	int y = 1, x = 1; // coordonatele cursorului
	int orizontal = 1; // directia navei proiectate curent
	int lungime = 1; // lungimea navei proiectate curent
	char *opt1[] = {
			"Cancel",
			"Yes"
			};
	char *opt2[] = {
			"Retype Name",
			"Overwrite file"
			};
	char *ok_opt[] = {
			"OK"	
			};
	char *nume_fisier = NULL;
	char cale_completa[200];
	int conf[4] = {4, 3, 2, 1}; // conf[i] rep cate nave de lungime i + 1 sunt
	FONT standard = initializare_font("standard.font");
	char aux[200] = {0};
	FILE *map;
	int t;
	while(1)
	{
		wclear(grid);
		clear();
		refresh();
		wrefresh(grid);
		if(tasta == 'q')
			break;
		if(tasta == '\n')
		{
			if(pozitie)
			{
				for(i = 0; i < 4; i++)
					conf[i] = 0;
				harta_new = malloc(12 * sizeof(int*));
				for(i = 0; i < 12; i++)
				{
					harta_new[i] = malloc(12 * sizeof(int));
					for(j = 0; j < 12; j++)
						harta_new[i][j] = harti[pozitie - 1][i][j];
				}
			}
				else
			{
				for(i = 0; i < 4; i++)
					conf[i] = 4 - i;
				harta_new = sablon_harta(NULL);
			}
			tasta = '1';
			nava.activ = 0;
			while(1)
			{
		if(tasta == KEY_RESIZE || tasta == 's')
		{
				wclear(grid);
				clear();
				wrefresh(grid);
				refresh();
                                mvwin(grid, (LINES - 12) / 2, (COLS - 23) / 2);
				if(tasta == 's')
				{
					t = 1;
					for(i = 0; i < 4; i++)
						if(conf[i])
						{
							pop_up(10, 30, "The map is not configured correctly, not all ships have been placed!", 1, ok_opt);
							t = 0;
							break;
						}
					while(t)
					{
						clear();
						wclear(grid);
						wrefresh(grid);
						refresh();
						if(pop_up(6, 25, "Proceed to saving your map?", 2, opt1))
						{
							while((nume_fisier = pop_up_write(10, 48, "Enter the name of the map. It will be saved under Maps/ directory.", 2, opt2, PATHMAP)) == NULL);	
							map = fopen(nume_fisier, "w");
							for(i = 1; i <= 10; i++)
							{
								for(j = 1; j <= 10; j++)
								{
									fprintf(map, "|");
									if(harta_new[i][j])
										fprintf(map, "X");
									else
										fprintf(map, " ");
								}
								fprintf(map, "|\n");
							}
							fclose(map);
							break;	
		print_grid(grid);
						}
						else
							break;
					
					}
				}
		}	
		if(tasta == 'd')
			lungime = lungime % 4 + 1;
		if(tasta == 'a')
			lungime =  lungime - 1 + 4 * ((5 - lungime) / 4);
		if(tasta == 'r')
			orizontal = (orizontal + 1) % 2;
                if(tasta == 'q')
		{
			wclear(grid);
			clear();
			refresh();
			wrefresh(grid);
			if(pop_up(6, 43, "Are you sure you want to exit? Any unsaved map will be lost!", 2, opt1))
                        	break;
		}
		if(tasta == KEY_LEFT && x > 1)
			x--;
		if(tasta == KEY_RIGHT && x < 10)
			x++;
		if(tasta == KEY_UP && y > 1)
			y--;
		if(tasta == KEY_DOWN && y < 10)
			y++;
		if(tasta == '\n')
		{
			if(harta_new[y][x] && nava.activ == 0)
			{
				nava.activ = 1;
				nava.coresp = 0;
				nava.lungime = 1;
				harta_new[y][x] = 0;
				nava.x = x;
				nava.y = y;
				nava.orizontal = 0;
				if(harta_new[y - 1][x] || harta_new[y + 1][x])
					{
						orizontal = 0;
						nava.orizontal = 0;
						i = 1;
						while(harta_new[y - i][x])
						{
							harta_new[y - (i++)][x] = 0;
							nava.lungime++;
						}
						mvprintw(2, 0, "%d %d", i, harta_new[y - 1][x]);
						nava.x = x;
						if(i > 1)
							nava.y = y - i + 1;
						i = 1;
						while(harta_new[y + i][x])
						{
							harta_new[y + (i++)][x] = 0;
							nava.lungime++;
						}
							
					}
				else
					if(harta_new[y][x - 1] || harta_new[y][x + 1])
					{
						orizontal = 1;
						nava.orizontal = 1;
						i = 1;
						while(harta_new[y][x - i])
						{
							harta_new[y][x - (i++)] = 0;
							nava.lungime++;
						}
						nava.y = y;
						if(i > 1)
							nava.x = x - i + 1;
						i = 1;
						while(harta_new[y][x + i])
						{
							harta_new[y][x + (i++)] = 0;
							nava.lungime++;
						}
					}
				lungime = nava.lungime;
			}
			else
				if(nava.activ && nava.coresp)
				{
					while(nava.lungime)
					{
						harta_new[y + (nava.lungime - 1) * ((orizontal + 1) % 2)][x + (nava.lungime - 1) * orizontal] = 1;
						nava.lungime--;
					}
					nava.activ = 0;
					nava.coresp = 0;
				}	
		}
		if(tasta == 27 && nava.activ && nava.x)
                                {
                                        while(nava.lungime)
                                        {
                                                harta_new[nava.y + (nava.lungime - 1) * ((nava.orizontal + 1) % 2)][nava.x + (nava.lungime - 1) * nava.orizontal] = 1;
                                                nava.lungime--;
                                        }
                                        nava.activ = 0;
                                        nava.coresp = 0;
                                }
		if(tasta == ' ')
			if(nava.activ)
			{
				conf[nava.lungime - 1]++;
				nava.activ = 0;
				nava.coresp = 0;
			}
			else
				if(conf[lungime - 1])
				{
					nava.activ = 1;
					nava.x = 0;
					nava.y = 0;
					nava.orizontal = orizontal;
					nava.lungime = lungime;
					conf[lungime - 1]--;
				}

		if(nava.activ)
		{
			if(orizontal && ((x + nava.lungime - 1) > 10))
				x = 11 - nava.lungime;
			if(!orizontal && ((y + nava.lungime - 1) > 10))
				y = 11 - nava.lungime;
			nava.coresp = 1;
			for(i = -1; i <= nava.lungime && nava.coresp; i++)
				if(orizontal)
				{
					if(harta_new[y - 1][x + i] || harta_new[y][x + i] || harta_new[y + 1][x + i])
						nava.coresp = 0;
				}
				else
					if(harta_new[y + i][x - 1] || harta_new[y + i][x] || harta_new[y + i][x + 1])
						nava.coresp = 0;
				
		}

		print_grid(grid);
		afisare_nave(grid, harta_new);
		if(nava.activ == 0)
			mvwaddch(grid, y, 2 * x, ACS_CKBOARD | COLOR_PAIR(2) | A_BLINK);
		else
		{
			if(nava.coresp)
				for(i = 0; i < nava.lungime; i++)
				{
					mvwaddch(grid, y + ((orizontal + 1) % 2) * i, x * 2 + orizontal * i * 2, ' ' | COLOR_PAIR(6));
					if(orizontal && i)
						mvwaddch(grid, y + ((orizontal + 1) % 2) * i, x * 2 + orizontal * i * 2 - 1, ' ' | COLOR_PAIR(6));
				}
			else
                                for(i = 0; i < nava.lungime; i++)
				{
					mvwaddch(grid, y + ((orizontal + 1) % 2) * i, x * 2 + orizontal * i * 2, ' ' | COLOR_PAIR(1) | A_REVERSE);
                                        if(orizontal && i)
                                                mvwaddch(grid, y + ((orizontal + 1) % 2) * i, x * 2 + orizontal * i * 2 - 1, ' ' | COLOR_PAIR(1) | A_REVERSE);

		
				}
		}
		attron(A_BOLD | COLOR_PAIR(4));
		mvprintw((LINES - 12) / 2 - 1, (COLS - 24) / 2,  "                          ");
                mvprintw((LINES - 12) / 2 - 4, (COLS - 21) / 2,  "SHIP            NO. OF");
                mvprintw((LINES - 12) / 2 - 3, (COLS - 21) / 2,  "LENGTH           SHIPS");
		attroff(A_BOLD | COLOR_PAIR(4));
		for(i = 0; i < lungime; i++)
			mvaddch((LINES - 12) / 2 - 1, (COLS - 21) / 2 + i, 'X' | COLOR_PAIR(4) | A_REVERSE);
		attron(A_BOLD);
		mvprintw((LINES - 12) / 2 - 1, (COLS - 21) / 2 + 20, "%d", conf[lungime - 1]);
		attroff(A_BOLD);
                
		attron(COLOR_PAIR(4));
                mvprintw(LINES / 2 + 7, (COLS - 27) / 2, "PRESS <Q> FOR PREVIOUS MENU");
                mvprintw(LINES / 2 + 8, (COLS - 34) / 2, "USE ARROW KEYS TO NAVIGATE THE MAP");
                mvprintw(LINES / 2 + 9, (COLS - 24) / 2, "USE <A> AND <D> TO CHANGE");
                mvprintw(LINES / 2 + 10, (COLS - 22) / 2, "LENGTH OF STOCKED SHIP");
                mvprintw(LINES / 2 + 11, (COLS - 36) / 2, "USE <SPACE> TO USE/SAVE STOCKED SHIP");
                mvprintw(LINES / 2 + 12, (COLS - 32) / 2, "USE <ENTER> TO PLACE/SELECT SHIP");
		mvprintw(LINES / 2 + 13, (COLS - 36) / 2, "PRESS <ESC> TO CANCEL SHIP PLACEMENT");
		mvprintw(LINES / 2 + 14, (COLS - 21) / 2, "PRESS <S> TO SAVE MAP");

		attroff(COLOR_PAIR(4));
	
			refresh();
			//nodelay(grid, 1);
			//while(wgetch(grid) != ERR);
			//nodelay(grid, 0);
			//usleep(1000000 / FRAMERATE);
			tasta = wgetch(grid);
			
			}
		}
		if(tasta == KEY_RIGHT)
			pozitie = (pozitie + 1) % (n + 1);
                if(tasta == KEY_LEFT  && n)
                        pozitie =  pozitie - 1 + (n + 1) * ((n - pozitie) / n);
                
                        attron(COLOR_PAIR(4) | A_BOLD | A_BLINK);
                        scrie_text("sag_st.txt", stdscr, (LINES - 5) / 2, (COLS - 41) / 2 - 12);
                        scrie_text("sag_dr.txt", stdscr, (LINES -5) / 2, (COLS - 41) / 2 + 48);
                        attroff(COLOR_PAIR(4) | A_BOLD | A_BLINK);
                
			attron(COLOR_PAIR(4) | A_BOLD);
			box(stdscr, 0, 0);
			if(pozitie)
			{	
				lungime_nume = strlen(nume_harti[pozitie - 1]);
                        	mvprintw((LINES - 12) / 2 - 1, (COLS - 23) / 2 + (23 - lungime_nume) / 2, nume_harti[pozitie - 1]);
			}
				refresh();
			attroff(COLOR_PAIR(4) | A_BOLD);

		if(pozitie)
		{
			print_grid(grid);
			afisare_nave(grid, harti[pozitie - 1]);
		}
		else
		{
			attron(COLOR_PAIR(4) | A_BOLD | A_BLINK);
			scrie_text("sag_st.txt", stdscr, (LINES - 5) / 2, (COLS - 41) / 2 - 12);
			attroff(A_BLINK);
			custom_text(standard, "NEW", 1, (LINES + 5)/ 2, (COLS - 26) / 2, "Fonts/standard.font");
			attron(A_BLINK);
                        scrie_text("sag_dr.txt", stdscr, (LINES -5) / 2, (COLS - 41) / 2 + 48);
                        attroff(A_BLINK);
			box(stdscr, 0, 0);
			attroff(COLOR_PAIR(4) | A_BOLD);
		}
		attron(COLOR_PAIR(4));
                mvprintw(LINES / 2 + 8, (COLS - 27) / 2, "PRESS <Q> FOR PREVIOUS MENU");
                mvprintw(LINES / 2 + 9, (COLS - 23) / 2, "PRESS <ENTER> TO SELECT");
                mvprintw(LINES / 2 + 10, (COLS - 25) / 2, "USE <- AND -> TO NAVIGATE");
		attroff(COLOR_PAIR(4));
		refresh();
		if(pozitie)
			tasta = wgetch(grid);
		else
			tasta = getch();
	}
}

void deschidere_harti(char ***lista_harti, int ****harti_jucator, int *nr_harti_valide, int *capacitate)
{
	char filename[200];
	int i;
	FILE *harta;
	int auxiliar = 0;
	if(*nr_harti_valide)
		auxiliar = (*nr_harti_valide);
	(*lista_harti) = verifica_fisier(nr_harti_valide);
	printf("%d ", *nr_harti_valide);
	//scanf("%d ", &i);
	if(*nr_harti_valide)
	{
		//scanf("%d ", &i);
		if((*harti_jucator) == NULL)
		{
			//scanf("%d ", &i);
			(*harti_jucator) = malloc((*capacitate) * sizeof(int**));
			//scanf("%d ", &i);
		}
		else
		{
			if((*nr_harti_valide) > (*capacitate))
				{
					(*capacitate) += (*nr_harti_valide); 
					(*harti_jucator) = realloc((*harti_jucator), (*capacitate) * sizeof(int**));
				}
					for(i = 0; i < auxiliar; i++)
						free_harta(&((*harti_jucator)[i]));
		}
		for(i = 0; i < (*nr_harti_valide); i++)
		{
			strcpy(filename, PATHMAP);
			strcat(filename, (*lista_harti)[i]);
			harta = fopen(filename, "r");
			(*harti_jucator)[i] = sablon_harta(harta);
			fclose(harta);
		}
	}
}

void clear_list(char ***lista, int n)
{
	int i;
	for(i = 0; i < n; i++)
		free((*lista)[i]);
	free(*lista);
}

void scriere_scor(SCOR *v)
{
	FILE *f;
	SCOR aux[2];
	int i;

	
	if(access(PATHBIN, F_OK) != -1)
	{
		f = fopen(PATHBIN, "rb+");
		fread(aux, sizeof(SCOR), 2, f);
		for(i = 0; i < 2; i++)
		{
			aux[i].total_nave_distruse += v[i].total_nave_distruse;
			aux[i].jocuri_standard_castigate += v[i].jocuri_standard_castigate;
			aux[i].jocuri_expert_castigate += v[i].jocuri_expert_castigate;
			aux[i].total_eficienta = (aux[i].total_eficienta + v[i].total_eficienta) / 2;
		}
	}
	else
	{
		f = fopen(PATHBIN, "wb");
		for(i = 0; i < 2; i++)
			aux[i] = v[i];
	}
		fseek(f, 0, SEEK_SET);
		fwrite(aux, sizeof(SCOR), 2, f);
		fclose(f);
}
//functie pt actualizarea scorului

int main()
{
	FILE *harta, *f;
	int row, col; 
		// lungimea si latimea ferestrei cu care lucram
	int quit = 0; 
		// variabila pt a iesi din joc
	int pozitie = 1; 
		// ce optiune e selectata
	int y, x; 
		// pozitia cursorului
	int tasta;
	int i,j;
	int auxiliar, capacitate = 10;
	int nr_nave_jucator, nr_nave_calculator; 
					// nr navelor active
	char *optiuni_meniu[] = {
				"New Game",
				"Resume Game",
				"Map Editor",
				"General Status",
				"Quit"
				};
	char *ok_opt[] = {"OK"};
	int contor;
	int random;
	int indice;
	int nr_harti_valide = 0; 
	// variabila care stocheaza cate harti din Maps/ sunt corect construite
	char **lista_harti; 
	// numele hartilor valide din Maps/
	char filename[200];
	int **harta_aleasa, **harta_calculator, ***harti_jucator = NULL, **harta_atac, **harta_atac_comp;
	int resume = 0; 
		// folosit pt a testa daca exista sau nu un joc deja inceput
	int coord[] ={1 , 2}; 
		// set coordonate y si x pt cursor
	float lovituri_jucator[] = {0, 0}; 
		// pozitia 0 semnifica lovituri reusite, 1 lovituri totale
	float lovituri_calculator[] = {0, 0}; // la fel
	float aux[] = {0 ,0};
	int alegere; 
		// indicele hartii alese de jucator su random
	int gamemode; 
		// 0 standard, 1 expert
	int animatie_terminata; 
		// pentru animatia finala cu scorul
	char winner[20]; 
		// cine a castigat
	SCOR v[2];
	// v[0] scorul Playerului, v[1] scorul Computerului

	FONT doom = initializare_font("JS.font"); // font pt scor
	NAVA flota_jucator, flota_calculator;
	WINDOW *meniu;
	WINDOW *grid_jucator, *grid_calculator; 
		// plansa celor doi jucatori
	WINDOW *grid_preview; 
		// grid pentru meniul config ( jucatorul vede ce harta poate alege)

	/*if(argc == 1)
	{	
		printf("[Eroare]:Nu s-au dat argumente de comanda.\n");
		return 1;
	}
	harti = malloc((argc - 1) * sizeof(FILE*));
	if(!(deschidere_fisiere(argc - 1, harti, argv)))
	{
		free(harti);
		return 1;
	}*/

	//harti_jucator = malloc((argc - 1) * sizeof(int**));
	//for(i = 0; i < argc - 1; i++)
	//{
	//	harti_jucator[i] = sablon_harta(harti[i]);
	//}
	deschidere_harti( &lista_harti, &harti_jucator, &nr_harti_valide, &capacitate);
	harta_calculator = sablon_harta(NULL);
	harta_atac = sablon_harta(NULL);
	harta_atac_comp = sablon_harta(NULL);
	srand(time(0));
	initscr();
	start_color();
	init_pair(1, COLOR_RED, COLOR_WHITE); // bucati de nava lovite
	init_pair(2, COLOR_BLACK, COLOR_YELLOW); // selectie jucator
	init_pair(3, COLOR_WHITE, COLOR_CYAN); // locuri atacate fara nava
	init_pair(4, COLOR_YELLOW, COLOR_BLACK); // pt scris
	init_pair(5, COLOR_BLUE, COLOR_WHITE); // lovitura reusita
	init_pair(6, COLOR_WHITE, COLOR_GREEN); // nava poate fi plasata
	//echo();
	cbreak();
	getmaxyx(stdscr, row, col);
	meniu = newwin(row, col, 0, 0);
	grid_jucator = newwin(12, 23, (row - 12) / 2, col / 2 - 25);
	grid_calculator = newwin(12, 23, (row - 12) / 2, col / 2 + 2);
	grid_preview = newwin(12, 23, (row - 12) / 2, (col - 23) / 2);
	//wtimeout(meniu, FRAMERATE);
	//wtimeout(grid_calculator, FRAMERATE);
	//nodelay(meniu, 1);
	keypad(meniu, TRUE);
	keypad(grid_calculator, TRUE);
	keypad(grid_preview, TRUE);
	keypad(stdscr, TRUE);
	meniu_joc(meniu, optiuni_meniu, pozitie);
	curs_set(0);
	harta_aleasa = sablon_harta(NULL);
	nr_nave_jucator = 10;
        nr_nave_calculator = 10;
	while(!quit)
	{
		wrefresh(meniu);
		nodelay(meniu, 1);
		while(wgetch(meniu) != ERR);
		nodelay(meniu, 0);
		usleep(1000000 / FRAMERATE);
		tasta = wgetch(meniu);
		switch (tasta)
		{		
			case KEY_UP:
				if(pozitie > 1)
				{
					pozitie--;
					//beep();
				}
				break;
			case KEY_DOWN:
				if(pozitie < OPTIUNI)
				{
					pozitie++;
					//beep();

				}
				break;
			case '\n':
			{	
				beep();
				if(nr_harti_valide && pozitie == 1)
				{
					if(pozitie == 1 || (pozitie == 2 && resume))
					{
						wclear(meniu);
                                                wrefresh(meniu);
						if(pozitie == 1)
						{
							attron(COLOR_PAIR(4) | A_BOLD);
							while(1)
							{
								alegere = meniu_config(grid_preview, nr_harti_valide, harti_jucator, lista_harti);
								if(alegere == -1)
								{	
									gamemode = -1;
									break;
								}
								gamemode = meniu_gm();
								if(gamemode != -1)
									break;
							}
							attroff(COLOR_PAIR(4) | A_BOLD);
							if(alegere >= 0)
							{
								if(alegere == 0)
									indice = rand() % (nr_harti_valide);
								else
									indice = alegere - 1;
								for(i = 1; i <= 10; i++)
									for(j = 1; j <= 10; j++)
											harta_aleasa[i][j] = harti_jucator[indice][i][j];
								for(i = 0; i < 12; i++)
									for(j = 0; j < 12; j++)
									{	
										harta_calculator[i][j] = 0;
										harta_atac[i][j] = 0;
										harta_atac_comp[i][j] = 0;
									}
								initializare_set_nave(&flota_jucator);
								initializare_set_nave(&flota_calculator);
								generator_harta(&flota_calculator, harta_calculator);
								random = 1;
								coord[0] = 1; // y
								coord[1] = 2; // x
								nr_nave_jucator = 10;
								nr_nave_calculator = 10;
								resume = 1;
								for(i = 0; i < 2; i++)
								{
									lovituri_calculator[i] = 0;
									lovituri_jucator[i] = 0;
									aux[i] = 0;
								}
							}
						}
								getmaxyx(stdscr, row, col);
                                                                mvwin(grid_jucator, (row - 12) / 2, col / 2 - 25);
                                                                mvwin(grid_calculator, (row - 12) / 2, col / 2 + 2);

						tasta = 'a';
						while(tasta != 'q' && alegere >= 0)
						{
							attron(A_BOLD);
							mvprintw((LINES - 12) / 2 - 2, COLS / 2 - 24, "PLAYER");
							mvprintw((LINES - 12) / 2 - 1, COLS / 2 - 24, "MAP");
							mvprintw((LINES - 12) / 2 - 2, COLS / 2 + 16, "COMPUTER");
							mvprintw((LINES - 12) / 2 - 1, COLS / 2 + 21, "MAP");
							attroff(A_BOLD);
							if(gamemode == 0 && tasta == 'r')
							{
								aux[0] += lovituri_calculator[0];
								aux[1] += lovituri_calculator[1];
								lovituri_calculator[0] = 0;
								lovituri_calculator[1] = 0;
								scan_harta(harta_aleasa, &flota_jucator);
								harta_dupa_randomizare(&flota_jucator, harta_atac_comp);
								harta_dupa_randomizare(&flota_jucator, harta_aleasa);
								generator_harta(&flota_jucator, harta_aleasa);
								contor = 0;
								for(i = 1; i <= 4; i++)
								{
									contor += nr_nave_lungime(flota_jucator, i, 0) * (float)i;
								}
								aux[0] -= contor;
								aux[1] -= contor;
							}
							if(gamemode == 0 && tasta == 'd')
							{
								for(i = 1; i <= 10 && nr_nave_jucator && nr_nave_calculator; i++)
								{
									atac_random(harta_atac, harta_calculator);
									atac_random(harta_atac_comp, harta_aleasa);
									scan_harta(harta_aleasa, &flota_jucator);
									scan_harta(harta_calculator, &flota_calculator);
									nr_nave_jucator = nave_ramase(flota_jucator);
									nr_nave_calculator = nave_ramase(flota_calculator);

								}								
							}
							if(tasta == KEY_RESIZE)
                                                        {
                                                               	clear();
                                                               	getmaxyx(stdscr, row, col);
                                                               	wclear(grid_jucator);
                                                               	wclear(grid_calculator);
                                                               	mvwin(grid_jucator, (row - 12) / 2, col / 2 - 25);
                                                               	mvwin(grid_calculator, (row - 12) / 2, col / 2 + 2);
								attron(A_BOLD);
                                                        	mvprintw((LINES - 12) / 2 - 2, COLS / 2 - 24, "PLAYER");
                                                        	mvprintw((LINES - 12) / 2 - 1, COLS / 2 - 24, "MAP");
                                                        	mvprintw((LINES - 12) / 2 - 2, COLS / 2 + 16, "COMPUTER");
                                                       		mvprintw((LINES - 12) / 2 - 1, COLS / 2 + 21, "MAP");
								attroff(A_BOLD);
								refresh();
                                                       	}
							print_grid(grid_jucator);
							print_grid(grid_calculator);
							afisare_nave(grid_jucator, harta_aleasa);
							afisare_nave(grid_calculator,harta_atac);
							
							for(i = 0; i < 8; i++)
								mvchgat(y / 2 + 10 + i, x / 4, -1, A_INVIS, 0, NULL);

							mvaddch(y / 2 + 10, x / 4, ' ' | COLOR_PAIR(1));
							mvaddch(y / 2 + 12, x / 4, 'X' | COLOR_PAIR(1));
							mvaddch(y / 2 + 10, (3 * x) / 4 - 20, 'O' | COLOR_PAIR(3));
							mvaddch(y / 2 + 12, (3 * x) / 4 - 20, ' ' | COLOR_PAIR(2));
							
							attron(A_BOLD);
							mvprintw(y / 2 + 10, x / 4 + 1, " - INTACT SHIP PART" );
							mvprintw(y / 2 + 12, x / 4 + 1, " - DESTROYED SHIP PART");
                                                        mvprintw(y / 2 + 10, (3 * x) / 4 - 19, " - MISSED SPOT" );
                                                        mvprintw(y / 2 + 12, (3 * x) / 4 - 19, " - PLAYER SELECTION");
							attroff(A_BOLD);

							attron(COLOR_PAIR(4));
							mvprintw(y / 2 + 14, (x - 32) / 2, "USE ARROW KEYS TO MOVE SELECTION");
							mvprintw(y / 2 + 15, (x - 37) / 2, "PRESS <ENTER> TO ATTACK SELECTED SPOT");
							if(!gamemode)
							{
								mvprintw(y / 2 + 16, (x - 31) / 2, "PRESS <R> TO RANDOMIZE YOUR MAP");
								mvprintw(y / 2 + 17, (x - 44) / 2, "PRESS <D> TO DESTROY 10 POSITIONS IN ADVANCE");
							}
							attroff(COLOR_PAIR(4));
							// indicatii jucator

							if(interfata_atac(grid_calculator, coord, tasta, harta_atac, harta_calculator, &nr_nave_calculator) == 0)
							{
								getmaxyx(stdscr, y, x);
								if(gamemode)
								{
									delay(0.5);
									while(comp_atac(harta_atac_comp, harta_aleasa, &random, &flota_jucator))
									{
										beep();
										delay(0.1);
										beep();
										afisare_nave(grid_jucator, harta_aleasa);
										wrefresh(grid_jucator);
										refresh();
                      							mvprintw(y / 2 + 8, (x - 15) / 2, "               ");
										attron(COLOR_PAIR(5) | A_BLINK);
                        							mvprintw(y / 2 + 8, (x - 14) / 2, "SUCCESSFUL HIT");
										attroff(COLOR_PAIR(5) | A_BLINK);
										refresh();
										delay(1.5);
										scan_harta(harta_aleasa, &flota_jucator);
										nr_nave_jucator = nave_ramase(flota_jucator);
										if(nr_nave_jucator == 0)
											break;
                        							mvprintw(y / 2 + 8, (x - 15) / 2, "               ");
										attron(A_BOLD | COLOR_PAIR(4));
										mvprintw(y / 2 + 8, (x - 15) / 2, "COMPUTER'S TURN");
										refresh();
										attroff(A_BOLD | COLOR_PAIR(4));  
										delay(0.5);
									}
								}
								else
								{
									delay(3);
									while(atac_random(harta_atac_comp, harta_aleasa))
									{
										beep();
										delay(0.1);
										beep();
										afisare_nave(grid_jucator, harta_aleasa);
										wrefresh(grid_jucator);
                     								mvprintw(y / 2 + 8, (x - 15) / 2, "               ");
										attron(COLOR_PAIR(5) | A_BLINK);
                        							mvprintw(y / 2 + 8, (x - 14) / 2, "SUCCESSFUL HIT");
										attroff(COLOR_PAIR(5) | A_BLINK);
										refresh();
										delay(1.5);
										scan_harta(harta_aleasa, &flota_jucator);
										nr_nave_jucator = nave_ramase(flota_jucator);
										if(nr_nave_jucator == 0)
											break;
                        							mvprintw(y / 2 + 8, (x - 15) / 2, "               ");
										attron(A_BOLD | COLOR_PAIR(4));
										mvprintw(y / 2 + 8, (x - 15) / 2, "COMPUTER'S TURN");
										refresh();
										attroff(A_BOLD | COLOR_PAIR(4));  
										delay(3);
									}
								}
								if(nr_nave_jucator)
								{
									beep();
									getmaxyx(stdscr, y, x);
									attron(A_BLINK | COLOR_PAIR(1) | A_BOLD);
                               						mvprintw(y / 2 + 8, (x - 15) / 2, "               ");
									mvprintw(y / 2 + 8, (x - 11) / 2, "M I S S E D");
									attroff(A_BLINK | COLOR_PAIR(1) | A_BOLD);
									refresh();
									delay(1.5);
									mvprintw(y / 2 + 8, (x - 15) / 2, "               ");
									attron(COLOR_PAIR(4) | A_BOLD);
                                					mvprintw(y / 2 + 8, (x - 13) / 2, "PLAYER'S TURN");
									attroff(COLOR_PAIR(4) | A_BOLD);
									refresh();
									afisare_nave(grid_jucator, harta_aleasa);
									wrefresh(grid_jucator);
								}

							}
							scan_harta(harta_aleasa, &flota_jucator);
							scan_harta(harta_calculator, &flota_calculator);
							nr_nave_jucator = nave_ramase(flota_jucator);
							nr_nave_calculator = nave_ramase(flota_calculator);
							calc_lovituri(harta_atac_comp, lovituri_calculator);
							refresh();
							if(nr_nave_jucator == 0)
							{
								resume = 0;
								break;
							}
							if(nr_nave_calculator == 0)
							{
								resume = 0;
								break;
							}
							attron(A_BOLD);
							if(gamemode == 0)
								mvprintw(2, 1, "STANDARD MODE");
							else
								mvprintw(2, 1, "EXPERT MODE");
							attroff(A_BOLD);
							getmaxyx(stdscr, y, x);
							mvprintw(y / 2 + 8, (x - 15) / 2, "               ");
                                                        attron(COLOR_PAIR(4) | A_BOLD);
                                                        mvprintw(y / 2 + 8, (x - 13) / 2, "PLAYER'S TURN");
                                                        attroff(COLOR_PAIR(4) | A_BOLD);
                                                        refresh();
								refresh();
								nodelay(grid_calculator, 1);
								while(wgetch(grid_calculator) != ERR);
								nodelay(grid_calculator, 0); // structura necesara pt a impiedica spamul tastelor								
								//usleep( 1000000 / FRAMERATE);
								//wtimeout(grid_calculator, FRAMERATE);	
								tasta = wgetch(grid_calculator);
						}
					}
				}
				else
					if(nr_harti_valide == 0 && pozitie == 1)
					{
			                        pop_up(12, 60, "It seems like either none of the maps in Maps/ folder are built correctly or you do not have any maps. Either manually insert a valid map in Maps/ folder or create one using the Map Editor available in the main menu.", 1, ok_opt);	
					}
					if(pozitie == 3)
					{
						wclear(meniu);
						wrefresh(meniu);
						//pop_up(15, 40, "Are you sure you want to exit? This configuration will be lost! All things aside pretty neat program! Ok Please dont exit!");		
						Editor_harta(grid_preview, harti_jucator, lista_harti, nr_harti_valide);
						clear_list(&lista_harti, nr_harti_valide);
						deschidere_harti( &lista_harti, &harti_jucator, &nr_harti_valide, &capacitate);
						//deschidere_harti( &lista_harti, &harti_jucator, &nr_harti_valide, &capacitate);
						/*if(auxiliar > nr_harti_valide)
						{
							if(auxiliar > capacitate)
							{
								capacitate += (auxiliar - nr_harti_valide) + 10;
								harti_jucator = realloc(harti_jucator, capacitate * sizeof(int**));
							}
							for(i = nr_harti_valide; i < auxiliar; i++)
							{
								strcpy(filename, PATHMAP);
								strcat(filename, lista_harti[i]);
								harta = fopen(filename, "r");
								harti_jucator[i] = sablon_harta(harta);
								fclose(harta);
							}
							nr_harti_valide = auxiliar;
						}*/
					}
					if(pozitie == 4)
					{
						wclear(meniu);
						wrefresh(meniu);
						if(access(PATHBIN, F_OK) != -1)
						{
							f = fopen(PATHBIN, "rb");
							fread(v, sizeof(SCOR), 2, f);
							fclose(f);
							while(1)
							{
								clear();
								refresh();
								attron(COLOR_PAIR(4) | A_BOLD);
								custom_text(doom, "OVERALL STATUS", 3, 14, (COLS - 87) / 2, "Fonts/JS.font");
								custom_text(doom, "PLAYER", 3, LINES / 2, (COLS - 84) / 4, "Fonts/JS.font");
								custom_text(doom, "COMPUTER", 3, LINES / 2, (3 * COLS - 116) / 4, "Fonts/JS.font");
								attroff(A_BOLD);
								mvprintw(LINES / 2 + 2, (COLS - 50) / 4, "TOTAL SHIPS DESTROYED - %d", v[0].total_nave_distruse);
								mvprintw(LINES / 2 + 2, (3 * COLS - 50) / 4, "TOTAL SHIPS DESTROYED - %d", v[1].total_nave_distruse);
								mvprintw(LINES / 2 + 4, (COLS - 44) / 4, "OVERALL EFFICIENCY - %0.2f %%", v[0].total_eficienta);
								mvprintw(LINES / 2 + 4, (3 * COLS - 44) / 4, "OVERALL EFFICIENCY - %0.2f %%", v[1].total_eficienta);
								mvprintw(LINES / 2 + 6, (COLS - 56) / 4, "TOTAL STANDARD GAMES WON - %d", v[0].jocuri_standard_castigate);
								mvprintw(LINES / 2 + 6, (3 * COLS - 56) / 4, "TOTAL STANDARD GAMES WON - %d", v[1].jocuri_standard_castigate);
								mvprintw(LINES / 2 + 8, (COLS - 52) / 4, "TOTAL EXPERT GAMES WON - %d", v[0].jocuri_expert_castigate);
								mvprintw(LINES / 2 + 8, (3 * COLS - 52) / 4, "TOTAL EXPERT GAMES WON - %d", v[1].jocuri_expert_castigate);
								attroff(COLOR_PAIR(4));
								tasta = getch();
								if(tasta == '\n')
									break;
							}	
						}
						else
							while(1)
							{
								clear();
								attron(A_BOLD | A_BLINK);
								custom_text(doom, "No Data available", 3, LINES / 2, (COLS - 90) / 2, "Fonts/JS.font");
								attroff(A_BLINK);
								mvprintw(LINES / 2 + 6, (COLS - 31) / 2, "YOUR SCORE FILE IS CORRUPTED OR");
								mvprintw(LINES / 2 + 7, (COLS - 28) / 2, "YOU HAVEN'T FINISHED PLAYING");
								mvprintw(LINES / 2 + 8, (COLS - 12) / 2, "ANY GAME YET");
								attron(A_BLINK);
								mvprintw(LINES / 2 + 10 , (COLS - 36) / 2, "PRESS <ENTER> TO RETURN TO MAIN MENU");
								attroff(A_BOLD | A_BLINK);
								refresh();
								tasta = getch();
								if(tasta == '\n')
									break;
							}
					}
					if(pozitie == 5)
						quit = 1;

			}
			break;
		}
		if(alegere >= 0 && (nr_nave_jucator == 0 || nr_nave_calculator == 0))
		{
		noecho();	
			lovituri_calculator[0] += aux[0];
			lovituri_calculator[1] += aux[1];
			calc_lovituri(harta_atac, lovituri_jucator);
			v[0].total_eficienta = (lovituri_jucator[0] * 100 ) / lovituri_jucator[1];
			v[0].total_nave_distruse = 10 - nave_ramase(flota_calculator);
			v[1].total_nave_distruse = 10 - nave_ramase(flota_jucator);
			v[1].total_eficienta = (lovituri_calculator[0] * 100) / lovituri_calculator[1];
			if(nr_nave_jucator)
			{
				v[0].jocuri_standard_castigate = (gamemode + 1) % 2;
				v[0].jocuri_expert_castigate = gamemode;
                                v[1].jocuri_standard_castigate = 0;
                                v[1].jocuri_expert_castigate = 0;

			}
			else
                        {
                                v[1].jocuri_standard_castigate = (gamemode + 1) % 2;
                                v[1].jocuri_expert_castigate = gamemode;
                                v[0].jocuri_standard_castigate = 0;
                                v[0].jocuri_expert_castigate = 0;

                        }
			scriere_scor(v);


			tasta = 'a';
			animatie_terminata = 0;
				clear();
				attron(A_BOLD | COLOR_PAIR(4));
				if(nr_nave_jucator == 0 && nr_nave_calculator == 0)
				{
					scrie_text("draw.txt", stdscr, (LINES - h_text("draw.txt")) / 2, (COLS - l_text("draw.txt")) / 2);
					refresh();
					strcpy(winner, "draw.txt");
				}
				if(nr_nave_calculator == 0 && nr_nave_jucator != 0)
				{
					scrie_text("Player_wins.txt", stdscr, (LINES - h_text("Player_wins.txt")) / 2, (COLS - l_text("Player_wins.txt")) / 2);
					refresh();
					strcpy(winner, "Player_wins.txt");
					beep();
					delay(0.5);
					for(i = 1; i <= 4; i++)
					{
						beep();
						delay(0.2);
					}
					delay(0.4);
					beep();
					delay(0.3);
					beep();
					// melodie Player wins
				}
				else
					if(nr_nave_jucator == 0 && nr_nave_calculator != 0)
					{
						strcpy(winner, "Computer_wins.txt");
						scrie_text("Computer_wins.txt", stdscr, (LINES - h_text("Computer_wins.txt")) / 2, (COLS - l_text("Computer_wins.txt")) / 2);
						refresh();
						for(i = 1; i <= 3; i++ )
						{
							beep();
							delay(0.3);
						}
						delay(0.1);
						beep();
						// melodie Computer wins
					}
				delay(1);
				clear();
				for(i = 1; i < (LINES - h_text(winner)) / 2; i++)
				{	
					scrie_text(winner, stdscr, (LINES - h_text(winner)) / 2 - i, (COLS - l_text(winner)) / 2);
					refresh();
					delay(0.1);
					clear();
				}
				attroff(A_BOLD);
				while(tasta != '\n')
				{
					attron(A_BLINK | A_BOLD);
					scrie_text(winner, stdscr, 0, (COLS - l_text(winner)) / 2);
					attroff(A_BLINK | A_BOLD);
					row = h_text(winner) / 2 + (LINES - 15) / 2;
					col = (COLS - l_text(winner)) / 2;
					mvprintw(row, col - 25, "P L A Y E R");
					mvprintw(row, (COLS + l_text(winner)) / 2, "C O M P U T E R");
					mvprintw(row + 3, col - 25, "DESTROYED %d ENEMY SHIPS",10 - nr_nave_calculator);
					mvprintw(row + 3, (COLS + l_text(winner)) / 2, "DESTROYED %d ENEMY SHIPS",10 - nr_nave_jucator);
				
					for(i = 0; i <= 6; i += 2)
					{
						for(j = 0; j < i / 2 + 1; j++)
							mvaddch(row + i + 5, col - 25 + j, 'X' | COLOR_PAIR(1));
						mvprintw(row + i + 5, col - 25 + i / 2 + 2, "X %d", nr_nave_lungime(flota_calculator, i / 2 + 1, 0));
						refresh();
						if(animatie_terminata == 0)
							delay(0.2);	
					}
					for(i = 0; i <= 6; i += 2)
					{
                                        	for(j = 0; j < i / 2 + 1; j++)
                                                	mvaddch(row + i + 5, (COLS + l_text(winner)) / 2 + j, 'X' | COLOR_PAIR(1));
                                        	mvprintw(row + i + 5, (COLS + l_text(winner)) / 2 + i / 2 + 2, "X %d", nr_nave_lungime(flota_jucator, i / 2 + 1, 0));
						refresh();
						if(animatie_terminata == 0)
							delay(0.2);
                                	
					}
					mvprintw(row + 13, col - 25, "EFFICIENCY : %0.2f %%", (lovituri_jucator[0] * 100) / lovituri_jucator[1]);
					mvprintw(row + 13, (COLS + l_text(winner)) / 2, "EFFICIENTCY : %0.2f %%", (lovituri_calculator[0] * 100)/ lovituri_calculator[1]);
					mvprintw((LINES - 1) / 2,(COLS - 23) / 2 , "PRESS ENTER TO CONTINUE");
					refresh();
					animatie_terminata = 1;
					tasta = getch();
					clear();
				}
				
			attroff(COLOR_PAIR(4));
			clear();
			nr_nave_jucator = 10;
			nr_nave_calculator = 10;

		}
		meniu_joc(meniu, optiuni_meniu, pozitie);
		
	}
	free_harta(&harta_aleasa);
	free_harta(&harta_calculator);
	free_harta(&harta_atac);
	free_harta(&harta_atac_comp);
	for(i = 0; i < nr_harti_valide; i++)
	{
		for(j = 0; j < 12; j++)
			if(harti_jucator[i][j] != NULL)
				free(harti_jucator[i][j]);
		if(harti_jucator[i] !=NULL)
			free(harti_jucator[i]);
	}
	if(nr_harti_valide)
		free(harti_jucator);
	endwin();
	return 0;

}
