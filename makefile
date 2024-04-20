all: BIN BINARYSEARCH LINEARSEARCH

BIN:
	mkdir -p bin/

BINARYSEARCH:
	g++ -pthread -o bin/binary_pthread BinarySearch/binarypthreads.cpp

LINEARSEARCH:
	g++ -pthread -o bin/linear_pthread LinearSearch/linearpthreads.cpp
	g++ -o bin/linear_single_thread LinearSearch/linearsinglethread.cpp

# MPI:
# 	mpicc -o bin/linear_mpi linear_mpi.cpp

run:
	./bin/${file} Testcases/test_1.txt

clean:
	rm -rf bin/



