#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>
#include <vector>
#include <fstream>

int linear_search(int arr[], int key, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == key) {
            return i;
        }
    }
    return -1;
}

int main(int argc, char* argv[]) {

    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " <filename>\n";
        return EXIT_FAILURE;
    }



    int number_of_processes, rank;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

    if (rank == 0) {
        //std::vector<int> arr = { 1, 5, 7, 10, 12, 14, 15, 18, 20, 22, 25, 27, 30, 64, 110, 220 };
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

        int number_of_elements = _arr.size();
        int key = 14;
        int chunk_size = number_of_elements / number_of_processes;
        bool found = false;

        int local_index = linear_search(arr, key, chunk_size);
        if (local_index != -1) {
            std::cout << "The key " << key << " is found at index " << local_index << std::endl;
            found = true;
        }

        for (int i = 1; i < number_of_processes; i++) {
            int start_index = i * chunk_size;
            int chunk_size_remaining = (i == number_of_processes - 1) ? number_of_elements - start_index : chunk_size;
            MPI_Send(&key, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&chunk_size_remaining, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&start_index, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&arr[start_index], chunk_size_remaining, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        for (int i = 1; i < number_of_processes; i++) {
            int local_index, start_index;
            MPI_Recv(&local_index, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&start_index, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
            if (local_index != -1) {
                int global_index = local_index + start_index;
                std::cout << "The key " << key << " is found at index " << global_index << std::endl;
                found = true;
            }
        }

        if (!found) {
            std::cout << "The key " << key << " is not found" << std::endl;
        }
    } else {
        int key, chunk_size, local_index, start_index;
        MPI_Recv(&key, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&chunk_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&start_index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        int count;
        MPI_Get_count(&status, MPI_INT, &count);
        std::vector<int> chunk(count);

        MPI_Recv(chunk.data(), chunk_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        local_index = linear_search(chunk.data(), key, chunk_size);
        MPI_Send(&local_index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&start_index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}