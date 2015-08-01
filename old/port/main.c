#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include "pargol_console.h"
#include "pargol_logic.h"
#include "pargol_input.h"
#include "pargol_output.h"

//Variable Declarations
int xlen = 0, ylen = 0, zlen = 0, cellcount = 0;
int population = 0;
char * inputFile = NULL;

//Program Entry Point
int main(int argc, char * argv[])
{	
	parseArguments(argc, argv, inputFile, &xlen, &ylen, &zlen);

	//Dimensionen der Welt	
	cellcount = xlen*ylen*zlen;

	//Allocate memory for worlds
	int * current; // = malloc(cellcount, sizeof(int));
	int * next = calloc(cellcount, sizeof(int));

	//How long should it evolve
	int maxGenerationen = 100;

	//Regeln
	int minToLive = 4;
	int maxToLive = 5;
	int minToResurrect = 5;
	int maxToResurrect = 5;

	//Parse input Txt to create initial world
	current = createWorldFromTxt(inputFile, &population, xlen, ylen, zlen);
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
	getline();
}

