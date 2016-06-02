#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//spawnrate must be greater than 0 and less than or equal 1
int createWorld(int number, int xlen, int ylen, int zlen, float spawnrate) {
	
	char * name = malloc((100)*sizeof(char));
	sprintf(name, "inputfiles/world%d.txt", number);
	
	int spawnrate_converted = (int)(1 / spawnrate);
	printf("Randomly spawning approximately every %dth cell.\n", 
		spawnrate_converted);

	FILE *fp = fopen(name, "w");
	if (fp == NULL)	{
		printf("Datei %s konnte nicht geoeffnet werden.\n", name);
		return 1;
	} else {
		fputc(10, fp);

		for (int k = 0; k < zlen; k++) {
			for (int j = 0; j < ylen; j++) {
				for (int i = 0; i < xlen; i++) {
					int random = rand();
					if (random % spawnrate_converted == 0) {
						fputc('1', fp);
					} else {
						fputc('0', fp);
					}
				}
				fputc(10, fp);
			}
			fputc(10, fp);
		}
		fclose(fp);
	}

	free(name);
	return 0;
}