#pragma once
#include <iostream>

template <typename T> struct TreeNode {
    T _data;
    TreeNode<T> *_left, *_right, *_pred;
    TreeNode(T data, TreeNode<T> *pred=nullptr):_data(data), _pred(pred) {_left = nullptr; _right = nullptr;_pred = pred;}
    TreeNode() {_left = nullptr; _right = nullptr; _pred = nullptr;}

    int height(TreeNode<T> *node) {
        if (node == nullptr) {
            return 0;
        } 

        return 1 + max(height(node->_left), height(node->_right));
    }

    int max(int a, int b) {return a > b ? a : b;}

    bool is_leftchild() {
        TreeNode<T> *pred = _pred;
        if (pred->_left == this) {
            return true;
        }
        return false;
    }

    bool is_balanced() {
        int l_height =  height(this->_left);
        int r_height = height(this->_right);
        int abs = l_height - r_height;
        return (-2 < abs) && (abs < 2);
    }

    TreeNode<T>* taller_child() {
        int l_height = height(_left);
        int r_height = height(_right);

        if (l_height < r_height) {
            return _right;
        }

        if (l_height > r_height) {
            return _left;
        }

        return is_leftchild() ? _left: _right;
    }

    // Return the next largest element
    TreeNode<T> *succ() {
        TreeNode<T> *succ = this->_right;
        while((succ != nullptr) && (succ->_left != nullptr)) {
            succ = succ->_left;
        }
        return succ;
    }
};

template <typename T> class AVL {
    int _size;
    TreeNode<T> *_root, *_hot;
    bool del(TreeNode<T> *node);

    protected:
        void init();
        void clear();

    public:
        AVL() { init(); }
        ~AVL() { clear(); }
        void print() { print(_root); }
        void print(TreeNode<T> *node);
        bool insert(T data);
        bool remove(T data);
        bool search(T data);
        bool rotate(TreeNode<T> *node);
        bool connect3_4(TreeNode<T> *v, TreeNode<T> *p, TreeNode<T> *g, 
            TreeNode<T> *t0, TreeNode<T> *t1, TreeNode<T> *t2, TreeNode<T> *t3);
};