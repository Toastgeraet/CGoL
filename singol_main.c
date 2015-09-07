#define _CRT_SECURE_NO_WARNINGS
#define _XOPEN_SOURCE 600

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include "pargol_console.h"
#include "pargol_logic.h"
#include "pargol_input.h"
#include "pargol_output.h"

//MPI Variables and Constants
const int MASTER = 0;
const int STEP_1 = 127;
const int STEP_2 = 128;
const int STEP_3 = 129;
const int STEP_4 = 130;

int numberOfProcesses = 0, processId = 0;

//Rules of Life
const int minToLive = 4;
const int maxToLive = 5;
const int minToResurrect = 5;
const int maxToResurrect = 5;

void evolveWorld(char * inputFile, int xlen, int ylen, int zlen);
char *replace_str(char *str, char *orig, char *rep, int start);

int maxGenerationen = 100;
int stencils;

clock_t begin, end;
double time_spent;
int numberOfWorlds = 0;
time_t start;

//Program Entry Point
int main(int argc, char * argv[])
{
	//Parse commandline arguments | pargol_console.c	
	char * inputFileArgument = argv[1]; //this should be done in parsearguments - confused by pointer logic
	char * inputFile = NULL;
	int xlen = 0, ylen = 0, zlen = 0;
		
	//Initialize MPI	
	//MPI_Init(NULL, NULL);
	//MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
	//MPI_Comm_rank(MPI_COMM_WORLD, &processId);

	//if (processId == MASTER)
	{
		begin = clock();
		start = time(NULL);
	}
		
	//parsing of inputfile not working in parsearguments? -seg fault??? done in main for now
	parseArguments(argc, argv, inputFile, &xlen, &ylen, &zlen, &maxGenerationen);
	/*printf("MAIN DEBUG AFTER parseArguments:\n");
	printf("inputFileArgument: %s\n", inputFileArgument);
	printf("inputFile: %s\n", inputFile);*/

	//Check if inputfile is a world or an inputfiles directory
	struct stat s;
	if (stat(inputFileArgument, &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
		{
			//it's a directory
			DIR *dir;
			struct dirent *ent;
			if ((dir = opendir(inputFileArgument)) != NULL) { //unneccessary double check if it is a dir?! different apis - clean up later
				
				char * concatenationBuffer = (char*)malloc(100*sizeof(char));

				/* print all the files and directories within directory */
				while ((ent = readdir(dir)) != NULL) {
					if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
					{
						continue;
					}										
					sprintf(concatenationBuffer, "inputfiles/%s", ent->d_name);
					if (processId == MASTER){
						printf("ProcessId: %d Filename: %s\n", processId, concatenationBuffer);
					}
					numberOfWorlds++;
					evolveWorld(concatenationBuffer, xlen, ylen, zlen);					
				}
				closedir(dir);
			}
			else {
				/* could not open directory */
				perror("");
				return EXIT_FAILURE;
			}
		}
		else if (s.st_mode & S_IFREG)
		{
			//it's a file
			numberOfWorlds++;
			inputFile = inputFileArgument;
			printf("Evolving %s ...\n", inputFile);
			evolveWorld(inputFile, xlen, ylen, zlen);
		}
		else
		{
			//something else
		}
	}
	else
	{
		//error
	}

	//if (processId == MASTER){
		
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("Finished evolving %d worlds for %d generations each.\n", numberOfWorlds, maxGenerationen);
		printf("Performed a total of %d stencil operations.\n", xlen*ylen*zlen*maxGenerationen*numberOfWorlds);
		printf("Program execution took %.2f seconds.\n", time_spent);
		printf("That is %f stencil operations per second.\n", (xlen*ylen*zlen*maxGenerationen*numberOfWorlds) / time_spent);				
		printf("Wallclock execution time: %.2f\n", (double)((time(NULL) - start)));
		//printf("Press any key to close this window.\n");

		//getline();
	//}

	//MPI_Finalize();
}

void evolveWorld(char * inputFile, int xlen, int ylen, int zlen)
{
	//Parse inputworld into memory | pargol_input.c
	int * input;
	int population = 0;
	input = createWorldFromTxt(inputFile, &population, xlen, ylen, zlen);
	printf("Initial population is %d\n", population);

	//Calculate distribution values for parallel processing
	//each process will count the neighbours for a given amount of consecutive zlayers
	int input_length_total = xlen * ylen * zlen;
	int z_layer_size = (xlen * ylen);
	
	int * current = input;
	int * next = calloc(xlen * ylen * zlen, sizeof(int));
	
	//TODO >> MASTER ONLY >> Output initial configuration and popluation
	char * output_name_buf = malloc((100)*sizeof(char)); // max filename length
	char * addtext = malloc((100)*sizeof(char));
	sprintf(addtext, "Create this file... Date or something - maybe rules here...\n");
	sprintf(output_name_buf, "outputfiles/%s_single.txt", inputFile);
	replace_str(output_name_buf, "inputfiles/", "", 0);
	replace_str(output_name_buf, ".txt", "", 0);

	outputTXT(output_name_buf, "write", addtext, NULL, xlen, ylen, zlen);

	//Generationen berechnen
	int count = xlen * ylen;

	//for status output
	int advancement_counter = 0, advancement_target = 25;
	char * advancement_print_buf = malloc((100)*sizeof(char));	
		printf("Evolving World %d : [_________________________]", numberOfWorlds); //change to current world variable
		fflush(stdout);

	
	for (int generationX = 0; generationX < maxGenerationen; generationX++)
	{
			if(generationX > 0 && (int)((float)generationX/(float)((float)maxGenerationen/(float)advancement_target)) > advancement_counter)
			{
				advancement_counter++;
				printf("\rEvolving World %d : [", numberOfWorlds); //change to current world variable
				for(int adv = 0; adv < advancement_counter-1; adv++){
					printf("=");
				}
				printf(">");
				for(int rest = 0; rest < advancement_target - advancement_counter; rest++){
					printf("_");
				}
				printf("]");
				fflush(stdout);
			}
		
		//this will be printed into the outputfiles
		sprintf(addtext, "Generation: %d\nPopulation: %d\n", generationX, population);
		outputTXT(output_name_buf, "append", addtext, current + count, xlen, ylen, zlen);
				
		population = 0;
		for (int k = 1; k < zlen - 1; k++)
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
		next = calloc(xlen * ylen * zlen, sizeof(int));
	}
	
		printf("\rEvolving World %d : [=========================] - done.\n", numberOfWorlds); //change to current world variable


	free(advancement_print_buf);
	free(output_name_buf);
	free(addtext);
	return;
}

//Credit goes to Tudor from stackoverflow who edited this
//stackoverflow.com/questions/8137244/best-way-to-replace-a-part-of-string-by-another-in-c
char *replace_str(char *str, char *orig, char *rep, int start)
{
	static char temp[4096];
	static char buffer[4096];
	char *p;

	strcpy(temp, str + start);

	if (!(p = strstr(temp, orig)))  // Is 'orig' even in 'temp'?
		return temp;

	strncpy(buffer, temp, p - temp); // Copy characters from 'temp' start to 'orig' str
	buffer[p - temp] = '\0';

	sprintf(buffer + (p - temp), "%s%s", rep, p + strlen(orig));
	sprintf(str + start, "%s", buffer);

	return str;
}