#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <vector>
#include <fstream>
#include <iostream>

int binary_search(long long arr[], long long key, long long begin, long long end) {
    if (begin > end)
        return -1;

    long long mid = begin + (end - begin) / 2;
    if (arr[mid] == key)
        return mid;
    else if (key < arr[mid])
        return binary_search(arr, key, begin, mid - 1);
    else
        return binary_search(arr, key, mid + 1, end);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <filename>\n";
        return EXIT_FAILURE;
    }

    long long index, global_index = -1;
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
        if (!file.is_open()) {
            std::cerr << "Error opening file\n";
            MPI_Finalize();
            return EXIT_FAILURE;
        }

        std::vector<long long> _arr;
        long long value;
        while (file >> value) {
            _arr.push_back(value);
        }

        long long* arr = new long long[_arr.size()];
        std::copy(_arr.begin(), _arr.end(), arr);

        file.close();

        double start = MPI_Wtime();
        long long key = 820767;
        size_t elements_per_process = _arr.size() / number_of_processes;
        size_t remaining_elements = _arr.size() % number_of_processes;
        bool found = false;

        // Binary search on master process
        long long start_index = 0;
        long long end_index = elements_per_process + (rank == 0 ? remaining_elements : 0) - 1;
        index = binary_search(arr, key, start_index, end_index);
        if (index != -1) {
            std::cout << "The key " << key << " is found at index " << index << std::endl;
            found = true;
        }

        // Pack and send information to other processes
        long long send_buffer[3];
        MPI_Request* requests = new MPI_Request[2 * number_of_processes];
        for (int i = 1; i < number_of_processes; i++) {
            start_index = i * elements_per_process + (i <= remaining_elements ? i : remaining_elements);
            end_index = start_index + elements_per_process + (i <= remaining_elements ? 0 : -1);
            send_buffer[0] = key;
            send_buffer[1] = start_index;
            send_buffer[2] = end_index - start_index + 1;
            MPI_Isend(send_buffer, 3, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &requests[(i - 1) * 2]);
            MPI_Isend(&arr[start_index], end_index - start_index + 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &requests[(i - 1) * 2 + 1]);
        }

        // Receive result from other processes and analyze the result
        MPI_Status recv_status;
        long long recv_buffer[3];
        for (int i = 1; i < number_of_processes; i++) {
            MPI_Recv(&recv_buffer, 3, MPI_LONG_LONG_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recv_status);
            index = recv_buffer[0];
            start_index = recv_buffer[1];
            long long num_of_elements_received = recv_buffer[2];
            if (index != -1) {
                global_index = start_index + index;
                printf("The key %lld is found at index %lld\n", key, global_index);
                found = true;
            }
        }

        if (!found) {
            printf("The key %lld is not found\n", key);
        }
        double end = MPI_Wtime();
        printf("Time taken to search: %.8lfms\n", (end - start) * 1000);

        delete[] requests;
        delete[] arr;
    } else {
        // Other processes: Receive info from master process, do binary search, and send the results back to master process
        MPI_Status recv_status;
        long long recv_buffer[3];
        MPI_Recv(&recv_buffer, 3, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &recv_status);
        long long key = recv_buffer[0];
        long long start_index = recv_buffer[1];
        long long num_of_elements_received = recv_buffer[2];
        long long* buffer = new long long[num_of_elements_received];
        MPI_Recv(buffer, num_of_elements_received, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &status);
        index = binary_search(buffer, key, 0, num_of_elements_received - 1);

        long long send_buffer[2];
        send_buffer[0] = index;
        send_buffer[1] = start_index;
        MPI_Send(send_buffer, 2, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);

        delete[] buffer;
    }

    MPI_Finalize();
    return 0;
}