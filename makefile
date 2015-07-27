main: main.c
	mpicc -o main main.c -g -std=c99

clean:
	rm -f main
