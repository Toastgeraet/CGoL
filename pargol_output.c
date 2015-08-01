#include<stdio.h>
#include<string.h>
#include "pargol_logic.h"

int outputTXT(char * name, char * mode, char * text, int * world, int xlen, int ylen, int zlen)
{
	//printf("Writing to file \"%s\"...\n", name);
	FILE *fp;

	if (strcmp(mode, "write") == 0)
		fp = fopen(name, "w");
	else if (strcmp(mode, "append") == 0)
		fp = fopen(name, "a");
	else{
		printf("Bitte den Schreibmodus angeben.\n", name);
		return 1;
	}

	if (fp == NULL)
	{
		printf("Datei %s konnte nicht geoeffnet werden.\n", name);
		return 1;
	}
	else
	{
		fwrite(text, sizeof(char), strlen(text), fp);
		fputc(10, fp);
		if (world != NULL)
		{
			for (int k = 0; k < zlen; k++)
			{
				for (int j = 0; j < ylen; j++)
				{
					for (int i = 0; i < xlen; i++)
					{
						if (world[offset(i, j, k, xlen, ylen)])
							fputc('1', fp);
						else
							fputc('0', fp);
					}
					fputc(10, fp);
				}
				fputc(10, fp);
			}
		}
		fclose(fp);
	}
	//printf("Finished writing.\n");
	return 0;
}