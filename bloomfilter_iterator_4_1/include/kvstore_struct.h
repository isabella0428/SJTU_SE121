#pragma once

#include <vector>
#include <iostream>

using namespace std;

/**
 * Store key and value, using template to correspond with skiplist(all using template)
 */
template <typename K, typename V>
struct Entry
{
    K _key;
    V _value;
    Entry(K key, V value) : _key(key), _value(value) {}
    Entry() {}
};

/**
* Store key and offset in sstable
*/
struct key_offset
{
    uint64_t _key;
    uint64_t _offset;
    key_offset(uint64_t key, uint64_t offset) : _key(key), _offset(offset) {}
    key_offset() {}
};

/**
* Store 4 things:
* 1. all key offset
* 2. maxkey
* 3. minkey
* 4. timestamp
*/
struct sstable_index
{
    vector<key_offset> _vector;
    uint64_t _max, _min;
    int _timestamp;
    sstable_index(vector<key_offset> q, int timeStamp, uint64_t max, uint64_t min)
        : _vector(q), _timestamp(timeStamp), _max(max), _min(min) {}
    sstable_index() {}
};

/**
* Store maxkey and minkey
*/
struct key_range
{
    uint64_t _max;
    uint64_t _min;
    key_range(uint64_t max, uint64_t min) : _max(max), _min(min) {}
    key_range() {}
};

/**
 * Store Entry with timestamp(when it is created)
 */
struct Entry_time
{
    uint64_t _key;
    string _value;
    int _time;
    Entry_time(uint64_t key, string value, int time) : _key(key), _value(value), _time(time) {}
    Entry_time(const Entry_time &bk) : _key(bk._key), _value(bk._value), _time(bk._time) {}
    Entry_time() {}
    friend bool operator>(Entry_time e1, Entry_time e2)
    {
        if ((e1._key < e2._key) || ((e1._key == e2._key) && (e1._time > e2._time)))
        {
            return false;
        }
        return true;
    };
};

/**
 * Output Iterator to access key and value
 * In order to correspond with c++ convention, first and second don't have "_"
 */

class scan_container {
    std::vector< std::pair<uint64_t, string> > _vec;

    public:
        scan_container(std::vector<std::pair<uint64_t, string>> vec):_vec(vec) {}

        class iterator {
            int _pos;
            std::vector<std::pair<uint64_t, string>> _vec;

        public:
            iterator(int pos, std::vector<std::pair<uint64_t, string>> vec ) : _pos(pos), _vec(vec) {}
            iterator(const iterator &it) {
                this->_pos = it._pos;
                this->_vec = it._vec;
            }
            ~iterator() {}

            // ++iter
            iterator operator++()
            {
                this->_pos++;
                return *this;
            }

            // iter++
            iterator operator++(int)
            {
                this->_pos++;
                return *this;
            }

            // ++iter
            iterator operator--()
            {
                this->_pos--;
                return *this;
            }

            // iter++
            iterator operator--(int)
            {
                this->_pos--;
                return *this;
            }

            std::pair<uint64_t, std::string> &operator*()
            {
                return this->_vec[this->_pos];
            }

            bool operator!=(iterator it)
            {
                return this->_pos != it._pos;
            }
        };
        iterator begin()
        {
            return iterator(0, this->_vec);
        }

        iterator end()
        {
            return iterator(this->_vec.size(), vector<std::pair<uint64_t, std::string>>());
        }
};
