#define _CRT_SECURE_NO_WARNINGS

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include "pargol_console.h"
#include "pargol_logic.h"
#include "pargol_input.h"
#include "pargol_output.h"

//MPI Variables (and Constants)
const int MASTER = 0;
int numberOfProcesses = 0, processId = 0;

//Program Entry Point
int main(int argc, char * argv[])
{	
	//Initialize MPI
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
	MPI_Comm_rank(MPI_COMM_WORLD, &processId);

	//Parse commandline arguments | pargol_console.c	
	char * inputFile = NULL;
	int xlen = 0, ylen = 0, zlen = 0;	
	parseArguments(argc, argv, inputFile, &xlen, &ylen, &zlen);

	//Parse inputworld into memory | pargol_input.c
	int * input;
	if (world_rank == MASTER){
		input = createWorldFromTxt(inputFile, &population, xlen, ylen, zlen);
	}

	//Calculate distribution values for parallel processing
	//each process will count the neighbours for a given amount of consecutive zlayers
	int input_length_total = xlen * ylen * zlen;
	int zlayer_count_per_process = (zlen / world_size);
	int extra_zlayers = (zlen % world_size);		
	if (processId < extra_zlayers)
	{
		++zlayer_count_per_process;
	}
	zlayer_count_per_process = zlayer_count_per_process + 2; //Plus 2 because it needs space for its neighbouring block's layers
	int processWorldSize = xlen * ylen * zlayer_count_per_process;

	int * current = calloc(zlayer_count_per_process, sizeof(int));
	int * next = calloc(zlayer_count_per_process, sizeof(int));

	


	//How long should it evolve
	int maxGenerationen = 100;

	//Regeln
	int minToLive = 4;
	int maxToLive = 5;
	int minToResurrect = 5;
	int maxToResurrect = 5;
	
	//TODO >> MASTER ONLY
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
		next = calloc(gol_cells_total, sizeof(int));
	}
	//free(output_name_buf);
	printf("Finished evolving for %d generations.\n\nPress any key to close this window.", maxGenerationen);
	getline();
}

