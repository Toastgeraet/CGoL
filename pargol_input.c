#include<stdlib.h>
#include<stdio.h>

int * createWorldFromTxt(char * name, int * population, int xlen, int ylen, int zlen)
{
	printf("Creating space for intial world...\n");
	int worldsize = xlen*ylen*zlen;
	int * tempworld = malloc(worldsize * sizeof(int));
	printf("Finished creating world.\n");

	* population = 0;

	FILE *fp;
	int c;

	printf("Starting to parse initial world from file...\n");
	if (!(fp = fopen(name, "r"))) {
		perror(name);
		exit(1);
	}
	for (int i = 0; i < worldsize; i++)
	{
		if ((c = fgetc(fp)) == EOF) break;
		if (c == '0') tempworld[i] = 0;
		else if (c == '1') {
			tempworld[i] = 1;
			* population++;
		}
		else i--; //because it's a linebreak char or something			
	}
	fclose(fp);
	printf("Finished creating world and parsing input file.\n\n");
	printf("Initial population = %d\n", * population);
	return tempworld;
}