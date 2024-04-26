#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>
#include <vector>
#include <fstream>
#include <iostream>

int binary_search(int arr[], int key, int begin, int end) {
    if (begin <= end) {
        int mid = (begin + end) / 2;
        if (arr[mid] == key)
            return mid;
        else if (key < arr[mid])
            return binary_search(arr, key, begin, mid - 1);
        else
            return binary_search(arr, key, mid + 1, end);
    }
    return -1;
}

int main(int argc, char* argv[]) {

    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " <filename>\n";
        return EXIT_FAILURE;
    }

    int i, index, global_index;
    int rank, number_of_processes;
    MPI_Status status;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

    if (rank == 0) {
        // The master process
        // Load the array from file
        std::string filename = argv[1];
        std::ifstream file(filename);
        if(!file.is_open()){
            std::cerr << "Error opening file\n";
            MPI_Finalize();
            return EXIT_FAILURE;
        }

        std::vector<int> _arr;
        int value;
        while(file >> value){
            _arr.push_back(value);
        }

        int arr[_arr.size()];
        std::copy(_arr.begin(), _arr.end(), arr);

        file.close();

        long long key = 820767;
        int elements_per_process = _arr.size() / number_of_processes;
        bool found = 0;

        //Binary search on master process
        index = binary_search(arr, key, 0, elements_per_process - 1);
        if (index != -1) {
            std::cout << "The key " << key << " is found at  index " << index << std::endl;
            found = true;
        }

        //Send key, elements per process, start index, and arr from start index to other processes
        for (i = 1; i < number_of_processes; i++) {
            int start_index = i * elements_per_process;
            MPI_Send(&key, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&start_index, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&arr[start_index], elements_per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        //Receive result from other proecesses and analyse the result
        for (i = 1; i < number_of_processes; i++) {
            int start_index;
            MPI_Recv(&index, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&start_index, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
            if (index != -1) {
                global_index = start_index + index;
                printf("The key %d is found at index %d\n", key, global_index);
                found = 1;
            }
        }

        if (!found) {
            printf("The key %d is not found\n", key);
        }

    } else {
        //Other processes: Receives info from master process, does binary search and sends the results back to master process
        int key, num_of_elements_received, start_index;
        MPI_Recv(&key, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&num_of_elements_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&start_index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        int buffer[num_of_elements_received];
        MPI_Recv(&buffer, num_of_elements_received, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        index = binary_search(buffer, key, 0, num_of_elements_received - 1);
        MPI_Send(&index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&start_index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}