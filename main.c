#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int offset(int x, int y, int z, int xlen, int ylen) { return (z * xlen * ylen) + (y * xlen) + x; }

int xlen = 0, ylen = 0, zlen = 0, cellcount = 0;
int population = 0;
char * inputFile = NULL;


int * createWorldFromTxt(char * name, int xlen, int ylen, int zlen)
{
	printf("Creating space for intial world...\n");
	int worldsize = xlen*ylen*zlen;
	int * tempworld = malloc(worldsize * sizeof(int));
	printf("Finished creating world.\n");

	population = 0;

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
			population++;
		}
		else i--; //because it's a linebreak char or something			
	}
	fclose(fp);
	printf("Finished creating world and parsing input file.\n\n");
	printf("Initial population = %d\n", population);
	return tempworld;
}

int count_neighbours(int * world, int xlen, int ylen, int zlen, int x, int y, int z)
{

	int count = 0;
	int starti = -1;
	int startj = -1;
	int startk = -1;
	int endi = 1;
	int endj = 1;
	int endk = 1;
	int xo = 0;
	int yo = 0;
	int zo = 0;

	for (int k = startk; k <= endk; k++)
	{
		for (int j = startj; j <= endj; j++)
		{
			for (int i = starti; i <= endi; i++)
			{
				if (x + i < 0)
					xo = xlen;
				else if (x + i > xlen)
					xo = 0;
				else 
					xo = x + i;

				if (y + j < 0)
					yo = ylen;
				else if (y + j > ylen)
					yo = 0;
				else 
					yo = y + j;

				if (z + k < 0)
					zo = zlen;
				else if (z + k > zlen)
					zo = 0;
				else 
					zo = z + k;

				int index = offset(xo, yo, zo, xlen, ylen);
				if (i == 0 && j == 0 && k == 0) continue;
				if (world[index]) count++;
			}
		}
	}

	return count;
}

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

void usage(void)
{
	printf("Usage:\n");
	printf(" -f<name>\n");
	printf(" -d<name>\n");
	exit(8);
}

void parseArguments(int argc, char * argv[]){
	printf("Starting to parse arguments...\n");
	printf("Program name: %s\n", argv[0]);
	printf("Input file: %s\n", argv[1]);
	inputFile = argv[1];

	while ((argc > 2) && (argv[2][0] == '-'))
	{
		char ** str = argv[3];

		switch (argv[2][1])
		{
		case 'x':
			printf("x = %s\n", str);
			xlen = (int)strtol(str, (char **)NULL, 10);
			break;

		case 'y':
			printf("y = %s\n", str);
			ylen = (int)strtol(str, (char **)NULL, 10);
			break;

		case 'z':
			printf("z = %s\n", str);
			zlen = (int)strtol(str, (char **)NULL, 10);
			break;

		default:
			printf("Wrong Argument: %s\n", argv[1]);
			usage();
		}

		argv += 2;
		argc -= 2;
	}
	printf("Finished parsing of arguments.\n\n");
}

int main(int argc, char * argv[])
{
	parseArguments(argc, argv);

	//Dimensionen der Welt	
	cellcount = xlen*ylen*zlen;

	//Allocate memory for worlds
	int * current; // = malloc(cellcount, sizeof(int));
	int * next = calloc(cellcount, sizeof(int));

	//How long should it evolve
	int maxGenerationen = 100;

	//Regeln
	int minToLive = 2;
	int maxToLive = 3;
	int minToResurrect = 3;
	int maxToResurrect = 3;

	//Parse input Txt to create initial world
	current = createWorldFromTxt(inputFile, xlen, ylen, zlen);
	//char * output_name_buf = malloc((100)*sizeof(char)); // max filename length
	char * addtext = malloc((100)*sizeof(char));

	sprintf(addtext, "Create this file... Date or something - maybe rules here...\n");
	outputTXT("output_combined.txt", "write", addtext, NULL, xlen, ylen, zlen);

	//Generationen berechnen
	for (int generationX = 0; generationX < maxGenerationen; generationX++)
	{
		sprintf(addtext, "Generation: %d\nPopulation: %d\n", generationX, population);
		outputTXT("output_combined.txt", "append", addtext, current, xlen, ylen, zlen);

		population = 0;
		for (int k = 0; k < zlen; k++)
		{
			for (int j = 0; j < ylen; j++)
			{
				for (int i = 0; i < xlen; i++)
				{
					int index = offset(i, j, k, xlen, ylen);					

					int c = count_neighbours(current, xlen, ylen, zlen, i, j, k);

					if (current[index])
					{
						if (next[index] = c >= minToLive && c <= maxToLive)
							population++;
					}
					else
					{
						if (next[index] = c >= minToResurrect && c <= maxToResurrect)
							population++;
					}
				}
			}
		}

		free(current);
		current = next;
		next = calloc(cellcount, sizeof(int));
	}
	//free(output_name_buf);
	printf("Finished evolving for %d generations.\n\nPress any key to close this window.", maxGenerationen);
	//getline();
}

