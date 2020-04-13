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
	SkipList<uint64_t, string> *memtable;
	int mem_limit;
	int current_size;
	int sstable_num;
	int max_level;
	vector<bloomfilter> all_sstable_bmfilter;
	vector<sstable_index> all_sstable_index; 
	string sstable_base_dir;
	map<int, int> sstable_level;

	int level_limit(int level) { return pow(2, level + 1); }

	bool check_level();

	bool check_level(int level, int limit);

	bool save_as_sstable(const vector<Entry_time> &vec, int level);

	bool compaction(int level, int filenumber, int limit);

	key_range get_key_range(int file_id);

	int get_id(string path);

	bool merge_files(vector<string> lower_layer_sstable_path, vector<string> higher_layer_sstable_path, int level);

	bool binary_search_sstable(const sstable_index &sstable_index, uint64_t key, string *value, int sstable_id, int &timestamp);

	vector<Entry_time> read_key_offset_from_file(string file_path, int level);

	vector<Entry_time> merge_two_files(
		vector<Entry_time> file1, vector<Entry_time> file2);

	vector<string> select_sstable_path(int level, int filenumber, int limit);

	vector<int> get_level_sstable_num(int level);

	vector<Entry_time> k_merge_sort(const vector< vector<Entry_time> > &all_sstable_content);

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


