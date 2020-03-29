#include "../include/skipList.h"

#include <iostream>
#include <cstdint>
#include <string>

using namespace std;

int main()
{
    SkipList<int, string> skList = SkipList<int, string>();
    
    // Test put
    skList.put(1, "hello");
    skList.put(10, "Aloha");
    skList.put(3, "akkkkk");

    // Test get
    string *ans = new string("");
    int result;
    result = skList.get(10, ans);
    result = skList.get(1, ans);
    result = skList.get(100, ans);

    // Test del
    skList.del(1);
    skList.del(10);
    skList.del(300);
}