ifeq ($(OS),Windows_NT)
    RM = Del /s /q
    MKDIR = mkdir
    EXE_EXT = .exe
else
    RM = rm -rf
    MKDIR = mkdir -p
    EXE_EXT =
endif

all:  BIN BINARYSEARCH LINEARSEARCH

BIN:
	$(MKDIR) bin

BINARYSEARCH: BIN
	g++ -pthread -o bin/binary_pthread$(EXE_EXT) BinarySearch/binarypthreads.cpp
	mpic++ -o bin/binary_mpi$(EXE_EXT) BinarySearch/binarympi.cpp

LINEARSEARCH: BIN
	g++ -pthread -o bin/linear_pthread$(EXE_EXT) LinearSearch/linearpthreads.cpp
	g++ -o bin/linear$(EXE_EXT) LinearSearch/linear.cpp
	mpic++ -o bin/linear_mpi$(EXE_EXT) LinearSearch/linearmpi.cpp

run:
ifeq ($(MPI), 1)
	mpirun -n ${n} bin/${file} Testcases/test_1.txt
else
	./bin/${file} Testcases/test_1.txt
endif

clean:
	$(RM) bin/
