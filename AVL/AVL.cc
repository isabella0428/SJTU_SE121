#include "AVL.h"
#include <vector>
using namespace std;

template <typename T>
void AVL<T>::init() {
    _root = new TreeNode<T>();
    _hot = new TreeNode<T>();
    _size = 0;
}

template <typename T> bool AVL<T>::del(TreeNode<T> *node) {
    bool ans = true;

    if (node == nullptr) {
        return false;
    }

    if (node->_left != nullptr) {
        ans &= del(node->_left);
    }
    
    if (node->_right != nullptr) {
        ans &= del(node->_right);
    }

    delete node;
    return ans;
}

template <typename T>
void AVL<T>::clear() {
    del(_root);
}

template <typename T>
bool AVL<T>::search(T data) {
    _hot = _root;

    while(true) {
        if (_hot->_data == data) {
            return true;
        }

        if (_hot->_data > data) {
            if (_hot->_left != nullptr) {
                _hot = _hot->_left;
            } else {
                return false;
            }
        } else {
            if (_hot->_right != nullptr) {
                _hot = _hot->_right;
            } else {
                return false;
            }
        }
    }
}

template <typename T>
bool AVL<T>::insert(T data) {
    if (_size == 0) {
        _root->_data = data;
        _size++;
        return true;
    }

    if (search(data)) {
        return false;
    }

    // Insert
    if (data < _hot->_data) {
        _hot->_left = new TreeNode<T>(data);
        _hot->_left->_pred = _hot;
    } else {
        _hot->_right = new TreeNode<T>(data);
        _hot->_right->_pred = _hot;
    }
    _size++;

    // Test if it is balanced
    for(TreeNode<T> *node = _hot; node != nullptr; node = node->_pred) {
        bool factor = node->is_balanced();
        if (!node->is_balanced()) {
            rotate(node->taller_child()->taller_child());
            break;      // Insert will only let one node unbalanced
        }
    }
    return true;
}

template <typename T>
bool AVL<T>::connect3_4(TreeNode<T> *v, TreeNode<T> *p, TreeNode<T> *g,
                        TreeNode<T> *t0, TreeNode<T> *t1, TreeNode<T> *t2, TreeNode<T> *t3) {
    v->_left = t0;
    if (t0 != nullptr) 
        t0->_pred = v;
    v->_right = t1;
    if (t1 != nullptr)
        t1->_pred = v;

    g->_left = t2;
    if (t2 != nullptr)
        t2->_pred = g;
    g->_right = t3;
    if (t3 != nullptr)
        t3->_pred = g;

    p->_left = v;
    v->_pred = p;
    p->_right = g;
    g->_pred = p;
    return true;
}

template <typename T> bool AVL<T>::rotate(TreeNode<T> *v) {
    TreeNode<T> *p, *g;
    p = v->_pred;
    g = p->_pred;

    if (p->is_leftchild()) {
        if (v->is_leftchild())  { 
            //zig-zig
            if (g == _root) {
                _root = p;
            } else {
                if (g->is_leftchild()) {
                    g->_pred->_left = p;
                } else {
                    g->_pred->_right = p;
                }
            }
            p->_pred = g->_pred;
            connect3_4(v, p, g, v->_left, v->_right, p->_right, g->_right);
        } else {
            //zig-zag
            if (g == _root)
            {
                _root = v;
            }
            else
            {
                if (g->is_leftchild())
                {
                    g->_pred->_left = v;
                } else {
                    g->_pred->_right = v;
                }
            }
            v->_pred = g->_pred;
            connect3_4(p, v, g, p->_left, v->_left, v->_right, g->_right);
        }
    } else {
        // zag-zig
        if (v->is_leftchild()) {
            if (g == _root)
            {
                _root = v;
            }
            else
            {
                if (g->is_leftchild())
                {
                    g->_pred->_left = v;
                }
                else
                {
                    g->_pred->_right = v;
                }
            }
            v->_pred = g->_pred;
            connect3_4(g, v, p, g->_left, v->_left, v->_right, p->_right);
        } else {
            // zag-zag
            if (g == _root)
            {
                _root = p;
            }
            else
            {
                if (g->is_leftchild())
                {
                    g->_pred->_left = p;
                }
                else
                {
                    g->_pred->_right = p;
                }
            }
            p->_pred = g->_pred;
            connect3_4(g, p, v, g->_left, p->_left, v->_left, v->_right);
        }
    }

    return true;
}

template <typename T>
bool AVL<T>::remove(T data) {
    if (!search(data)) {
        return false;
    }

    TreeNode<T> *succ = new TreeNode<T>();
    TreeNode<T> *w = new TreeNode<T>();
    TreeNode<T> *p = new TreeNode<T>();
    if (_hot->_left == nullptr) {
        succ = _hot->_right;
        p = _hot->_pred;
        w = _hot;
    } else {
        if (_hot->_right == nullptr) {
            succ = _hot->_left;
            p = _hot->_pred;
            w = _hot;
        } else {
            w = _hot->succ();
            // Swap the data in the two nodes
            _hot->_data = w->_data;
            w->_data = data;

            succ = w->_right;
            p = w->_pred;
        }
    }

    if (w->is_leftchild()) {
        p->_left = succ;
        if(succ != nullptr)
            succ->_pred = p;
    }
    else {
        p->_right = succ;
        if (succ != nullptr)
            succ->_pred = p;
    }

    // Test if it is balanced
    for (TreeNode<T> *node = p; node != nullptr; node = node->_pred)
    {
        bool factor = node->is_balanced();
        if (!node->is_balanced())
        {
            rotate(node->taller_child()->taller_child());
        }
    }

    delete w;
    _hot = new TreeNode<T>();
    return true;
}

template <typename T> void AVL<T>::print(TreeNode<T> *node) {
    if (node == nullptr) return;
    print(node->_left);
    cout << node->_data << " ";
    print(node->_right);
    return;
}