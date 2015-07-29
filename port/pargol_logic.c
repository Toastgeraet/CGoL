#include "pargol_logic.h"

int count_neighbours(int * world, int xlen, int ylen, int zlen, int x, int y, int z)
{
	int neighbour_count = 0;
	int starti = -1;
	int startj = -1;
	int startk = -1;
	int endi = 1;
	int endj = 1;
	int endk = 1;
	int xo = 0;
	int yo = 0;
	int zo = 0;

	for (int k = startk; k <= endk; k++)
	{
		for (int j = startj; j <= endj; j++)
		{
			for (int i = starti; i <= endi; i++)
			{
				if (i == 0 && j == 0 && k == 0) //don't count the cell being analyzed itself
				{
					continue;
				}
				
				xo = (x + i) % xlen;
				yo = (y + j) % ylen;
				zo = (z + k) % zlen;

				int index = offset(xo, yo, zo, xlen, ylen);
				
				if (world[index]) //if the value of the cell is not '0' it is not false; we have a living cell and count it as a neighbour
				{
					neighbour_count++;					
				}
			}
		}
	}

	return neighbour_count;
}

int offset(int x, int y, int z, int xlen, int ylen) 
{
	return (z * xlen * ylen) + (y * xlen) + x; 
}

int count_neighbours1(int * world, int xlen, int ylen, int zlen, int x, int y, int z)
{
	int count = 0;
	int starti = -1;
	int startj = -1;
	int startk = -1;
	int endi = 1;
	int endj = 1;
	int endk = 1;
	int xo = 0;
	int yo = 0;
	int zo = 0;

	for (int k = startk; k <= endk; k++)
	{
		for (int j = startj; j <= endj; j++)
		{
			for (int i = starti; i <= endi; i++)
			{
				if (x + i < 0)
					xo = xlen - 1;
				else if (x + i >= xlen)
					xo = 0;
				else xo = x + i;

				if (y + j < 0)
					yo = ylen - 1;
				else if (y + j >= ylen)
					yo = 0;
				else yo = y + j;

				if (z + k < 0)
					zo = zlen - 1;
				else if (z + k >= zlen)
					zo = 0;
				else zo = z + k;

				int index = offset(xo, yo, zo, xlen, ylen);
				if (i == 0 && j == 0 && k == 0) continue;
				if (world[index]) count++;
			}
		}
	}

	return count;
}