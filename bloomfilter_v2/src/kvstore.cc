#include "../include/kvstore.h"
#include <filesystem>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <iostream>
#include "time.h"

using namespace std;

/**
 * Initializes the memory table
 */
KVStore::KVStore(const std::string &dir): KVStoreAPI(dir)
{
	// Initialize memtable
	memtable = new SkipList<uint64_t, string>();
	mem_limit = 2000000;
	current_size = 0;
	sstable_num = 0;
	sstable_base_dir = dir;

	// Clean directory
	if (filesystem::exists(this->sstable_base_dir))
	{
		filesystem::remove_all(this->sstable_base_dir);
		filesystem::create_directories(this->sstable_base_dir);
	}

	if (!filesystem::exists(this->sstable_base_dir)) {
		filesystem::create_directories(this->sstable_base_dir);
	}

	// For level file picking
	srand(time(NULL));
}

/**
 * Release all resources
 * --Clean all sstables
 * --Clear the memory table
 */
KVStore::~KVStore()
{
	this->memtable->clear();
	for (int i = 0; i < sstable_num; ++i) {
		all_sstable_bmfilter[i].clear();
	}
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
	if (this->current_size > this->mem_limit) {
		vector<Entry_time> key_value_vec;

		for (Entry<uint64_t, string> e : this->memtable->getAllElement()) {
			key_value_vec.push_back( 
				Entry_time(e._key, e._value, sstable_num));
		}
			
		// Store the elements to sstable
		if (!save_as_sstable(key_value_vec, 0, true)) {
			cout << "Failed to save as sstable!" << endl;
			return;
		}

		// Check if the number of elements in each level is okay
		if (!check_level()) {
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
	if (this->memtable->get(key, &value)) {
		return value;
	}

	uint64_t maxKey, minKey;
	int timestamp = -1, cur_timestamp;
	string cur_value;
	for (int i = 0; i < sstable_num; ++i) {
		bloomfilter bf = all_sstable_bmfilter[i];

		if (!bf.contains(key)) {
			continue;
		}

		// If the key is within the key_range of the sstable, do binary search
		// Since there may be multiple same keys with different timestamp
		// return the one with the biggest timestamp
		if (binary_search_sstable(all_sstable_index[i], key, &cur_value, i, cur_timestamp)) {
			if (cur_timestamp > timestamp) {
				timestamp = cur_timestamp;
				value = cur_value;
			}
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
	vector< std::pair<uint64_t, string> > vec;

	for (int k = key1; k <= key2; ++k) {
		string value = this->get(k);

		// The key exists!
		if (value.length() > 0) {
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
	if (this->get(key).length() <= 0) {
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
	if (filesystem::exists(this->sstable_base_dir)) {
		filesystem::remove_all(this->sstable_base_dir);
		filesystem::create_directories(this->sstable_base_dir);
	}
}

/**
 * This check if the sstable file number exceed the level limit
 * If exceeds, call compaction
 */
bool KVStore::check_level() {
	return check_level(0, level_limit(0));
}

/**
 * Helper funcition for check_level()
 * Check if the number of files in the given directory exceed the limit
 * If exceed, call compaction
 */
bool KVStore::check_level(int level, int limit) {
	string path = sstable_base_dir + "/" + "level" + to_string(level) + "/";

	if (!std::filesystem::exists(path))
		return false;

	// Count the number of files in the directory
	int file_number = 0;

	// Check if the files have the extension ".sst"
	regex basic_regex(".*\\.sst");
	smatch base_match;
	string file_name;
	for (auto &p : filesystem::directory_iterator(path))
	{
		file_name = p.path();
		if (regex_match(file_name, base_match, basic_regex))
			++file_number;
	}
	
	// If the file number exceed the limit, it should do compaction
	if (file_number > limit) {
		return compaction(level, file_number, limit);
	}
	
	return true;
}

/**
 * Do sstable compaction when the file number on this level exceeds
 * Combine the files with the files in the lower level when their key range overlap
 */
bool KVStore::compaction(int level, int filenumber, int limit) {
	// Get this level and  next level path
	// string format_string = sstable_base_dir + "/level%d/";
	string level_path = sstable_base_dir + "/level" + to_string(level) + "/";
	string next_level_path = sstable_base_dir + "/level" + to_string(level + 1) + "/";

	bool result = true;
	string target_path;

	// If the next level doesn't exist, create the directories and copy all files there
	if (!filesystem::exists(next_level_path)) {
		filesystem::create_directories(next_level_path);

		string file_path;
		for (auto &p : std::filesystem::directory_iterator(level_path))
		{
			file_path = p.path();
			target_path = next_level_path + to_string(get_file_id(file_path)) + ".sst";
			result &= filesystem::copy_file(file_path, target_path);
			filesystem::remove(file_path);
		}
		return result;
	}

	// Get the key range of the files of the lower layer
	uint64_t max_key = 0, min_key = 0xffffffffffffffff;
	vector<string> lower_layer_sstable_path = select_sstable_path(level, filenumber, limit);
	string sstable_file_path;

	for (auto iter = lower_layer_sstable_path.begin(); iter != lower_layer_sstable_path.end(); ++iter) {
		int id = get_file_id(*iter);

		// Get the key range of the sstable file from index in memory
		key_range cur_key_range = get_key_range(id);
		max_key = max_key > cur_key_range._max ? max_key : cur_key_range._max;
		min_key = min_key < cur_key_range._min ? min_key : cur_key_range._min;
	}

	// Find all files in the next level whose key ranges overlaps with this key range
	string extension = ".sst";
	vector<string> higher_level_sstable_path;
	key_range k_range;
	int file_id;

	for (auto &p : std::filesystem::directory_iterator(next_level_path))
	{
		sstable_file_path = p.path();
		file_id = get_file_id(sstable_file_path.c_str());
		k_range = get_key_range(file_id);

		if ((k_range._max < min_key) || (k_range._min > max_key)) {
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
bool KVStore::merge_files(vector<string> file1, vector<string> file2, int next_level) {
	// Timestamp of the newly merged sstables
	int new_timestamp = sstable_num;

	// Merge the key offset of file1 and file2 into a vector
	vector<Entry_time> merged;
	vector<Entry_time> sstable_content;
	for (string path : file1) {
		sstable_content = read_key_offset_from_file(path);
		merged = merge_two_files(merged, sstable_content);
		filesystem::remove(path);
	}

	if (file2.size() > 0) {
		for (string path : file2)
		{
			sstable_content = read_key_offset_from_file(path);
			merged = merge_two_files(merged, sstable_content);
			filesystem::remove(path);
		}
	}

	save_as_sstable(merged, next_level, false);

	return check_level(next_level, level_limit(next_level));
}

/**
 * Save MemTable as SSTable when the size of memtable exceed 2M
 * Each SSTable is at most 2M
 * Notice that when it is converted from memtable, it size can be a little higher than limit
 */
bool KVStore::save_as_sstable(
	vector<Entry_time> merged, int level, bool from_memtable)
{
	bool operation_result = true;
	int m_size = merged.size();

	while (merged.size() > 0) {
		// Get the file_path of the sstable we are going to create (under sstable_base_dir)
		string file_path = sstable_base_dir + "/" + to_string(sstable_num) + ".sst";

		// Create the file
		ofstream out;
		out.open(file_path);
		out.close();

		// Open the file in binary form
		out.open(file_path, ios::binary | ios::out); // Open the file in binary format

		if (!out.is_open())
			return false;

		uint64_t offset = 0;
		vector<key_offset> key_offset_vector;		// key value pair
		Entry_time cur_key_value;

		bloomfilter bf = bloomfilter(sstable_num);

		while ((offset <= this->mem_limit || from_memtable) && merged.size() > 0)
		{
			cur_key_value = merged[0];

			key_offset_vector.push_back(key_offset(cur_key_value._key, offset));

			// Write key
			out.write((char *)&(cur_key_value._key), sizeof(cur_key_value._key));
			offset += sizeof(cur_key_value._key);

			// Write value
			out.write((char *)&(cur_key_value._value[0]), cur_key_value._value.length());
			offset += cur_key_value._value.length();

			out.flush();
			merged.erase(merged.begin());

			// Add to bloomfilter
			bf.add(cur_key_value._key);
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

		// Store the index of the sstable in memory
		all_sstable_index.push_back(s); 

		// Store the bloomfilter to memory
		all_sstable_bmfilter.push_back(bf);

		// Copy this sst file to the given level
		string level_path = sstable_base_dir + "/level" + to_string(level) + "/";
		filesystem::path level_p(level_path);
		if (!exists(level_p))
			create_directories(level_p);

		string target_file = level_path + to_string(sstable_num++) + ".sst";
		operation_result &= std::filesystem::copy_file(filesystem::path(file_path), filesystem::path(target_file));
	}

	return operation_result;
}

/**
 *  Merge key-value pair
 */
vector<Entry_time> KVStore::merge_two_files(
	vector<Entry_time> file1, vector<Entry_time> file2)
{
	// Merged key value vector
	vector<Entry_time> merged;

	uint64_t key;
	int time, length;

	while(file1.size() > 0 && file2.size() > 0) {
		// Pick the smaller key first
		// If equal, pick the one with the newest timestamp
		if ((file1[0]._key < file2[0]._key) 
			|| ((file1[0]._key == file2[0]._key) && file1[0]._time > file2[0]._time)) {
			merged.push_back(Entry_time(file1[0]));
			file1.erase(file1.begin());
		} else {
			merged.push_back(Entry_time(file2[0]));
			file2.erase(file2.begin());
		}
	}

	while(file1.size() > 0) {
		merged.push_back(Entry_time(file1[0]));
		file1.erase(file1.begin());
	}

	while (file2.size() > 0){
		merged.push_back(Entry_time(file2[0]));
		file2.erase(file2.begin());
	}
	return merged;	
}

/**
 * Read key-value pair from disk given the fileid and the level it is in
 */
vector<Entry_time> KVStore::read_key_offset_from_file(string file_path)
{
	fstream in;
	in.open(file_path, ios::binary | ios::in);

	// Get file id
	int file_id = get_file_id(file_path);

	// Get certain key-offset index of the given file
	sstable_index index = all_sstable_index.at(file_id);
	vector< key_offset> v = index._vector;

	// Store all key value within this file
	vector<Entry_time> all_key_value;

	// Read all key value pair with timestamp from the sstable
	uint64_t key;
	char *value;
	int key_value_length;

	int a = v.size();
	for (int i = 0; i < v.size() - 1; ++i) {
		key_value_length = v[i + 1]._offset - v[i]._offset;
		value = new char[key_value_length - sizeof(uint64_t) + 1];

		in.read((char *)&key, 8);
		in.read(value, key_value_length - sizeof(uint64_t));
		value[key_value_length - sizeof(uint64_t)] = '\0';

		all_key_value.push_back(
			Entry_time(
				key,
				string(value),
				index._timestamp));

		delete []value;
	}
	
	// Read the last key_value
	in.read((char *)&key, 8);
	
	// Calculate the total length of the file since a binary file doesn't have eod
	int offset = v[v.size() - 1]._offset;
	in.seekg(0, ios::end);
	int length = in.tellg();
	int value_length = length - offset - 8;
	value = new char[value_length + 1];

	in.seekg(offset + 8, ios::beg);
	in.read(value, value_length);
	value[value_length] = '\0';

	all_key_value.push_back(
		Entry_time(key, string(value), index._timestamp));

	delete []value;

	in.close();
	return all_key_value;
}

/**
 * Get the key range of the specific sstable
 */
key_range KVStore::get_key_range(int file_id) {
	return key_range(
		all_sstable_index.at(file_id)._max, 
		all_sstable_index.at(file_id)._min);
}

/**
 * Get file id based on given file
 */
int KVStore::get_file_id(string filepath)
{
	int id = -1;
	int num_start = 0, start = 0;
	for (char c : filepath) {
		if (c == '/') {
			start = num_start + 1;
		}
		num_start++;
	}

	string num = "";
	for (int i = start; i < filepath.length(); ++i) {
		if (filepath[i] >= '0' && filepath[i] <= '9') {
			num += filepath[i];
		} else {
			break;
		}
	}
	return atoi(num.c_str());
}

bool KVStore::binary_search_sstable(
	const sstable_index &sstable_index, 
	uint64_t key, string *value, int sstable_id, int &timestamp) {

		// Binary Search
		vector<key_offset> vec = sstable_index._vector;
		int loc = -1;

		int low = 0, high = vec.size() - 1;
		while (low <= high) {
			int middle = (low + high) / 2;
			uint64_t middle_key = vec[middle]._key;
			
			if (middle_key == key) {
				loc = middle;
				break;
			}

			if (middle_key < key) {
				low = middle + 1;
			} else {
				high =  middle - 1;
			}
		}

		if (loc == -1) {
			return false;
		}

		// There may exist multiple same keys
		while(loc != vec.size() - 1 && vec[loc]._key == vec[loc + 1]._key) {
			loc++;
		}

		// Read Value from sstable
		string path = sstable_base_dir + "/" + to_string(sstable_id) + ".sst";
		fstream in;
		in.open(path, ios::binary | ios::in);

		int value_length;
		if (loc == vec.size() - 1) {
			in.seekg(0, ios::end);
			int len = in.tellg();
			value_length = len - vec[loc]._offset - 8;
		} else {
			value_length = vec[loc + 1]._offset - vec[loc]._offset - 8;
		}

		in.seekg(vec[loc]._offset + 8, ios::beg);

		char *binary_value;
		binary_value = new char[value_length + 1];
		binary_value[value_length] = '\0';
		in.read(binary_value, value_length);

		in.close();

		int s_len = string(binary_value).length();

		// Set value
		string s = "";
		s.append(binary_value);
		s_len = s.length();
		*value = s;
		char c = binary_value[value_length - 1];
		timestamp = sstable_index._timestamp;

		delete []binary_value;

		return true;
}


/**
 * Select sstables from the level which sstable numbers exceed the limit
 * If it is level0, choose all sstables
 * If not, randomly select files, but the number of sstables selected must exceed filenumber-limit
 */
vector<string> KVStore::select_sstable_path(int level, int filenumber, int limit) {
	vector<string> path_vec;
	string sstable_file_path;

	string level_path = sstable_base_dir + "/level" + to_string(level) + "/";
	for (auto &p : std::filesystem::directory_iterator(level_path))
	{
		// TODO: how to select files when it is not the first layer
		if (level != 0 && rand() % 3 != 1)
		{
			continue;
		}

		sstable_file_path = p.path();
		path_vec.push_back(sstable_file_path);
	}

	if (path_vec.size() < filenumber - limit) {
		for (auto &p : std::filesystem::directory_iterator(level_path))
		{
			sstable_file_path = p.path();
			if (find(path_vec.begin(), path_vec.end(), sstable_file_path) == path_vec.end())
			{
				path_vec.push_back(sstable_file_path);
				if (path_vec.size() >= filenumber - limit) {
					return path_vec;
				}
			}
		}
	}

	if(level > 0) {
		while (path_vec.size() > filenumber - limit)
			path_vec.erase(path_vec.begin());
	}
	return path_vec;
}