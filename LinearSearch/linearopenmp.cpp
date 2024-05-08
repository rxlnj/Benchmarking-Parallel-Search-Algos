#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <omp.h>
using namespace std;

// Function to perform linear search
int LinearSearch(const vector<int>& arr, int key, int start, int end) {
    int result = -1;
    for (int i = start; i < end; ++i) {
        if (arr[i] == key) {
            result = i; // Key found
            break;
        }
    }
    return result; // Key not found
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    string filename = argv[1];

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Unable to open file " << filename << endl;
        return 1;
    }

    vector<int> arr;
    int value;
    while (file >> value) {
        arr.push_back(value);
    }
    file.close();

    int key = 820767; // Key to search
    int found_index = -1;
    int found_thread = -1;


    auto start_time = omp_get_wtime(); // Start time before search
    #pragma omp parallel num_threads(8)
    {
        int thread_id = omp_get_thread_num();
        int chunk_size = arr.size() / omp_get_num_threads();
        int start = thread_id * chunk_size;
        int end = (thread_id == omp_get_num_threads() - 1) ? arr.size() : (thread_id + 1) * chunk_size;

        int result = LinearSearch(arr, key, start, end);

        #pragma omp critical
        {
            if (result != -1) {
                found_index = result;
                found_thread = thread_id;
            }
        }
    }

    if (found_index != -1) {
        cout << "Key element found by thread " << found_thread << " at index " << found_index << endl;
    } else {
        cout << "Key element not found" << endl;
    }

    auto end_time = omp_get_wtime();   // End time after search

    // Calculate elapsed time in milliseconds
    double elapsed_time = end_time - start_time;

    cout << "Search time: " << elapsed_time * 1000 << " milliseconds" << endl;

    return 0;
}
