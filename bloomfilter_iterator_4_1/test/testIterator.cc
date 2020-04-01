#include "../include/kvstore.h"
#include "../include/kvstore_struct.h"

int main() {
    KVStore store("./data");
    store.put(124, "onetwofour");
    store.put(456, "heiheiya");
    store.put(789, "seveneightnine");

    scan_container vec = store.scan(100, 800);
    for (auto iter = vec.begin(); iter != vec.end(); iter++) {
        auto p = *iter;
        std::cout << (*iter).first << " " << (*iter).second << endl;
    }
}