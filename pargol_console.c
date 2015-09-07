#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "pargol_create.h"
#include "file_io.h"


char * getline(void) {
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

void usage(void)
{
	//printf("Usage:\n");
	//printf("<inputfile> -x <value> -y <value> -z <value> -g <gens to evolve>\n");
	exit(8);
}

void parseArguments(int argc, char * argv[], char * inFile, int * xlen, int * ylen, int * zlen, int * maxGens){
	//printf("Starting to parse arguments...\n");
	//printf("Program name: %s\n", argv[0]);
	//printf("Input file: %s\n", argv[1]);	
	
	//This should be moved to a different file. creating of directories got nothing to do with console...
	if (strcmp(argv[1], "-create") == 0)
	{		
		printf("Deleting inputfiles directory and contents.\n");
		rmrf("inputfiles");
		printf("Creating new inputfiles directory.\n");
		mkdir("inputfiles", 0700);

		int count = atoi(argv[2]);
		printf("Creating %d testworlds...\n", count);
		int x = atoi(argv[3]);
		int y = atoi(argv[4]);
		int z = atoi(argv[5]);
		float s;
		if (argc == 7)
		{
			s = atof(argv[6]); //s is the spawnrate - constraints: 0 < s <= 1
		}
		else
		{
			s = 0.25f; //default spawnrate
		}		

		for (int c = 0; c < count; c++)
		{
			createWorld(c, x, y, z, s);
		}
		printf("Finished creating %d Test Worlds.\n\n", count);
		
		printf("Deleting outputfiles directory and contents.\n");
		rmrf("outputfiles");
		printf("Creating new outputfiles directory.\n");
		mkdir("outputfiles", 0700);
		printf("All set to go. Run pargol as follows:\n");
		printf("mpiexec -np [numberOfProcesses] ./pargol inputfiles -x %d -y %d -z %d -g [generations per world (default = 100)]:\n", x, y, z);
		
		//close program
		exit(0);
	} //end create //non mpi

	//'Usual' startup parameters
	if (argc < 8)
	{
		//usage();
	}
	else
	{
		inFile = argv[1];
		while ((argc > 2) && (argv[2][0] == '-'))
		{
			char ** str = &argv[3];

			switch (argv[2][1])
			{
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
				usage();
			}

			argv += 2;
			argc -= 2;
		}
		//printf("Finished parsing of arguments.\n\n");
	}
}