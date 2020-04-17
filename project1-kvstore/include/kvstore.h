/**
 * Author:  Yi Lyu
 * Email:   isabella_aus_china@sjtu.edu.cn
 * Date:    2020.04.15
 */

#pragma once

#include "skipList.h"
#include "kvstore_api.h"
#include "kvstore_struct.h"
#include "bloomfilter.h"
#include <cmath>
#include <vector>
#include <set>
#include <map>

using namespace std;

class KVStore : public KVStoreAPI {
private:
	SkipList<uint64_t, string> *memtable;					// Memory part: SkipList
	int mem_limit, current_size, sstable_num, max_level;	// Private member for memtable, sstable number and max_level
	char *buffer, *offsetBuffer;							// Buffer for batch write
	vector<bloomfilter> all_sstable_bmfilter;				// Store all sstable's bloomfilter
	vector<sstable_index> all_sstable_index; 				// Store all sstable index part
	vector<set<int>> level_sstable_num;						// The sstable num in each level
	string sstable_base_dir;								// sstable dir
	map<int, int> sstable_level;							// map sstable no to level np

	// Number of sstable limit of each level
	int level_limit(int level) { return pow(2, level + 1); }

	// Check if there exists sstable level that exceeds the limit
	bool check_level();

	// Check if the specfic level exceeds the memory limit
	bool check_level(int level, int limit);

	// Save the entry as sstable
	bool save_as_sstable(const vector<Entry_time> &vec, int level);

	// Compact sstables
	bool compaction(int level, int filenumber, int limit);

	// Get the key range of a specfic sstable
	key_range get_key_range(int file_id);

	// Get sstable no or level no given its path
	int get_id(string path);

	// Merge different sstables
	bool merge_files(const vector<int> &lower_layer_sstable_num, const vector<int> &higher_layer_sstable_num, int level);

	// Binary Search if a key exist in the sstable
	bool binary_search_sstable(const sstable_index &sstable_index, uint64_t key, string *value, int sstable_id, int &timestamp);

	// Read all key-value pair from sstable
	vector<Entry_time> read_key_value_from_file(string file_path, int level);

	// Select sstables from the level which exceeds the limit(merge with the next level)
	vector<int> select_level_sstable(int level, int filenumber, int limit);

	// K merge sort
	vector<Entry_time> k_merge_sort(vector< vector<Entry_time> > &all_sstable_content);

	// Functions for persistence
	void recover();

	bool recover_sstable_level();

	bool recover_index();

	bool recover_bf();

public:
	KVStore(const std::string &dir);

	~KVStore();

	void put(uint64_t key, const std::string &s) override;

	std::string get(uint64_t key) override;

	bool del(uint64_t key) override;

	void reset() override;

	scan_container scan(uint64_t key1, uint64_t key2) override;
};


