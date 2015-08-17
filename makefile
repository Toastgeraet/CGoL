pargol: pargol_main.c pargol_console.c pargol_logic.c pargol_input.c pargol_output.c pargol_create.c file_io.c
	mpicc -o pargol pargol_main.c pargol_console.c pargol_logic.c pargol_input.c pargol_output.c pargol_create.c file_io.c -g -std=c99

clean:
	rm -rf pargol
	rm -rf ddt.out
	rm -rf input*
	rm -rf output*
