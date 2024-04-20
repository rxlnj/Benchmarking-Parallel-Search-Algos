#include <iostream>
#include <fstream>
#include <pthread.h>
#include <vector>
using namespace std;

// Max size of array
#define max 30

// Max number of threads to create
#define thread_max 4

// Global variables
int a[max];
int key = 202;
int f = 0;
int current_thread = 0;

// Linear search function for each thread
void* ThreadSearch(void* args)
{
    int num = current_thread++;

    for (int i = num * (max / 4); i < ((num + 1) * (max / 4)); i++) {
        if (a[i] == key)
            f = 1;
    }
    return NULL;
}

// Function to read array from file
bool ReadArrayFromFile(const string& filename, vector<int>& arr) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Unable to open file " << filename << endl;
        return false;
    }

    int value;
    while (file >> value) {
        arr.push_back(value);
    }
    file.close();
    return true;
}

// Driver code
int main(int argc, char* argv[])
{
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    string filename = argv[1];
    vector<int> arr;

    if (!ReadArrayFromFile(filename, arr))
        return 1;

    // Copy vector to array
    for (int i = 0; i < max; ++i) {
        if (i < arr.size())
            a[i] = arr[i];
        else
            a[i] = 0; // Fill remaining elements with 0
    }

    pthread_t thread[thread_max];

    for (int i = 0; i < thread_max; i++) {
        pthread_create(&thread[i], NULL, ThreadSearch, (void*)NULL);
    }

    for (int i = 0; i < thread_max; i++) {
        pthread_join(thread[i], NULL);
    }

    if (f == 1)
        cout << "Key element found" << endl;
    else
        cout << "Key not present" << endl;

    return 0;
}
