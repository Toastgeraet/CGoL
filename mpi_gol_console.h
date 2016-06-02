#ifndef MPI_GOL_CONSOLE_H_INCLUDED
#define MPI_GOL_CONSOLE_H_INCLUDED

char * getline(void);
void parseArguments(int argc, char * argv[], char * inFile, int * xlen, int * ylen, int * zlen, int * maxGens);
void usage(void);

#endif