/**
 * Author:  Yi Lyu
 * Email:   isabella_aus_china@sjtu.edu.cn
 * Date:    2020.04.15
 */

#include "../include/bloomfilter.h"
#include "../include/MurmurHash3.h"

bloomfilter::bloomfilter(int bf_no, int hash_k, int arr_length)
    : k(hash_k), m(arr_length), no(bf_no)
{
    array = new int[arr_length];
    memset(array, 0, arr_length * sizeof(int));
}

// Add value to bloomfilter
bool bloomfilter::add(uint64_t insert_key)
{
    // Do MurmurHash3
    uint64_t *key = &insert_key;
    uint32_t hash[1];

    for (int h = 0; h < this->k; ++h)
    {
        MurmurHash3_x86_32((void *)key, sizeof(uint64_t), 100 + h + this->no, hash);
        this->array[(*hash) % this->m] = 1;
    }
    return true;
}

// Check if the key is added in the bloomfilter
// Can be false positive
bool bloomfilter::contains(uint64_t insert_key)
{
    uint64_t *key = &insert_key;
    uint32_t hash = 0;

    for (int i = 0; i < this->k; ++i)
    {
        MurmurHash3_x86_32((void *)key, sizeof(uint64_t), 100 + i + this->no, &hash);
        if (this->array[(hash) % this->m] != 1)
            return false;
    }
    return true;
}