all: BIN BINARYSEARCH LINEARSEARCH

BIN:
	mkdir bin/

BINARYSEARCH: BIN
	g++ -pthread -o bin/binary_pthread BinarySearch/binarypthreads.cpp
	mpic++ -o bin/binary_mpi BinarySearch/binarympi.cpp

LINEARSEARCH: BIN
	g++ -pthread -o bin/linear_pthread LinearSearch/linearpthreads.cpp
	g++ -o bin/linear_single_thread LinearSearch/linearsinglethread.cpp


run:
ifeq ($(MPI), 1)
	mpirun -n ${n} bin/${file} Testcases/test_1.txt
else
	./bin/${file} Testcases/test_1.txt
endif

clean:
	rm -rf bin/



