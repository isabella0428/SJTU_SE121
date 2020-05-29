#include "Dijkstra.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <climits>

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

		vector< vector<endEdge> > edges;

		for (int i = 0; i < node_num; ++i) {
			vector<endEdge> temp;
			edges.push_back(temp);
		}

		// Read edges
		for (int i = 0; i < edge_num; ++i)
		{
			getline(in, line);
			vec = getNumberFromString(line);
			edges[vec[0]].push_back(endEdge(vec[1], vec[2]));
		}

		all_edges.push_back(edges);
	} while (getline(in, line));

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
	vector< vector<endEdge> > edges = all_edges[testcase];

	int src = 0; 			// Setting given in pdf
	int dst = node - 1;

	// Initialize parent and dist
	for (int i = 0; i < node; ++i)
	{
		vector<int> temp;
		temp.push_back(-1);
		parent.push_back(temp);
		dist.push_back(INT_MAX);
	}
	dist[src] = 0;

	priority_queue<nodeDist, vector<nodeDist>, Cmp1> q;
	q.push(nodeDist(src, 0));

	while(!q.empty()) {
		nodeDist nd = q.top();
		q.pop();

		int n = nd.node;
		int dt = nd.dist;

		for (endEdge ee : edges[n]) {
			int edge_val = ee.value;

			if (dist[ee.end] >= dist[n] + edge_val) {
				if (dist[ee.end] == dist[n] + edge_val) {
					parent[ee.end].push_back(n);
				} else {
					vector<int> temp;
					temp.push_back(n);
					parent[ee.end] = temp;
					dist[ee.end] = dist[n] + edge_val;
				}
				q.push(nodeDist(ee.end, dist[ee.end]));
			}
		}
	}

	vector< vector<int> > all_paths = dfs(parent, src, dst);
	printPathToFile(outputFile, dist[dst], all_paths, true);
}

void DijkstraProject2::run2(int testcase, const char* outputFile)
{
	std::cout << "Save result to file:" << outputFile << std::endl;

	int node_num = all_node_num[testcase];
	vector< vector<endEdge> > edges = all_edges[testcase];

	vector< vector<int> > all_paths;
	int min_dist = INT_MAX;

	for (int ascend = 0; ascend < 2; ++ascend) {
		vector<int> dist;
		vector< vector<int> > parent;

		// Initialize parent and dist
		for (int i = 0; i < node_num; ++i)
		{
			vector<int> temp;
			temp.push_back(-1);
			parent.push_back(temp);
			dist.push_back(INT_MAX);
		}

		if (ascend == 1) {
			for (int j = 0; j < node_num; ++j) {
				std::sort(edges[j].begin(), edges[j].end(), cmpDescending);
			}
		} else {
			for (int j = 0; j < node_num; ++j) {
				std::sort(edges[j].begin(), edges[j].end(), cmpAscending);
			}
		}

		int src = 0;
		int dst = node_num - 1;

		dist[src] = 0;
		priority_queue<nodeDistVal, vector<nodeDistVal>, Cmp2> pq;

		// Add edges from src to its nearby node into the queue
		for (int i = 0; i < edges[src].size(); ++i) {
			endEdge ee = edges[src][i];
			int node = ee.end;
			int val = ee.value;

			// Update distance
			dist[node] = val;

			// Update parent
			vector<int> temp;
			temp.push_back(src);
			parent[node] = temp;

			pq.push(nodeDistVal(node, dist[node], val));
		}

		while(pq.size() > 0) {
			nodeDistVal ndv = pq.top();
			pq.pop();

			int node = ndv.node;
			int dt = ndv.dist;
			int edge_val = ndv.val;

			if (ascend == 1) {
				for (int j = 0; j < edges[node].size(); ++j) {
					endEdge ee = edges[node][j];
					if (ee.value <= edge_val) {
						break;
					}

					if (dist[node] + ee.value <= dist[ee.end])
					{
						if (dist[node] + ee.value == dist[ee.end])
						{
							parent[ee.end].push_back(node);
						}
						else
						{
							vector<int> par;
							par.push_back(node);
							parent[ee.end] = par;
						}
						dist[ee.end] = dist[node] + ee.value;
						pq.push(nodeDistVal(ee.end, dist[ee.end], ee.value));
					}
				}
			} else {
				for (int j = 0; j < edges[node].size(); ++j)
				{
					endEdge ee = edges[node][j];
					if (ee.value >= edge_val)
					{
						break;
					}

					if (dist[node] + ee.value <= dist[ee.end])
					{
						if (dist[node] + ee.value == dist[ee.end])
						{
							parent[ee.end].push_back(node);
						}
						else
						{
							vector<int> par;
							par.push_back(node);
							parent[ee.end] = par;
						}
						dist[ee.end] = dist[node] + ee.value;
					}
				}
			}
		}

		if (dist[dst] != INT_MAX && dist[dst] <= min_dist) {
			vector< vector<int> > cur_paths = dfs(parent, src, dst);
			if (dist[dst] == min_dist) {
				for (vector<int> &temp : cur_paths) {
					if (find(all_paths.begin(), all_paths.end(), temp) == all_paths.end())
						all_paths.push_back(temp);
				}
			} else {
				all_paths = cur_paths;
				min_dist = dist[dst];
			}
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