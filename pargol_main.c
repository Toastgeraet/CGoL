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


//Program Entry Point
int main(int argc, char * argv[])
{	
	//Initialize MPI
	int numberOfProcesses = 0, processId = 0;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
	MPI_Comm_rank(MPI_COMM_WORLD, &processId);

	//Parse commandline arguments | pargol_console.c	
	char * inputFile = argv[1]; //this should be done in parsearguments - confused by pointer logic
	int xlen = 0, ylen = 0, zlen = 0;	
	parseArguments(argc, argv, inputFile, &xlen, &ylen, &zlen);
	
	//Parse inputworld into memory | pargol_input.c
	int * input;
	int population = 0;

	if (processId == MASTER){
		input = createWorldFromTxt(inputFile, &population, xlen, ylen, zlen);
	}

	if (processId == MASTER){
		printf("Initial population is %d\n", population);
	}	

	//Calculate distribution values for parallel processing
	//each process will count the neighbours for a given amount of consecutive zlayers
	int input_length_total = xlen * ylen * zlen;
	
	int chunksize = (zlen / numberOfProcesses);
	int extra = (zlen % numberOfProcesses);

	//Chunksize information for scatterv; Scattering of initial generation
	int * scounts = (int*)malloc(numberOfProcesses*sizeof(int));
	for (int i = 0; i < numberOfProcesses; i++)
	{
		scounts[i] = chunksize;
		if (i < extra) scounts[i]++;
	}

	//Offset information for scatterv; Scattering of initial generation
	int * displs = (int*)malloc(numberOfProcesses*sizeof(int));
	displs[0] = 0;
	for (int i = 1; i < numberOfProcesses; i++)
	{
		displs[i] = scounts[i] + displs[i - 1];
	}
		
	//Calculating for each process how many layers it must be able to hold (2 for neighbouring slices)
	if (processId < extra)
	{
		++chunksize;
	}
	chunksize += 2;	

	//int processWorldSize = xlen * ylen * chunksize;
	int * current = calloc(xlen * ylen * chunksize, sizeof(int));
	int * next = calloc(xlen * ylen * chunksize, sizeof(int));

	int * sendbuf = input;
	int z_layer_size = (xlen * ylen);
	int * recvbuf = current + z_layer_size;
	
	MPI_Scatterv(sendbuf, scounts, displs, MPI_INT, recvbuf, scounts[processId], MPI_INT, MASTER, MPI_COMM_WORLD);

	//release parsed world into oblivion
	if (processId == MASTER){
		free(sendbuf);
	}	

	//How long should it evolve
	int maxGenerationen = 100;

	//Rules of Life
	int minToLive = 4;
	int maxToLive = 5;
	int minToResurrect = 5;
	int maxToResurrect = 5;
	
	//TODO >> MASTER ONLY >> Output initial configuration and popluation
	//char * output_name_buf = malloc((100)*sizeof(char)); // max filename length
	//char * addtext = malloc((100)*sizeof(char));
	//sprintf(addtext, "Create this file... Date or something - maybe rules here...\n");
	//outputTXT("output_combined.txt", "write", addtext, NULL, xlen, ylen, zlen);

	//Generationen berechnen
	for (int generationX = 0; generationX < maxGenerationen; generationX++)
	{		
		//sprintf(addtext, "Generation: %d\nPopulation: %d\n", generationX, population);
		//outputTXT("output_combined.txt", "append", addtext, current, xlen, ylen, zlen);
		
		//Exchange of front and back z-layers of neigbouhring slices of the cube
		int * data = NULL;
		int count = xlen * ylen;
		int nextProcessId = (processId + 1) % numberOfProcesses;
		int prevProcessId = (processId - 1);
		prevProcessId = prevProcessId < 0 ? numberOfProcesses + prevProcessId : prevProcessId;
		
		if (processId % 2 == 0) //every other process does this
		{ 
			data = &current[count*(chunksize - 2)];
			MPI_Send(data, count, MPI_INT, nextProcessId, "step 1", MPI_COMM_WORLD);
			
			data = current;
			MPI_Recv(data, count, MPI_INT, prevProcessId, "step 2", MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			data = current;
			MPI_Send(data, count, MPI_INT, prevProcessId, "step 3", MPI_COMM_WORLD);

			data = &current[count*(chunksize - 2)];
			MPI_Recv(data, count, MPI_INT, nextProcessId, "step 4", MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else //the rest does the opposite
		{ 
			data = current;
			MPI_Recv(data, count, MPI_INT, prevProcessId, "step 1", MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			data = &current[count*(chunksize - 2)];
			MPI_Send(data, count, MPI_INT, nextProcessId, "step 2", MPI_COMM_WORLD);
			
			data = &current[count*(chunksize - 2)];
			MPI_Recv(data, count, MPI_INT, nextProcessId, "step 3", MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			data = current;
			MPI_Send(data, count, MPI_INT, prevProcessId, "step 4", MPI_COMM_WORLD);
		}
			
		//Each cube calculates its portion
		population = 0;
		for (int k = 0; k < chunksize; k++)
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

		//GATHER population
		int * recbuffer = (int*)malloc(sizeof(int) * numberOfProcesses);
		MPI_Gather(&population, 1, MPI_INT, recbuffer, 1, MPI_INT, MASTER, MPI_COMM_WORLD);		
		population = 0;
		if (processId == MASTER){
			for (int i = 0; i < numberOfProcesses; i++){
				population += recbuffer[i];
				printf("task %d sum is %d\n", i, recbuffer[i]);
			}
			printf("Final sum= %d \n", population);
		}
		free(recbuffer);

		free(current);
		current = next;
		next = calloc(xlen * ylen * chunksize, sizeof(int));
	}
	
	MPI_Finalize();

	//free(output_name_buf);
	printf("Finished evolving for %d generations.\n\nPress any key to close this window.", maxGenerationen);
	getline();
}

