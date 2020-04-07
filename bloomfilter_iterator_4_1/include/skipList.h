#pragma once

#include "kvstore_struct.h"
#include <iostream>
#include <vector>

using std::vector;

template <typename K, typename V>
class SkipList
{
private:
    struct Node
    {
        K key;
        V value;
        vector<Node *> forward;         // Store the forward node, forward[0] means the next node on level0

        Node(K key, V value, int level) : key(key), value(value), forward(level + 1, nullptr) {}
        Node(int level): forward(level + 1, nullptr) {}               
    };

    Node *header;
    Node *trailer;

    /**
     * Initialize node with corresponding levels (for header and trailer)
     */
    void initNode(Node *&node, int level)
    {
        node = new Node(level);
    }

    /**
     * Initialize node with corresponding levels（Except header and trailer）
     */
    void initNode(Node *&node, int level, K key, V value)
    {
        node = new Node(key, value, level);
    }

    /**
     * Return the level number of the skiplist
     */
    int getLevel()
    {
        return header->forward.size() - 1;
    }

    // Get random level
    int randomLevel()
    {
        int level = 0;
        while (rand() % 2)
        {
            level++;
        }
        return level;
    }

    // Increase skip list level (mainly for header)
    void increaseLevel(int targetLevel)
    {
        int curLevel = header->forward.size() - 1;
        for (int i = curLevel + 1; i <= targetLevel; ++i)
        {
            header->forward.push_back(trailer);
        }
    }

    void decreaseLevel()
    {
        int curLevel = header->forward.size() - 1;
        int i;
        for (i = curLevel; i >= 0; --i)
        {
            if (header->forward[i] == trailer)
            {
                break;
            }
        }
        // At least keep the last level
        i = i > 1 ? i : 1;
        header->forward.erase(header->forward.begin() + i, header->forward.end());
    }

public:
    // Initialize skip list
    SkipList(int initLevel = 10)
    {
        // Initialize header
        initNode(header, initLevel);
        // Initialize trailer
        initNode(trailer, 0);
        // Make every level of header points to trailer
        for (int i = 0; i <= initLevel; ++i)
        {
            header->forward[i] = trailer;
        }
    }

    // Free space
    ~SkipList()
    {
        clear();
        delete header;
        delete trailer;
    }

    vector<Entry<K, V>> getAllElement() {
        vector<Entry<K, V>> all_elements;
        Node *cur = header->forward[0];
        while(cur != trailer) {
            all_elements.push_back(Entry<K, V>(cur->key, cur->value));
            cur = cur->forward[0];
        }

        return all_elements;
    }

    vector<Entry_time> getAllElement(int sstable_num) {
        vector<Entry_time> all_elements;
        Node *cur = header->forward[0];
        while(cur != trailer) {
            all_elements.push_back(Entry_time(cur->key, cur->value, sstable_num));
            cur = cur->forward[0];
        }

        return all_elements;
    }

    // clear skiplist
    void clear()
    {
        Node *cur = header->forward[0];
        while (cur != trailer)
        {
            Node *next = cur->forward[0];
            delete cur;
            cur = next;
        }

        for (int l = 0; l < header->forward.size(); ++l) {
            header->forward[l] = trailer;
        }
    }

    // Put key and value in skipList
    bool put(K key, V value)
    {
        // Get random level
        int nodeLevel = randomLevel();
        // Increase level
        if (nodeLevel > getLevel())
        {
            increaseLevel(nodeLevel);
        }
        // Record prev pointer into vector
        vector<Node *> prev(nodeLevel + 1, header);
        Node *node = header;
        for (int i = nodeLevel; i >= 0; i--)
        {
            while ((node->forward[i] != trailer) && (node->forward[i]->key <= key))
            {
                node = node->forward[i];
            }
            prev[i] = node;
        }

        // Create new node
        Node *newNode;
        initNode(newNode, nodeLevel, key, value);
        for (int i = 0; i <= nodeLevel; ++i)
        {
            newNode->forward[i] = prev[i]->forward[i];
            prev[i]->forward[i] = newNode;
        }
        return true;
    }

    // Get key and corresponding value
    bool get(K key, V *value)
    {
        int curLevel = getLevel();
        Node *node = header;
        // traverse right and down
        for (int i = curLevel; i >= 0; --i)
        {
            while ((node->forward[i] != trailer) && (node->forward[i]->key <= key))
            {
                node = node->forward[i];
            }
        }
        // Compare the result
        if (node != header && node->key == key)
        {
            *value = node->value;
            return true;
        }
        return false;
    }

    // Delete key
    bool del(K key)
    {
        int curLevel = getLevel();
        Node *node = header;
        vector<Node *> prev(curLevel + 1);
        // Find previous node
        for (int i = curLevel; i >= 0; --i)
        {
            while ((node->forward[i] != trailer) && (node->forward[i]->key < key))
            {
                node = node->forward[i];
            }
            prev[i] = node;
        }
        node = node->forward[0];
        if (node->key != key)
        {
            return false;
        }
        // Update the skip list
        for (int i = 0; i <= curLevel; ++i)
        {
            if (prev[i]->forward[i] != node)
            {
                break;
            }
            prev[i]->forward[i] = node->forward[i];
        }
        delete node;
        // decrease level and reduce extra space
        decreaseLevel();
        return true;
    }
};