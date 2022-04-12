#include "librarii.h"
#include "nava_impl.h"
#define PATHMAP "Maps/"

int verifica_harta(char *nume_fisier, char *cale)
{
	char aux[200];
	strcpy(aux, cale);
	strcat(aux, nume_fisier);
	FILE *f = fopen(aux, "r");
	int contor = 0;
	int i,j;
	int **harta;
	NAVA flota;
	while(fgets(aux, 200, f) != NULL)
	{
		contor++;
		if(strlen(aux) != 22)
			return 0;
		for(i = 0; i < 21; i++)
			if(((i % 2 == 0) && aux[i] != '|' ) || ((i % 2 == 1) && (aux[i] != 'X' && aux[i] != ' ')))
				return 0;
		if(aux[21] != '\n')
			return 0;
	}
	fseek(f, 0, SEEK_SET);
	harta = sablon_harta(f);
	//initializare_set_nave(&flota);
	scan_harta(harta, &flota);
	if(nave_ramase(flota) != 10)
	{
		free_harta(&harta);
		return 0;
	}
	for(i = 1; i <= 4; i++)
		if(nr_nave_lungime(flota, i, 1) != (5 - i))
		{
			free_harta(&harta);
			return 0;
		}
	for(i = 0; i < 10; i++)
		if(flota.orizontal[i])
		{
			for(j = -1; j <= flota.lungime[i]; j++)
			{
				if(harta[flota.x[i] - 1][flota.y[i] + j] || harta[flota.x[i] + 1][flota.y[i] + j] )
				{
					free_harta(&harta);
					return 0;
				}
			}
		}
		else
			for(j = -1; j <= flota.lungime[i]; j++)
				if(harta[flota.x[i] + j][flota.y[i] - 1] || harta[flota.x[i] + j][flota.y[i] + 1] )
				{
					free_harta(&harta);
					return 0;
				}
	free_harta(&harta);
	return 1;

}
char **verifica_fisier(int *nr_elem)
{
	DIR *d;
	struct dirent *dir;
	char **lista;
	int contor = 0;
	int cap = 10;
	int test;

	lista = malloc(cap * sizeof(char*));
	*nr_elem = 0;
	d = opendir(PATHMAP);
	if(d)
	{
		while((dir = readdir(d)) != NULL)
		{
			if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..") && verifica_harta(dir->d_name, PATHMAP))
			{
				lista[contor] = malloc((strlen(PATHMAP) + strlen(dir->d_name) + 1) * sizeof(char));
				strcpy(lista[contor++], dir->d_name);
			}
			if(contor == cap)
			{
				cap *= 10;
				lista = realloc(lista, cap * sizeof(char*));
			}	
		}
		if(contor)
		{
			*nr_elem = contor;
			return lista;
		}
		else
			return NULL;
	}
	else
		return NULL;
}
