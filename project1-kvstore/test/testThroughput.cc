/**
 * Author:  Yi Lyu
 * Email:   isabella_aus_china@sjtu.edu.cn
 * Date:    2020.04.15
 */

#include <iostream>
#include <time.h>
#include "../include/kvstore.h"

using namespace std;

/**
 * Test throughput of put operations
 * Notice:  merge is enabled
 */

int main() {
    KVStore store = KVStore("./data");
    uint64_t cnt = 0;

    vector<int> time;
    vector<int> throughput;

    int max = 100000;

    clock_t start = clock();
    clock_t end = clock();

    for (int i = 0; i < max + 1; ++i) {
        if ((i != 0) && (i % 5000 == 0)) {
            end = clock();
            time.push_back(end - start);
            throughput.push_back(1000000.0 / (end-start) * 5000);
            start = clock();
        }
        cout << i << endl;
        store.put(rand() % 10000, std::string(rand() % 10000 + 2000, 's'));
    }

    for (int i = 0; i < time.size(); ++i) {
        cout << time[i] << ",";
    }
    cout << endl;

    for (int i = 0; i < throughput.size(); ++i) {
        cout << throughput[i] << ",";
    }
    cout << endl;
}