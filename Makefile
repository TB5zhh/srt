NUM_P=2
DATASIZE=4096*1024
TESTTIME=32

compile:
	mpicc -DDATA_SIZE=$(DATASIZE) -DTEST_TIME=$(TESTTIME) src/mpi_example.c src/difftimeval.c -o bin/mpi_example
run: compile
	@echo ==============================
	@echo Number of processes is $(NUM_P)
	@echo ==============================
	mpiexec -n $(NUM_P) bin/mpi_example

compile_local:
	gcc -DDATA_SIZE=$(DATASIZE) -DTEST_TIME=$(TESTTIME) src/local.c src/difftimeval.c -o bin/local

run_local: compile_local
	bin/local

clean:
	rm -rf bin/*