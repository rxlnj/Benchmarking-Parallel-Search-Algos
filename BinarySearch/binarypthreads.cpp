// CPP Program to perform binary search using pthreads

#include <iostream>
#include <fstream>
#include <sstream>
#include <pthread.h>
#include <vector>
#include <string>
#include <chrono>
using namespace std;

#define MAX_THREAD 8

bool found = false;
int part = 0;
int key = 820767; // Key to search

struct ThreadArgs {
    int start;
    int end;
    vector<int>* arr; // Pointer to the array to be searched
};

void* binary_search(void* arg)
{
    ThreadArgs* args = (ThreadArgs*)arg;

    int thread_part = part++;
    int low = args->start;
    int high = args->end;


    vector<int>& arr = *(args->arr); // Reference to the array to be searched

    while (low <= high && !found) {
        int mid = low + (high - low) / 2;

        if (arr[mid] == key) {
            found = true;
            break;
        } else if (arr[mid] < key)
            low = mid + 1;
        else
            high = mid - 1;
    }

    return NULL;
}

int main(int argc, char* argv[])
{
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

    pthread_t threads[MAX_THREAD];
    ThreadArgs args[MAX_THREAD];
    
    using namespace std::chrono;
    auto start_time = high_resolution_clock::now(); // Start time before search

    int chunk_size = arr.size() / MAX_THREAD;
    for (int i = 0; i < MAX_THREAD; i++) {
        args[i].start = i * chunk_size;
        args[i].end = (i == MAX_THREAD - 1) ? arr.size() - 1 : (i + 1) * chunk_size - 1;
        args[i].arr = &arr; // Passing a pointer to the array to the thread
        pthread_create(&threads[i], NULL, binary_search, (void*)&args[i]);
    }

    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }

    if (found)
        cout << key << " found in array" << endl;
    else
        cout << key << " not found in array" << endl;
	
	auto end_time = high_resolution_clock::now();    // End time after search

    // Calculate elapsed time in milliseconds
    duration<double, milli> elapsed_time = end_time - start_time;

    cout << "Search time: " << elapsed_time.count() << " milliseconds" << endl;
    
    return 0;
}

