#include "Dijkstra.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
/**
 * You should implement the methods:`readFromFile`,`run1`and`run2` here.
 */
void DijkstraProject2::readFromFile(const char* inputfile)
{
	std::cout << "readFromFile: " << inputfile << std::endl;
	
	ifstream in(inputfile);
	if (!in.is_open()) {
		cerr << "Failed to open " << inputfile << endl;
		return;
	}

	vector<int> vec;
	string line;

	do {

		// Read node number and edge number
		getline(in, line);

		vec = getNumberFromString(line);

		int node_num = vec[0];
		int edge_num = vec[1];
		all_node_num.push_back(node_num);

		vector< vector<int> > edges;

		// Initialize dist
		for (int i = 0; i < node_num; ++i)
		{
			vector<int> temp;
			for (int j = 0; j < node_num; ++j)
			{
				temp.push_back(INT_MAX);
			}
			edges.push_back(temp);
		}

		// Read edges
		for (int i = 0; i < edge_num; ++i)
		{
			getline(in, line);
			vec = getNumberFromString(line);
			edges[vec[0]][vec[1]] = vec[2];
			edges[vec[0]][vec[1]] = vec[2];
		}

		all_edges.push_back(edges);
	} while(getline(in, line));

	test_case_num = all_node_num.size();
	
	in.close();
}

void DijkstraProject2::run(const char *outputFile)
{
	ofstream out;
	out.open(outputFile);
	out.close();

	for (int t = 0; t < test_case_num; ++t)
	{
		run1(t, outputFile);
		run2(t, outputFile);
	}
}

void DijkstraProject2::run1(int testcase, const char *outputFile)
{
	std::cout << "Save result to file:" << outputFile << std::endl;

	vector< vector<int> > parent;
	vector<int> dist;
	
	int node = all_node_num[testcase];
	vector< vector<int> > edges = all_edges[testcase];

	// Initialize parent and dist
	for (int i = 0; i < node; ++i)
	{
		vector<int> temp;
		temp.push_back(-1);
		parent.push_back(temp);
		dist.push_back(edges[0][i]);
	}

	int src = 0; // Setting given in pdf
	int dst = node-1;

	// Define visited vector
	vector<bool> visited;
	for (int i = 0; i < node; ++i)
	{
		visited.push_back(false);
	}
	visited[src] = true;

	int nodes_to_visit = node - 1;

	while (nodes_to_visit > 0)
	{
		vector<int> min_idx;
		int min_dist = INT_MAX;

		for (int i = 0; i < node; ++i)
		{
			if (visited[i] || dist[i] == INT_MAX || dist[i] > min_dist)
			{
				continue;
			}

			if (dist[i] == min_dist)
			{
				min_idx.push_back(i);
			}
			else
			{
				vector<int> temp;
				temp.push_back(i);
				min_idx = temp;
				min_dist = dist[i];
			}
		}


		for (int i = 0; i < min_idx.size(); ++i)
		{
			int n = min_idx[i];
			if (parent[n][0] == -1)
			{
				parent[n][0] = src;
			}
			visited[n] = true;
			nodes_to_visit--;

			for (int j = 0; j < node; ++j)
			{
				if (!visited[j] && edges[n][j] != INT_MAX && dist[j] >= dist[n] + edges[n][j])
				{
					if (dist[j] == dist[n] + edges[n][j])
					{
						parent[j].push_back(n);
					}
					else
					{
						vector<int> temp;
						temp.push_back(n);
						parent[j] = temp;
						dist[j] = dist[n] + edges[n][j];
					}
				}
			}
		}
	}

	vector< vector<int> > all_paths = dfs(parent, src, dst);
	printPathToFile(outputFile, dist[dst], all_paths, true);
}

bool cmpAscending(DijkstraProject2::Edge e1, DijkstraProject2::Edge e2)
{
	return e1.value < e2.value;
}

bool cmpDescending(DijkstraProject2::Edge e1, DijkstraProject2::Edge e2)
{
	return e1.value > e2.value;
}

void DijkstraProject2::run2(int testcase, const char* outputFile)
{
	std::cout << "Save result to file:" << outputFile << std::endl;

	vector< vector<int> > parent;
	vector<int> dist;
	vector<bool> visited;

	int node_num = all_node_num[testcase];
	vector< vector<int> > edges = all_edges[testcase];

	vector<DijkstraProject2::Edge> sorted_edges;
	for (int i = 0; i < node_num; ++i) {
		for (int j = 0; j < node_num; ++j) {
			if (edges[i][j] < INT_MAX) {
				sorted_edges.push_back(Edge(i, j, edges[i][j]));
			}
		}
	}

	vector< vector<int> > all_paths;
	int min_dist = INT_MAX;

	for (int i = 0; i < 2; ++i) {
		// Ascending Path
		if (i == 0) {
			sort(sorted_edges.begin(), sorted_edges.end(), cmpAscending);
		} else {
			sort(sorted_edges.begin(), sorted_edges.end(), cmpDescending);
		}

		// Initialize parent and dist
		for (int i = 0; i < node_num; ++i)
		{
			vector<int> temp;
			temp.push_back(-1);
			parent.push_back(temp);
			dist.push_back(INT_MAX);
			visited.push_back(false);
		}

		int src = 0;
		int dst = node_num - 1;

		int nodes_to_visit = node_num - 1;
		visited[src] = true;
		dist[src] = 0;

		for (int i = 0; i < sorted_edges.size(); ++i)
		{
			DijkstraProject2::Edge e = sorted_edges[i];
			int start = e.start;
			int end = e.end;
			if (visited[start])
			{
				if (dist[start] + e.value <= dist[end])
				{
					if (dist[start] + e.value == dist[end])
					{
						parent[end].push_back(start);
					}
					else
					{
						vector<int> par;
						par.push_back(start);
						parent[end] = par;
					}
					dist[end] = dist[start] + e.value;
				}
				visited[end] = true;
			}
		}

		vector< vector<int> > cur_path = dfs(parent, src, dst);

		if (dist[dst] < min_dist)
		{
			all_paths = cur_path;
			min_dist = dist[dst];
		}
	}

	printPathToFile(outputFile, min_dist, all_paths, false);
	
}

vector<int> DijkstraProject2::getNumberFromString(const string s) {
	string line, num_str;
	vector<int> nums;

	for (int i = 0; i < s.length(); ++i) {
		if (s[i] != ',') {
			num_str += s[i];
		} else {
			nums.push_back(stoi(num_str));
			num_str = "";
		}
	}
	nums.push_back(stoi(num_str));
	return nums;
}

vector< vector<int> > DijkstraProject2::dfs(vector< vector<int> > &parent, int src, int dst) {
	vector< vector<int> > all_paths;
	vector<int> temp;

	temp.push_back(dst);
	dfs(parent, temp, all_paths, dst, src);
	return all_paths;
}

void DijkstraProject2::printPathToFile(const char* outputFile, int min_dist, 
		vector< vector<int> > &all_paths, bool is_run1) {
	ofstream out(outputFile, ios::app);
	if (!out.is_open()) {
		cerr << "Failed to open " << outputFile << endl;
		return;
	}

	// First line: Min path length
	out << min_dist << endl;
	// Second line: Path number
	out << all_paths.size() << endl;

	for (int i = 0; i < all_paths.size(); ++i) {
		vector<int> path = all_paths[i];
		for (int i = path.size() - 1; i >= 0; --i) {
			if (i != 0) {
				out << path[i] << ",";
			} else {
				out << path[i] << endl;
			}
		} 
	}

	if (!is_run1) {
		out << "end" << endl;
	}

	out << endl;
	out.close();
}

void DijkstraProject2::dfs(vector< vector<int> > &parent, vector<int> temp, 
		vector< vector<int> > &all_paths, int cur_node, int src) {
	if(cur_node == src) {
		all_paths.push_back(temp);
		return;
	}

	for (int i = 0; i < parent[cur_node].size(); ++i) {
		int n = parent[cur_node][i];
		temp.push_back(parent[cur_node][i]);
		dfs(parent, temp, all_paths, n, src);
		temp.pop_back();
	}
}