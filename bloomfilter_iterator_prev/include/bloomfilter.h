#pragma once

#include <iostream>
#include <cstring>
class bloomfilter {
    int k;      // Number of hash function
    int m;      // length of bit vector
    int no;     // bloomfilter no, used in hash function -- murmur3
    int *array; // bit vector

    public:
        bloomfilter(int no, int k = 4, int length = 2000);
        bool clear() { delete []array; return true;}
        bool add(uint64_t key);
        bool contains(uint64_t key);
};