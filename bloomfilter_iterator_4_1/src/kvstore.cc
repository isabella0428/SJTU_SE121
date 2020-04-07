#include "../include/kvstore.h"
#include <filesystem>
#include <queue>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <iostream>
#include "time.h"
#include <chrono>

using namespace std;

/**
 * Initializes the memory table
 */
KVStore::KVStore(const std::string &dir) : KVStoreAPI(dir)
{
	// Initialize memtable
	memtable = new SkipList<uint64_t, string>();
	mem_limit = 2000000;
	current_size = 0;
	sstable_num = 0;
	sstable_base_dir = dir;
	max_level = -1;

	recover();

	// For level file picking
	srand(time(NULL));
}

/**
 * Recover memory index, bloomfilter
 */
void KVStore::recover() {
	recover_index();
	recover_bf();
	recover_sstable_level();
}


/**
 * Recover key offset from file "base_dir/manifest"
 * So that we can recover all index data even if system breakdowm
 */
bool KVStore::recover_index() {
	string index_path = this->sstable_base_dir + "/manifest";

	fstream in;
	in.open(index_path, ios::binary|ios::in);

	if (!in.is_open())
		return false;

	in.seekg(0, ios::end);
	long total_bytes = in.tellg();
	in.seekg(0, ios::beg);

	uint64_t key_offset_pair_num, key, offset;
	while (total_bytes > 0)
	{
		// Read number of key_offset pair from file
		in.read((char *)&key_offset_pair_num, sizeof(uint64_t));
		total_bytes -= sizeof(uint64_t);
		total_bytes -= 2 * sizeof(uint64_t) * key_offset_pair_num;

		// Start to read key_offset from file
		// Store in memory as sstable index
		sstable_index sst_index;
		while (key_offset_pair_num > 0)
		{
			in.read((char *)&key, sizeof(uint64_t));
			in.read((char *)&offset, sizeof(uint64_t));
			--key_offset_pair_num;
			sst_index._vector.push_back(key_offset(key, offset));
		}
		++sstable_num;
		all_sstable_index.push_back(sst_index);
	}

	in.close();
	return true;
}

/**
 * Recover bloomfilter from file "base_dir/bloomfilter"
 * So that we can recover all bloomfilter data even if system breakdowm
 */
bool KVStore::recover_bf() {
	string bf_path = this->sstable_base_dir + "/bloomfilter";

	fstream in;
	in.open(bf_path, ios::binary|ios::in);

	in.seekg(0, ios::end);
	long total_bytes = in.tellg();
	in.seekg(0, ios::beg);

	int sstable_id = 0;
	while(total_bytes > 0) {
		bloomfilter bf(sstable_id++);
		int *array;
		array = new int[bf.getArrayLength()];
		in.read((char *)array, sizeof(int) * bf.getArrayLength());

		for (int i = 0; i < bf.getArrayLength(); ++i) {
			bf.setArray(i, array[i]);
		}

		all_sstable_bmfilter.push_back(bf);
		total_bytes -= sizeof(int) * bf.getArrayLength();
		delete []array;
	}
	in.close();
	return true;
}

/**
 * Recover sstable_level map
 */
bool KVStore::recover_sstable_level() {
	string path = sstable_base_dir + "/";
	// string path = sstable_base_dir + "/" + "level" + to_string(level) + "/";

	if (!std::filesystem::exists(path))
		return false;

	// Check if the files have the extension ".sst"
	regex level_regex(".*level.*");
	regex sst_regex(".*\\.sst");
	smatch base_match;
	string level_path, file_path;
	level_sstable_num.push_back(set<int>());
	for (auto &p : filesystem::directory_iterator(path))
	{
		level_sstable_num.push_back(set<int>());
		// Find level directories
		level_path = p.path();

		if (regex_match(level_path, base_match, level_regex))
		{
			int level = get_id(level_path);
			if (max_level < level)
			{
				max_level = level;
			}

			// Find sstable files
			for (auto &file_p : filesystem::directory_iterator(level_path))
			{
				file_path = file_p.path();
				if (regex_match(file_path, base_match, sst_regex)) {
					int id = get_id(file_path);
					sstable_level.insert(make_pair<>(id, level));
					level_sstable_num[level].insert(id);
				}
			}
		}
	}
	
	return true;
}


/**
 * Release all resources
 * --Clean all sstables
 * --Clear the memory table
 */
KVStore::~KVStore()
{
	// Clean directory
	filesystem::remove_all(this->sstable_base_dir);
	delete this->memtable;
}

/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key, const std::string &s)
{
	// Insert the entry into the memory table
	this->memtable->put(key, s);
	current_size += sizeof(key);
	current_size += s.length();

	// If the memorytable's size is greater than the limit, store it to sstable
	if (this->current_size > this->mem_limit)
	{
		auto key_value_vec = this->memtable->getAllElement(sstable_num);
		// Store the elements to sstable
		if (!save_as_sstable(key_value_vec, 0, true))
		{
			cout << "Failed to save as sstable!" << endl;
			return;
		}

		// Check if the number of elements in each level is okay
		if (!check_level())
		{
			cout << "Failed to do compaction!" << endl;
			return;
		}

		// Clear memtable
		this->memtable->clear();
		current_size = 0;
	}
}

/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
string KVStore::get(uint64_t key)
{
	string value = "";

	// First Search MemoryTable
	if (this->memtable->get(key, &value))
	{
		return value;
	}

	uint64_t maxKey, minKey;
	int timestamp = -1, cur_timestamp;
	string cur_value = "";

	for (int l = 0; l <= max_level; ++l)
	{
		for (int num : get_level_sstable_num(l))
		{
			// cout << "total number " << get_level_sstable_num(l).size() << endl;
			// cout << "current number is " << num << endl;
			if (!all_sstable_bmfilter[num].contains(key))
			{
				continue;
			}
			// If the key is within the key_range of the sstable, do binary search
			// Since there may be multiple same keys with different timestamp
			// return the one with the biggest timestamp
			if (binary_search_sstable(all_sstable_index[num], key, &cur_value, num, cur_timestamp))
			{

				if (cur_timestamp > timestamp)
				{
					timestamp = cur_timestamp;
					value = cur_value;
				}
			}
		}

		if (value.length() > 0 || timestamp != -1)
		{
			return value;
		}
	}

	return value;
}

/**
 * Return an iterator of the key-value pairs whose keys lie in the range of key1 and key2
 * key lies within [k1, k2]
 */
scan_container KVStore::scan(uint64_t key1, uint64_t key2)
{
	vector<std::pair<uint64_t, string>> vec;

	for (int k = key1; k <= key2; ++k)
	{
		string value = this->get(k);

		// The key exists!
		if (value.length() > 0)
		{
			vec.push_back(std::make_pair<>(k, value));
		}
	}
	return scan_container(vec);
}

/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key)
{
	string value;
	if (this->get(key).length() == 0)
	{
		return false;
	}
	this->put(key, "");
	return true;
}

/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
{
	this->memtable->clear();
	current_size = 0;
	if (filesystem::exists(this->sstable_base_dir))
	{
		filesystem::remove_all(this->sstable_base_dir);
		filesystem::create_directories(this->sstable_base_dir);
	}
}

/**
 * This check if the sstable file number exceed the level limit
 * If exceeds, call compaction
 */
bool KVStore::check_level()
{
	return check_level(0, level_limit(0));
}

/**
 * Helper funcition for check_level()
 * Check if the number of files in the given directory exceed the limit
 * If exceed, call compaction
 */
bool KVStore::check_level(int level, int limit)
{
	string path = sstable_base_dir + "/" + "level" + to_string(level) + "/";

	if (!std::filesystem::exists(path))
		return false;

	// Count the number of files in the directory
	int file_number = 0;

	// Check if the files have the extension ".sst"
	string file_name;
	file_number = level_sstable_num[level].size();
	// If the file number exceed the limit, it should do compaction
	if (file_number > limit)
	{
		return compaction(level, file_number, limit);
	}

	return true;
}

/**
 * Do sstable compaction when the file number on this level exceeds
 * Combine the files with the files in the lower level when their key range overlap
 */
bool KVStore::compaction(int level, int filenumber, int limit)
{
	// Get this level and  next level path
	string level_path = sstable_base_dir + "/level" + to_string(level) + "/";
	string next_level_path = sstable_base_dir + "/level" + to_string(level + 1) + "/";

	bool result = true;
	string target_path;

	// If the next level doesn't exist, create the directories and copy all files there
	if (!filesystem::exists(next_level_path))
	{
		level_sstable_num.push_back(set<int>());
		filesystem::create_directories(next_level_path);
		max_level = level + 1;
		if (level > 0) {
			string file_path;
			for (auto &p : select_sstable_path(level, filenumber, limit))
			{
				file_path = p;
				int num = get_id(file_path);
				target_path = next_level_path + to_string(num) + ".sst";
				sstable_level[num] = level + 1;
				level_sstable_num[level].erase(num);
				level_sstable_num[level + 1].insert(num);
				filesystem::rename(file_path, target_path);
			}
			return true;
		}
	}
	
	// Get the key range of the files of the lower layer
	uint64_t max_key = 0, min_key = 0xffffffffffffffff;
	vector<string> lower_layer_sstable_path = select_sstable_path(level, filenumber, limit);
	string sstable_file_path;

	for (auto iter = lower_layer_sstable_path.begin(); iter != lower_layer_sstable_path.end(); ++iter)
	{
		int id = get_id(*iter);

		// Get the key range of the sstable file from index in memory
		key_range cur_key_range = get_key_range(id);
		max_key = max_key > cur_key_range._max ? max_key : cur_key_range._max;
		min_key = min_key < cur_key_range._min ? min_key : cur_key_range._min;
	}

	// Find all files in the next level whose key ranges overlaps with this key range
	string extension = ".sst";
	vector<string> higher_level_sstable_path;
	key_range k_range;
	// Use in memory index
	for (auto it = level_sstable_num[level + 1].begin(); it != level_sstable_num[level + 1].end(); ++it) {
		sstable_file_path = next_level_path + to_string(*it) + ".sst";
		k_range = get_key_range(*it);
		if ((k_range._max < min_key) || (k_range._min > max_key))
		{
			continue;
		}

		// If the sstable file in the higher level overlaps with the selected file in lower layer,
		// Do compactions with them
		higher_level_sstable_path.push_back(sstable_file_path);
	}
	return merge_files(lower_layer_sstable_path, higher_level_sstable_path, level + 1);
}

/**
 * Merge the files and split them into 2M sstables
 */
bool KVStore::merge_files(vector<string> file1, vector<string> file2, int next_level)
{
	int size_1 = file1.size();
	int size_2 = file2.size();

	// Timestamp of the newly merged sstables
	int new_timestamp = sstable_num;

	// vector[i] stores the entry time of a sstable
	vector<vector<Entry_time>> all_sstable_content(file1.size() + file2.size(), vector<Entry_time>());
	// Merge the key offset of file1 and file2 into a vector
	for (auto i = 0; i < file1.size() + file2.size(); ++i)
	{
		int level;
		string path;
		int id;
		if (i < file1.size())
		{
			path = file1[i];
			id = get_id(path);
			level = 0;
			level_sstable_num[next_level - 1].erase(id);
		}
		else
		{
			path = file2[i - file1.size()];
			id = get_id(path);
			level = 1;
			level_sstable_num[next_level].erase(id);
		}
		// Add level as new member of entry
		all_sstable_content[i] = read_key_offset_from_file(path, level);
		filesystem::remove(path);
		// Remove from the map
		sstable_level.erase(id);
	}

	save_as_sstable(k_merge_sort(all_sstable_content), next_level, false);

	return check_level(next_level, level_limit(next_level));
}

/**
 * K merge sort
 * Sort all sstable content together
 */
vector<Entry_time> KVStore::k_merge_sort(vector<vector<Entry_time>> &all_sstable_content)
{
	int sstable_num = all_sstable_content.size();
	vector<Entry_time> merged;

	struct cmp {
		bool operator()(const Entry_time *e1, const Entry_time *e2)
		{   
			if ((e1->_key < e2->_key) || ((e1->_key == e2->_key) && (e1->_level < e2->_level)) || ((e1->_key == e2->_key) && (e1->_time > e2->_time)))
			{
				return false;
			}
			return true;
		};
	};

	priority_queue<Entry_time*, vector<Entry_time*>, cmp> pq;

	// Store the next element of each sstable
	int index[sstable_num] = {0};

	bool all_empty = true;
	while (1)
	{
		all_empty = true;
		for (int i = 0; i < sstable_num; ++i)
		{
			// Jump old key-value
			auto &sstable_content = all_sstable_content[i];
			int sstable_length = sstable_content.size();
			// Get rid of old records within a sstable
			while (index[i] < sstable_length - 1 && (sstable_content[index[i]]._key == sstable_content[index[i] + 1]._key))
			{
				++index[i];
			}
			if (index[i] < sstable_length)
			{
				pq.push(&sstable_content[index[i]++]);
				all_empty = false;
			}
		}

		if (all_empty && pq.size() == 0)
			break;

		// Remove top element
		Entry_time* e = pq.top();
		pq.pop();

		// Get rid of all old records between different sstables
		while (pq.size() > 0 && pq.top()->_key == e->_key)
		{
			// Get the smallest level with largest timestamp
			if (e->_level > pq.top()->_level) {
				e = pq.top();
			}
			pq.pop();
		}

		merged.push_back(*e);
	}
	return merged;
}

/**
 * Save MemTable as SSTable when the size of memtable exceed 2M
 * Each SSTable is at most 2M
 * Notice that when it is converted from memtable, it size can be a little higher than limit
 */
bool KVStore::save_as_sstable(
	const vector<Entry_time> &merged, int level, bool from_memtable)
{
	bool operation_result = true;
	int m_size = merged.size();
	int index = 0;

	string level_path = sstable_base_dir + "/level" + to_string(level) + "/";
	filesystem::path level_p(level_path);
	if (!exists(level_p))
	{
		create_directories(level_p);
		max_level = level;
	}

	// Resize level_sstable_num
	if (level >= level_sstable_num.size()) {
		level_sstable_num.push_back(set<int>());
	}

	while (index < merged.size())
	{
		// push sstable into this level
		level_sstable_num[level].insert(sstable_num);
		// Get the file_path of the sstable we are going to create (under level folder)
		string file_path = level_path + to_string(sstable_num) + ".sst";
		// Create the file
		ofstream out;
		uint64_t offset = 0;
		vector<key_offset> key_offset_vector; // key value pair

		bloomfilter bf = bloomfilter(sstable_num);
				// Open the file in binary form
		out.open(file_path, ios::binary | ios::out); // Open the file in binary format

		if (!out.is_open())
			return false;

		while ((offset <= this->mem_limit || from_memtable) && index < m_size)
		{

			key_offset_vector.push_back(key_offset(merged[index]._key, offset));
			// Write key
			out.write((char *)&(merged[index]._key), sizeof(merged[index]._key));
			// Write value
			out.write((char *)&(merged[index]._value[0]), merged[index]._value.length());
			offset += sizeof(merged[index]._key) + merged[index]._value.length();
			// Add to bloomfilter
			bf.add(merged[index]._key);
			index++;
		}

		out.close();

		int size = key_offset_vector.size();
		uint64_t minKey = key_offset_vector[0]._key;
		uint64_t maxKey = key_offset_vector[size - 1]._key;

		// Store sstable index in memory
		sstable_index s =
			sstable_index(
				key_offset_vector,
				sstable_num,
				maxKey,
				minKey);

		// Store sstable and its corresponding level
		sstable_level.insert(make_pair<>(sstable_num, level));

		// Store the index of the sstable in memory
		all_sstable_index.push_back(s);

		// Store the index of the sstable in disk(for persistence)
		// First store the number of key_offset pair
		string manifest_path = this->sstable_base_dir + "/manifest";
		out.open(manifest_path, ios::binary|ios::out|ios::app);

		uint64_t key_offset_pair_num = key_offset_vector.size();

		// Write the number key_value pair
		out.write((char *)&key_offset_pair_num, sizeof(uint64_t));
		// cout << "sstable pair number " << key_offset_pair_num << endl;

		for (const auto &key_offset : key_offset_vector) {
			// Write key
			out.write((char *)&(key_offset._key), 8);
			// Write value
			out.write((char *)&(key_offset._offset), 8);
		}
		out.close();

		// Store the bloomfilter to memory
		all_sstable_bmfilter.push_back(bf);

		// Store the index of the sstable in disk(for persistence)
		// First store the number of key_offset pair
		string bf_path = this->sstable_base_dir + "/bloomfilter";
		out.open(bf_path, ios::binary|ios::out|ios::app);
		out.write((char *)bf.getArray(), sizeof(int) * bf.getArrayLength());
		out.close();

		// Add sstable number
		++sstable_num;
	}
	return true;
}

/**
 * Read key-value pair from disk given the fileid and the level it is in
 */
vector<Entry_time> KVStore::read_key_offset_from_file(string file_path, int level)
{
	fstream in;
	in.open(file_path, ios::binary | ios::in);

	// Get file id
	int file_id = get_id(file_path);

	// Get certain key-offset index of the given file
	const auto &index = all_sstable_index.at(file_id);
	const auto &v = index._vector;

	// Store all key value within this file
	vector<Entry_time> all_key_value(v.size());

	// Read all key value pair with timestamp from the sstable
	uint64_t key;
	char *value;
	int key_value_length;

	int a = v.size();
	for (int i = 0; i < v.size() - 1; ++i)
	{
		key_value_length = v[i + 1]._offset - v[i]._offset;
		int len = key_value_length - sizeof(uint64_t) + 1;
		if (len > 0) {
			value = new char[len];
		} else {
			cerr << "Allocate string with len < 0 in read_key_offset_from_file()" << endl;
			exit(-1);
		}

		in.read((char *)&key, 8);
		in.read(value, key_value_length - sizeof(uint64_t));
		value[key_value_length - sizeof(uint64_t)] = '\0';
		// Add level info for each entry (0: lower level, 1: upper level)
		all_key_value[i] = Entry_time(
			key,
			string(value),
			index._timestamp,
			level);
		delete[] value;
	}

	// Read the last key_value
	in.read((char *)&key, 8);
	// Calculate the total length of the file since a binary file doesn't have eod
	in.seekg(0, ios::end);
	int length = in.tellg();
	int offset = v[v.size() - 1]._offset;
	int value_length = length - offset - 8;
	if (value_length >= 0) {
		value = new char[value_length + 1];
	} else {
		cerr << "File path: " << file_path << endl;
		cerr << "Value_length: " << value_length << endl;
		cerr << "Allocate last string with len < 0 in read_key_offset_from_file()" << endl;
		exit(-1);
	}

	in.seekg(offset + 8, ios::beg);
	in.read(value, value_length);
	value[value_length] = '\0';

	all_key_value[v.size() - 1] = Entry_time(key, string(value), index._timestamp, level);

	delete[] value;

	in.close();
	return all_key_value;
}

/**
 * Get the key range of the specific sstable
 */
key_range KVStore::get_key_range(int file_id)
{
	return key_range(
		all_sstable_index.at(file_id)._max,
		all_sstable_index.at(file_id)._min);
}

/**
 * If given file path, return file id
 * If given directories path, return level id
 */
int KVStore::get_id(string filepath)
{	int id = -1;
	int num_start = 0, start = 0;
	int length = filepath.length();
	for (int i = 0; i < length; ++i)
	{
		char c = filepath[i];
		if (c == '/' && i < length - 1)
		{
			start = num_start + 1;
		}
		num_start++;
	}

	string num = "";
	for (int i = start; i < filepath.length(); ++i)
	{
		if (filepath[i] >= '0' && filepath[i] <= '9')
		{
			num += filepath[i];
		}
	}
	return atoi(num.c_str());
}

bool KVStore::binary_search_sstable(
	const sstable_index &sstable_index,
	uint64_t key, string *value, int sstable_id, int &timestamp)
{
	// Binary Search
	vector<key_offset> vec = sstable_index._vector;
	int loc = -1;
	int low = 0, high = vec.size() - 1;
	while (low <= high)
	{
		int middle = (low + high) / 2;
		uint64_t middle_key = vec[middle]._key;

		if (middle_key == key)
		{
			loc = middle;
			break;
		}

		if (middle_key < key)
		{
			low = middle + 1;
		}
		else
		{
			high = middle - 1;
		}
	}
	if (loc == -1)
	{
		return false;
	}

	// There may exist multiple same keys
	while (loc != vec.size() - 1 && vec[loc]._key == vec[loc + 1]._key)
	{
		loc++;
	}
	// Read Value from sstable
	string path = sstable_base_dir + "/level" + to_string(sstable_level.find(sstable_id)->second) 
		+ "/" + to_string(sstable_id) + ".sst";

	fstream in;
	in.open(path, ios::binary | ios::in);
	if (!in.is_open())
	{
		return false;
	}
	int value_length;
	if (loc == vec.size() - 1)
	{
		in.seekg(0, ios::end);
		int len = in.tellg();
		value_length = len - vec[loc]._offset - 8;
	}
	else
	{
		value_length = vec[loc + 1]._offset - vec[loc]._offset - 8;
	}

	in.seekg(vec[loc]._offset + 8, ios::beg);

	char *binary_value;
	if (value_length >= 0) {
		binary_value = new char[value_length + 1];
	} else {
		cerr << "Allocate string with len < 0 in binary_search_sstable()" << endl;
		exit(-1);	
	}
	binary_value[value_length] = '\0';
	in.read(binary_value, value_length);

	in.close();

	int s_len = string(binary_value).length();

	// Set value
	string s = "";
	s.append(binary_value);
	s_len = s.length();
	*value = s;
	timestamp = sstable_index._timestamp;
	delete[] binary_value;

	return true;
}

/**
 * Select sstables from the level which sstable numbers exceed the limit
 * If it is level0, choose all sstables
 * If not, randomly select files, but the number of sstables selected must exceed filenumber-limit
 */
vector<string> KVStore::select_sstable_path(int level, int filenumber, int limit)
{
	vector<string> path_vec;
	string sstable_file_path;

	int selected_num = 0;

	string level_path = sstable_base_dir + "/level" + to_string(level) + "/";

	for (auto it = level_sstable_num[level].begin(); it != level_sstable_num[level].end(); it++) {
		if (level != 0 && selected_num >= filenumber - limit) {
			break;
		}
		++selected_num;
		string path = level_path + to_string(*it) + ".sst";
		path_vec.push_back(path);
	}
	return path_vec;
}

/**
 * Get all sstable id of all sstables in this level
 */
vector<int> KVStore::get_level_sstable_num(int level)
{
	if (level >= level_sstable_num.size()) {
		return vector<int>();
	}
	return vector<int>(level_sstable_num[level].begin(), level_sstable_num[level].end());
}