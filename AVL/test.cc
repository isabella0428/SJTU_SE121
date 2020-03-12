#include "AVL.cc"
#include <iostream>

int main() {
    AVL<int> *avl = new AVL<int>();
    for (int i = 1; i <= 8; ++i) {
        avl->insert(i);
        avl->print();
        std::cout<<endl;
    }

    avl->remove(5);
    avl->remove(6);
    avl->remove(7);
    avl->print();
}