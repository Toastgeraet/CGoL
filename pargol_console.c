#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pargol_create.h"

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
	printf("Usage:\n");
	printf("<inputfile> -x <value> -y <value> -z <value>\n");
	exit(8);
}

void parseArguments(int argc, char * argv[], char * inFile, int * xlen, int * ylen, int * zlen){
	printf("Starting to parse arguments...\n");
	printf("Program name: %s\n", argv[0]);
	printf("Input file: %s\n", argv[1]);
	//inFile = argv[1];
		
	if (strcmp(argv[1], "-create") == 0)
	{		
		rmrf("inputfiles");
		mkdir("inputfiles");

		int count = atoi(argv[2]);
		printf("Creating %d testworlds...", count);
		int x = atoi(argv[3]);
		int y = atoi(argv[4]);
		int z = atoi(argv[5]);

		for (int c = 0; c < count; c++)
		{
			createWorld(c, x, y, z);
		}

		printf("Task finished.");
		return;
	}

	if (argc < 8)
	{
		usage();
	}
	else
	{
		while ((argc > 2) && (argv[2][0] == '-'))
		{
			char ** str = &argv[3];

			switch (argv[2][1])
			{
			case 'x':
				printf("x = %s\n", *str);
				*xlen = (int)strtol(*str, (char **)NULL, 10);
				break;

			case 'y':
				printf("y = %s\n", *str);
				*ylen = (int)strtol(*str, (char **)NULL, 10);
				break;

			case 'z':
				printf("z = %s\n", *str);
				*zlen = (int)strtol(*str, (char **)NULL, 10);
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
}