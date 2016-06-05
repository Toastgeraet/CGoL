// Created by Kolesnikov S.S., Vecherkin B.I.

#define _XOPEN_SOURCE 600

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "mpi_gol_utils.h"
#include "mpi_gol_logic.h"

const int MASTER = 0;
const int STEP_1 = 127;
const int STEP_2 = 128;
const int STEP_3 = 129;
const int STEP_4 = 130;

int numberOfProcesses = 0;
int processId = 0;

// Правила игры
const int minToLive = 4;
const int maxToLive = 5;
const int minToResurrect = 5;
const int maxToResurrect = 5;

void evolveWorld(char *inputFile, int xlen, int ylen, int zlen);

// заменить позже плюсовой функцией
char *replace_str(char *str, char *orig, char *rep, int start);

// Максимальное колво поколений
int maxGenerations = 100;
int stencils;

clock_t begin, end;
double time_spent;
int numberOfWorlds = 0;
time_t start;

int main(int argc, char * argv[])
{
	char *inputFileArgument = argv[1];
	char *inputFile = NULL;
	int xlen = 0, ylen = 0, zlen = 0;
		
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
	MPI_Comm_rank(MPI_COMM_WORLD, &processId);

	parseArguments(argc, argv, inputFile, &xlen, &ylen, &zlen, &maxGenerations);

	// Проверяю входной файл на корректность
	struct stat s;
	if (stat(inputFileArgument, &s) == 0) {
		// тип файла (также флаги доступа) & директория
		if (s.st_mode & S_IFDIR) {
			// если директория
			DIR *dir;
			struct dirent *ent;
			if ((dir = opendir(inputFileArgument)) != NULL) {
                
				char *concatenationBuffer = (char*)malloc(100*sizeof(char));

				while ((ent = readdir(dir)) != NULL) {
					if (strcmp(ent->d_name, ".") == 0 
						|| strcmp(ent->d_name, "..") == 0) {
						continue;
					}										
					sprintf(concatenationBuffer, "inputfiles/%s", ent->d_name);
					if (processId == MASTER) {
						printf("ProcessId: %d Filename: %s\n", 
							processId, concatenationBuffer);
					}
					numberOfWorlds++;
					evolveWorld(concatenationBuffer, xlen, ylen, zlen);					
				}
				closedir(dir);
			}
			else {
				// ошибка открытия директории
				perror("");
				return EXIT_FAILURE;
			}
		} else if (s.st_mode & S_IFREG) { // тип файла - regular
			// если файл
			numberOfWorlds++;
			inputFile = inputFileArgument;
			printf("Evolving %s ...\n", inputFile);
			evolveWorld(inputFile, xlen, ylen, zlen);
		} else {
			//
		}
	} else {
		// ошибка
	}

	if (processId == MASTER) {
		printf("Finished evolving %d worlds for %d generations each.\n", 
			numberOfWorlds, maxGenerations);
	}

	MPI_Finalize();
}

void evolveWorld(char *inputFile, int xlen, int ylen, int zlen) {

	int *input;
	int population = 0;

	if (processId == MASTER) {
		input = createWorldFromTxt(inputFile, &population, xlen, ylen, zlen);
	}

	if (processId == MASTER) {
		printf("Initial population is %d\n", population);
	}

	int input_length_total = xlen * ylen * zlen;
	int z_layer_size = (xlen * ylen);

	int chunksize = (zlen / numberOfProcesses);
	int extra = (zlen % numberOfProcesses);

	int *scounts = (int*)malloc(numberOfProcesses*sizeof(int));
	for (int i = 0; i < numberOfProcesses; i++)
	{
		scounts[i] = chunksize;
		if (i < extra) scounts[i]++;
		scounts[i] *= z_layer_size;
	}

	int *displs = (int*)malloc(numberOfProcesses*sizeof(int));
	displs[0] = 0;
	for (int i = 1; i < numberOfProcesses; i++)	{
		displs[i] = scounts[i - 1] + displs[i - 1];
	}

	if (processId < extra) {
		++chunksize;
	}
	int printsize = chunksize;
	chunksize += 2;

	int *current = calloc(xlen * ylen * chunksize, sizeof(int));
	int *next = calloc(xlen * ylen * chunksize, sizeof(int));

	int *sendbuf = input;

	int *recvbuf = current + z_layer_size;

	MPI_Scatterv(sendbuf, scounts, displs, 
		MPI_INT, recvbuf, scounts[processId], 
		MPI_INT, MASTER, MPI_COMM_WORLD);

	if (processId == MASTER){
		free(sendbuf);
	}

    // TODO: исправить выходные файлы [+]
	char *output_name_buf = malloc((100)*sizeof(char));
	char *addtext = malloc((100)*sizeof(char));
	sprintf(output_name_buf, "outputfiles/%s_process_%d.txt", 
		inputFile, processId);
	replace_str(output_name_buf, "inputfiles/", "", 0);
    // [-]

	outputTxt(output_name_buf, "write", addtext, NULL, xlen, ylen, zlen);

	// Вычисляем поколения
	int count = xlen * ylen;

	/////
	int advancement_counter = 0, advancement_target = 25;
	char *advancement_print_buf = malloc((100)*sizeof(char));	
	if(processId == MASTER)	{
		printf("Evolving World %d : [_________________________]", 
			numberOfWorlds);
		fflush(stdout);
	}
	
	for (int generationX = 0; generationX < maxGenerations; generationX++) {
		if(processId == MASTER) {
			if(generationX > 0 
				&& (int)((float)generationX/(float)((float)maxGenerations/(float)advancement_target)) > advancement_counter) {

				advancement_counter++;
				printf("\rEvolving World %d : [", numberOfWorlds);

				for(int adv = 0; adv < advancement_counter - 1; adv++) {
					printf("=");
				}

				printf(">");

				for(int rest = 0; rest < advancement_target - advancement_counter; rest++) {
					printf("_");
				}

				printf("]");

				fflush(stdout);
			}
		}
		
		// в файл
		sprintf(addtext, "Generation: %d\nPopulation: %d\n",
			generationX, population);

		outputTxt(output_name_buf, "append", addtext, 
			current + count, xlen, ylen, printsize);

		//Обмен передней и заднего Z-слоz в соседних сечениях куба
		int *data = NULL;

		int nextProcessId = (processId + 1) % numberOfProcesses;
		int prevProcessId = (processId - 1);

		prevProcessId = prevProcessId < 0 ? 
			numberOfProcesses + prevProcessId : prevProcessId;

		// Свящь между процессами
		if (processId % 2 == 0)	{			
			data = &current[count*(chunksize - 2)];
			MPI_Ssend(data, count, MPI_INT, 
				nextProcessId, STEP_1, MPI_COMM_WORLD);
						
			data = current;
			MPI_Recv(data, count, MPI_INT, 
				prevProcessId, STEP_1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						
			data = current + count;
			MPI_Ssend(data, count, MPI_INT, 
				prevProcessId, STEP_1, MPI_COMM_WORLD);
						
			data = &current[count*(chunksize - 1)];
			MPI_Recv(data, count, MPI_INT, 
				nextProcessId, STEP_1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		} else {
			data = current;
			MPI_Recv(data, count, MPI_INT, 
				prevProcessId, STEP_1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
									
			data = &current[count*(chunksize - 2)];
			MPI_Ssend(data, count, MPI_INT, 
				nextProcessId, STEP_1, MPI_COMM_WORLD);
			
			data = &current[count*(chunksize - 1)];
			MPI_Recv(data, count, MPI_INT, 
				nextProcessId, STEP_1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			data = current + count;
			MPI_Ssend(data, count, MPI_INT, 
				prevProcessId, STEP_1, MPI_COMM_WORLD);
		}

		// Для каждого кубика вычисляем его часть
		population = 0;
		for (int k = 1; k < chunksize - 1; k++) {
			for (int j = 0; j < ylen; j++) {
				for (int i = 0; i < xlen; i++) {
					int index = offset(i, j, k, xlen, ylen);

					int c = 
						count_neighbours(current, xlen, ylen, zlen, i, j, k);

					if (current[index]) {
						if (next[index] = c >= minToLive && c <= maxToLive) {
							population++;
						}
					} else {
						if (next[index] = c >= minToResurrect 
							&& c <= maxToResurrect) {
							population++;
						}
					}
				}
			}
		}

		// Сбор населения
		int *recbuffer = (int*)malloc(sizeof(int) * numberOfProcesses);

		MPI_Gather(&population, 1, MPI_INT, 
			recbuffer, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
		population = 0;

		if (processId == MASTER) {
			for (int i = 0; i < numberOfProcesses; i++) {
				population += recbuffer[i];
			}
		}

		free(recbuffer);
		free(current);

		current = next;
		next = calloc(xlen * ylen * chunksize, sizeof(int));
	}
	
	if(processId == MASTER) {
		printf("\rEvolving World %d : [=========================] - done.\n", 
			numberOfWorlds);
	}

	free(advancement_print_buf);
	free(output_name_buf);
	free(addtext);
	return;
}

// зачем?
char *replace_str(char *str, char *orig, char *rep, int start) {
	static char temp[4096];
	static char buffer[4096];
	char *p;

	strcpy(temp, str + start);

	if (!(p = strstr(temp, orig))){
		return temp;
	}

	strncpy(buffer, temp, p - temp);
	buffer[p - temp] = '\0';

	sprintf(buffer + (p - temp), "%s%s", rep, p + strlen(orig));
	sprintf(str + start, "%s", buffer);

	return str;
}