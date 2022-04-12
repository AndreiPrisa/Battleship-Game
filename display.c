#define PATH "Fonts/"
#define BIN "font.bin"
#include "librarii.h"
#include "grafica.h"
/*void optimizare_font(char *nume_font)
{
	FILE *f = fopen(nume_font, "r+");
	int lungime;
	char aux[200];
	long int poz = SEEK_SET;

	while(fgets(aux, 200, f) != NULL)
	{
		lungime = strlen(aux) - 1;
		while(aux[lungime - 1] == ' ' && lungime > 0)
			lungime--;
		if(lungime)
		{
			aux[lungime] = '\n';
			aux[lungime + 1] = '\0';
			fseek(f, poz, SEEK_SET);
			//printf("%s\n", aux);
			fprintf(f, "%s", aux);
		}
		poz = ftell(f);
	}
	fclose(f);
}*/


FONT initializare_font(char *nume_font)
{
	FONT font;
	int i ,j = 0 , lungime, cauta = 1, t;
	char aux[200];
	FILE *f;
	sprintf(aux, "%s%s", PATH, nume_font);
	f = fopen(aux, "r");
	long int pozitie, contor = 0;

	font.poz[0][0] = SEEK_SET;
	strcpy(font.nume, nume_font);
	for(i = 0; i <= 61; i++)
		font.lungime[i] = 0;
	//fgets(aux, 200, f);
	while(fgets(aux, 200, f) != NULL)
	{
		//if(!cauta)
			//contor++;
		if(aux[strlen(aux) - 1] == '\n')
			aux[strlen(aux) - 1] = '\0';
		lungime = strlen(aux);
		t = 1;
		/*while(aux[lungime - 1] == ' ' && lungime > 0)
			if(aux[--lungime - 1] != ' ')
			{
				t = 0;
			}
			*/
		for(i = 0; i < lungime; i++)
			if(aux[i] != ' ')
			{
				t = 0;
				break;
			}
		if(t == 0)
		{
			while(aux[lungime - 1] == ' ')
				lungime--;
			//printf("poz: %d %d\n", j, lungime);
			if(cauta)
			{
				cauta = 0;
				font.poz[j][0] = pozitie;
			}
				if(lungime > font.lungime[j])
					font.lungime[j] = lungime;
		}
		else
			if(t && cauta == 0)
			{
				//printf("%d %d\n\n", j, font.lungime[j]);
				font.poz[j++][1] = contor;
			       contor = 0;
		       		cauta = 1;	       
			}
		if(!cauta)
			contor++;
		pozitie = ftell(f);

	}
	//for(i = 0; i <= 61; i++)
		//printf("%d\n", font.lungime[i]);
	fclose(f);
	return font;
}

void custom_text (FONT font, char *text, int sp_carac, int y, int x, char *adresa_font)
{
	int poz, i, j, k, lungime_text, lungime_font = 0, adjust = 0;
	char aux[200];
	FILE *f = fopen (adresa_font, "r");

	lungime_text = strlen(text);
	for(i = 0; i < lungime_text; i++)
		if(text[i] != ' ')
		{
			if(text[i] < 'A')
				poz = text[i] - '0';
			else
				if(text[i] == tolower(text[i]))
					poz = text[i] - 'a' + 10;
				else
					poz = text[i] - 'A' + 'z' - 'a' + 11;
			if(tolower(text[i]) == 'g' || text[i] == 'y' || text[i] == 'p' || text[i] == 'q')
				adjust = font.poz[poz][1] / 2 - 1;
			else
				adjust = 0;
			for(j = font.poz[poz][1]; j >= 0; j--)
				{
					fseek(f, font.poz[poz][0], SEEK_SET);
					for(k = 0; k <= j; k++)
						fgets(aux, 200, f);
					//aux[font.lungime[poz]] = '\0';
					mvwprintw(stdscr, y - font.poz[poz][1] + adjust + j, x + lungime_font, "%s", aux);
				}
			lungime_font += font.lungime[poz];
			//mvwprintw(stdscr, 10, 10, "%d", font.lungime[poz]);
		}
		else
		{
			lungime_font += sp_carac;
		}

	refresh();
	fclose(f);
}


/*int main()
{
	//optimizare_font("Fonts/standard.font");
	FONT standard_font = initializare_font("JS.font");
	printf("%d\n", standard_font.lungime[3]);
	initscr();
	start_color();
	custom_text(standard_font, "SgylmpP", 10, 10, 1, "Fonts/JS.font");
	//nodelay(stdscr, 1);
	getch();
	endwin();
}*/
