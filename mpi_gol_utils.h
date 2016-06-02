// Created by Kolesnikov S.S., Vecherkin B.I.

#ifndef MPI_GOL_UTILS_H_INCLUDED
#define MPI_GOL_UTILS_H_INCLUDED

char * getline();

void parseArguments(int argc, char * argv[], char * inFile, 
	int * xlen, int * ylen, int * zlen, int * maxGens);

int * createWorldFromTxt(char * name, int * population, 
	int xlen, int ylen, int zlen);

int outputTXT(char * name, char * mode, char * text, int * world, 
	int xlen, int ylen, int zlen);


#endif