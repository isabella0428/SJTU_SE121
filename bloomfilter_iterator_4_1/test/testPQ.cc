#include "../include/kvstore_struct.h"
#include <queue>
#include <iostream>

int main() {
    priority_queue<Entry_time, vector<Entry_time>,greater<Entry_time>> pq;
    std::cout <<"Ao" << std::endl;

    for (int i = 10; i >= 0; --i) {
        pq.push(Entry_time(i, "sss", i));
        std::cout <<"AOOO" << std::endl;
    }

    for (int i = 0; i < 10; ++i) {
        std::cout << pq.top()._key << std::endl;
        pq.pop();
    }
}