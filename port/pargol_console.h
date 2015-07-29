#ifndef PARGOL_CONSOLE_H_INCLUDED
#define PARGOL_CONSOLE_H_INCLUDED

char * getline(void);
void parseArguments(int argc, char * argv[], char * inFile, int * xlen, int * ylen, int * zlen);
void usage(void);

#endif