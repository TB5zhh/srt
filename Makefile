NUM_P=5

compile:
	mpicc src/mpi_example.c -o bin/mpi_example
run:
	echo $(NUM_P)
	mpirun -n $(NUM_P) bin/mpi_example
clean:
	rm -rf bin/*