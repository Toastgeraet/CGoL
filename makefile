mpi_gol: mpi_gol_main.c mpi_gol_utils.c mpi_gol_logic.c
	mpicc -o mpi_gol mpi_gol_main.c mpi_gol_utils.c mpi_gol_logic.c -g -std=c99
	

clean:
	rm -rf mpi_gol
	rm -rf outputfiles/*
	
