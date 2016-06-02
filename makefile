mpi_gol: mpi_gol_main.c mpi_gol_console.c mpi_gol_logic.c mpi_gol_input.c mpi_gol_output.c mpi_gol_create.c file_io.c
	mpicc -o mpi_gol mpi_gol_main.c mpi_gol_console.c mpi_gol_logic.c mpi_gol_input.c mpi_gol_output.c mpi_gol_create.c file_io.c -g -std=c99
	

clean:
	rm -rf mpi_gol
	rm -rf outputfiles/*
	
