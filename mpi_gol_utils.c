// Created by Kolesnikov S.S., Vecherkin B.I.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "mpi_gol_logic.h"

void parseArguments(int argc, char *argv[], 
	char *inFile, 
	int *xlen, int *ylen, int *zlen, 
	int *maxGens) {

	// startup parameters
	if (argc >= 8) {
		inFile = argv[1];
		while ((argc > 2) && (argv[2][0] == '-')) {
			char **str = &argv[3];

			switch (argv[2][1])	{
				case 'x':
					*xlen = (int)strtol(*str, (char **)NULL, 10);
					break;

				case 'y':
					*ylen = (int)strtol(*str, (char **)NULL, 10);
					break;

				case 'z':
					*zlen = (int)strtol(*str, (char **)NULL, 10);
					break;

				case 'g':
					*maxGens = (int)strtol(*str, (char **)NULL, 10);
					break;
			}

			argv += 2;
			argc -= 2;
		}
	}
}


int* createWorldFromTxt(char *name, int *population, 
	int xlen, int ylen, int zlen) {
	
	int worldsize = xlen * ylen * zlen;
	int *tempworld = malloc(worldsize * sizeof(int));

	*population = 0;

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
	return tempworld;
}

int outputTxt(char *name, char *mode, char *text, int *world, 
	int xlen, int ylen, int zlen) {

	FILE *fp;

	if (strcmp(mode, "write") == 0) {
		fp = fopen(name, "w");
	} else if (strcmp(mode, "append") == 0) {
		fp = fopen(name, "a");
	} else {
		printf("Please specify the write mode.\n");
		return 1;
	}

	if (fp == NULL)	{
		printf("File %s could not be opened.\n", name);
		return 1;
	} else {
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

