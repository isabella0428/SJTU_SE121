#pragma once

#include <cstdint>
#include <string>
#include <vector>
using namespace std;


/**
 * This file declare the main class of Project2:DijkstraProject2.
 * You should implement the methods:`readFromFile`,`run1`and`run2` in Dijkstra.cpp.
 * You can add anything in DijkstraProject2 class to implement Project2.
 */
class DijkstraProject2 {
private:
	//You can declare your graph structure here.
	// dist/parent{1, 2} for run1 and run2
	vector< vector< vector<int> > > all_edges;	// all_edges[i] should be the edge array for different test cases
	vector<int> all_node_num;
	int test_case_num;

public:

	/**
	 * Read graph from Param:`inputfile`.
	 * 
	 */
	void readFromFile(const char* inputfile="input.txt");
	
	/**
	 * Part 1, implement Dijkstra algorithm to finish Part 1
	 * and save the result to Param:`outputFile`.
	 * Save the path as: node_1,node_2...node_n. (seperate nodes with comma)
	 *
	 */
	void run1(int testcase, const char* outputFile);

	/**
	 * Part 2, find the monotonically increasing path to finish Part 2
	 * and save the result to Param:`outputFile`.
	 * Save the path as: node_1,node_2...node_n. (seperate nodes with comma)
	 *
	 */
	void run2(int testcase, const char* outputFile);

	/**
	 * Integrate run1 and run2
	 */
	void run(const char *outputFile = "output.txt");

	/**
	 * Helper function to split the string with ',' and return in the form of vector
	 */
	vector<int> getNumberFromString(string s);



	/**
	 * Print path to outputFile, using parent array
	 */
	void printPathToFile(const char *outputFile, 
		int min_dist, vector< vector<int> > &all_paths, bool is_run1);

	/**
	 * DFS to return the path with parent array
	 */
	vector< vector<int> > dfs(vector< vector<int> > &parent, int src, int dst);

	/**
	 *  DFS helper function to get path with parent array
	 */
	void dfs(vector< vector<int> > &parent, vector<int> temp,
				vector< vector<int> > &all_paths, int cur_node, int src);

	struct Edge {
		int start;
		int end;
		int value;
		Edge(int s, int e, int v):start(s),end(e), value(v) {}
	};
};
