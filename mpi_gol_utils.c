// Created by Kolesnikov S.S., Vecherkin B.I.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "mpi_gol_logic.h"


char * getline() {
	char * line = malloc(100), *linep = line;
	size_t lenmax = 100, len = lenmax;
	int c;

	if (line == NULL)
		return NULL;

	for (;;) {
		c = fgetc(stdin);
		if (c == EOF)
			break;

		if (--len == 0) {
			len = lenmax;
			char * linen = realloc(linep, lenmax *= 2);

			if (linen == NULL) {
				free(linep);
				return NULL;
			}
			line = linen + (line - linep);
			linep = linen;
		}

		if ((*line++ = c) == '\n')
			break;
	}
	*line = '\0';
	return linep;
}

void parseArguments(int argc, char * argv[], 
	char * inFile, 
	int * xlen, int * ylen, int * zlen, 
	int * maxGens) {

	//printf("Starting to parse arguments...\n");
	//printf("Program name: %s\n", argv[0]);
	//printf("Input file: %s\n", argv[1]);

	//'Usual' startup parameters

	if (argc >= 8) {
		inFile = argv[1];
		while ((argc > 2) && (argv[2][0] == '-')) {
			char ** str = &argv[3];

			switch (argv[2][1])	{
				case 'x':
					//printf("x = %s\n", *str);
					*xlen = (int)strtol(*str, (char **)NULL, 10);
					break;

				case 'y':
					//printf("y = %s\n", *str);
					*ylen = (int)strtol(*str, (char **)NULL, 10);
					break;

				case 'z':
					//printf("z = %s\n", *str);
					*zlen = (int)strtol(*str, (char **)NULL, 10);
					break;

				case 'g':
					//printf("Worlds will evolve for %s generations.\n", *str);
					*maxGens = (int)strtol(*str, (char **)NULL, 10);
					break;

				default:
					//printf("Wrong Argument: %s\n", argv[1]);
					exit(8);
			}

			argv += 2;
			argc -= 2;
		}
	}
}


int * createWorldFromTxt(char * name, int * population, 
	int xlen, int ylen, int zlen) {
	
	//printf("Creating space for intial world...\n");
	int worldsize = xlen*ylen*zlen;
	int * tempworld = malloc(worldsize * sizeof(int));
	//printf("Finished creating world.\n");

	* population = 0;

	FILE *fp;
	int c;

	if (!(fp = fopen(name, "r"))) {
		perror(name);
		exit(1);
	}

	for (int i = 0; i < worldsize; i++)	{
		if ((c = fgetc(fp)) == EOF) {
			break;
		}
		if (c == '0') {
			tempworld[i] = 0;
		} else if (c == '1') {
			tempworld[i] = 1;
			*population = *population + 1;
		} else {
			i--; // linebreak char
		}
	}

	fclose(fp);
	//printf("Finished creating world and parsing input file.\n\n");
	//printf("Initial population = %d\n", * population);
	return tempworld;
}

int outputTXT(char * name, char * mode, char * text, int * world, 
	int xlen, int ylen, int zlen) {

	//printf("Writing to file \"%s\"...\n", name);
	FILE *fp;

	if (strcmp(mode, "write") == 0) {
		fp = fopen(name, "w");
	} else if (strcmp(mode, "append") == 0) {
		fp = fopen(name, "a");
	}
	else {
		printf("Please specify the write mode.\n", name);
		return 1;
	}

	if (fp == NULL)	{
		printf("File %s could not be opened.\n", name);
		return 1;
	}
	else {
		fwrite(text, sizeof(char), strlen(text), fp);
		fputc(10, fp);
		if (world != NULL) {
			for (int k = 0; k < zlen; k++) {
				for (int j = 0; j < ylen; j++) {
					for (int i = 0; i < xlen; i++) {
						if (world[offset(i, j, k, xlen, ylen)]) {
							fputc('1', fp);
						}
						else {
							fputc('0', fp);
						}
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