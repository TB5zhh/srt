NUM_PROC=2

phase_1:
	g++ src/phase_1.cpp src/my_mpi.cpp -o bin/phase_1 -lrt -lpthread
	bin/phase_1 ${NUM_PROC}

clean:
	rm -rf bin/* obj/*