pargol: pargol_main.c pargol_console.c pargol_logic.c pargol_input.c pargol_output.c pargol_create.c file_io.c
	mpicc -o pargol pargol_main.c pargol_console.c pargol_logic.c pargol_input.c pargol_output.c pargol_create.c file_io.c -g -std=c99
	
singol: singol_main.c pargol_console.c pargol_logic.c pargol_input.c pargol_output.c pargol_create.c file_io.c
	mpicc -o singol singol_main.c pargol_console.c pargol_logic.c pargol_input.c pargol_output.c pargol_create.c file_io.c -g -std=c99
	
vampol: pargol_main.c pargol_console.c pargol_logic.c pargol_input.c pargol_output.c pargol_create.c file_io.c
	vtcc -vt:cc mpicc -o vampol pargol_main.c pargol_console.c pargol_logic.c pargol_input.c pargol_output.c pargol_create.c file_io.c -g -std=c99

clean:
	rm -rf pargol
	rm -rf singol
	rm -rf ddt.out
	rm -rf inputfiles*
	rm -rf outputfiles*
	rm -rf *.z
	rm -rf vampol*
	