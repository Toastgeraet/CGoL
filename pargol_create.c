#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int createWorld(int number, int xlen, int ylen, int zlen)
{
	char * name = malloc((100)*sizeof(char));
	sprintf(name, "inputfiles\\world%d.txt", number);
	
	FILE *fp = fopen(name, "w");

	if (fp == NULL)
	{
		printf("Datei %s konnte nicht geoeffnet werden.\n", name);
		return 1;
	}
	else
	{
		fputc(10, fp);

		for (int k = 0; k < zlen; k++)
		{
			for (int j = 0; j < ylen; j++)
			{
				for (int i = 0; i < xlen; i++)
				{
					int random = rand();
					if (random % 2 == 1)
						fputc('1', fp);
					else
						fputc('0', fp);
				}
				fputc(10, fp);
			}
			fputc(10, fp);
		}
		fclose(fp);
	}

	free(name);
	return 0;
}