#include "../src/kvstore.cc"

#include <iostream>
#include <cstdint>
#include <string>

using namespace std;

int main()
{
    KVStore kv_store = KVStore("./data");
    kv_store.put(123, "Aloha");
    kv_store.put(145, "heigj");

    kv_store.put(456, "yiyayiya");
    kv_store.put(789, "Beckya");

    kv_store.put(1001, "Joya");
    kv_store.put(1300, "Picky");

    kv_store.put(928, "ticks");
    kv_store.put(1245, "Olhau");

    kv_store.put(9230, "licks");
    kv_store.put(1450, "UIlhau");

    kv_store.put(230, "tiktiok");
    kv_store.put(2459, "Ukraine");

    cout << kv_store.get(123) << endl;
    cout << kv_store.get(3450) << endl;

    // kv_store.put(1240, "Bella");
    // kv_store.put(220, "Vicky");
}