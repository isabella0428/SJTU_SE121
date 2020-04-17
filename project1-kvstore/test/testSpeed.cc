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
 * Test average time cost of put, delete and gets
 * Notice:  Merge is disabled
 */

void test(int max) {
    KVStore store = KVStore("./data");
    uint64_t total_put_time = 0;
    uint64_t total_delete_time = 0;
    uint64_t total_get_time = 0;
    srand(time(NULL));
    
    clock_t start;
    for (int i = 0; i < max; ++i)
    {
        uint64_t key = rand() % 10000;
        uint64_t value_len = rand() % 10000;

        start = clock();
        store.put(key, std::string(value_len, 's'));
        total_put_time += clock() - start;

        start = clock();
        store.get(key);
        total_get_time += clock() - start;

        start = clock();
        store.del(key);
        total_delete_time += clock() - start;
    }

    cout << "Size " << max  << endl;
    cout << "Put average time is " << total_put_time / (max) << endl;
    cout << "Get average time is " << total_get_time / (max) << endl;
    cout << "Delete average time is " << total_delete_time / (max) << endl;
}

int main() {
    
    for (int i = 1; i <= 10; ++i) {
        test(i * 10000);
    }
}