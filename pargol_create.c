#include<stdio.h>
#include<string.h>
#include <stdlib.h>

int createTXT(int xlen, int ylen, int zlen)
{
	//printf("Writing to file \"%s\"...\n", name);
	for (int ii = 1; ii < 100001; ii++)
	{
		char * name = "world%d.txt", ii;
		FILE *fp;
		char * mode = "write";
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
	}
	//printf("Finished writing.\n");
	return 0;
}