#ifndef MPI_GOL_LOGIC_H_INCLUDED
#define MPI_GOL_LOGIC_H_INCLUDED

int offset(int x, int y, int z, int xlen, int ylen);

int count_neighbours(int * world, 
	int xlen, int ylen, int zlen, 
	int x, int y, int z);

#endif