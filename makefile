pargol: pargol_main.c pargol_console.c pargol_logic.c pargol_input.c pargol_output.c pargol_create.c
	mpicc -o pargol pargol_main.c pargol_console.c pargol_logic.c pargol_input.c pargol_output.c pargol_create.c -g -std=c99

clean:
	rm -f pargol