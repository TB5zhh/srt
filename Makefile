NUM_PROC=10

phase_1:
	gcc src/phase_1.c src/my_mpi.c -o bin/phase_1 -lrt
	bin/phase_1 ${NUM_PROC}

clean:
	rm -rf bin/* obj/*