#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
using namespace std;

// Function to perform binary search
int BinarySearch(const vector<int>& arr, int key) {
    int left = 0;
    int right = arr.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (arr[mid] == key)
            return 1;

        if (arr[mid] < key)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return 0;
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

    int key = -1; // Key to search

    using namespace std::chrono;

    auto start_time = high_resolution_clock::now(); // Start time before search

    int found = BinarySearch(arr, key);

    // if (found == 1)
    //     cout << "Key element found" << endl;
    // else
    //     cout << "Key not present" << endl;

    auto end_time = high_resolution_clock::now();    // End time after search

    // Calculate elapsed time in milliseconds
    duration<double, milli> elapsed_time = end_time - start_time;

    cout << "Search time: " << elapsed_time.count() << " milliseconds" << endl;

    return 0;
}
