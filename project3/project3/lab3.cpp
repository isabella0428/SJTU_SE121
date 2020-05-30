#include "common.h"
#include "stdlib.h"
#include <algorithm>
#include <cstring>
#include <vector>
using namespace std;

//You should only code here. Don't edit any other files in this 
int func1(int amount, vector<int>& coins)
{	
	int dp[amount + 1];
	for (int i = 0; i <= amount; ++i) {
		dp[i] = 0;
	}

	dp[0] = 1;
	
	for (int m : coins) {
		for (int t = m; t <= amount; ++t)
		{
			dp[t] += dp[t-m];
		}
	}

	return dp[amount];
}

int func2(int amount, vector< vector<int> >& conquer)
{
	int dp[amount][amount];
	memset(dp, 0, sizeof(dp));

	// Next two people can always compete
	for (int p = 0; p < amount; ++p) {
		dp[p][(p+1)%amount] = 1;
	}

	for (int delta = 1; delta < amount; ++delta) {
		for (int left = 0; left < amount; ++left) {
			int right = left + delta + 1;
			for (int p = (left + 1) % amount; p < right; ++p) {
				int cur_r = right % amount;
				int cur_p = p % amount;
				if (dp[left][cur_p] && dp[cur_p][cur_r] 
						&& (conquer[left][cur_p] || conquer[cur_r][cur_p])) {
					dp[left][cur_r] = 1;
				}
			}
		}
	}

	int cnt = 0;
	for (int i = 0; i < amount; ++i) {
		if (dp[i][i]) {
			++cnt;
		}
	}

	return cnt;
}

void swap(vector< vector<double> >& mat, int i, int j) {
	int col = mat[0].size();
	for (int c = 0; c < col; ++c) {
		double temp = mat[i][c];
		mat[i][c] = mat[j][c];
		mat[j][c] = temp;
	}
}

void printMat(vector< vector<double> > mat) {
	for (uint i = 0; i < mat.size(); ++i) {
		for (uint j = 0; j < mat[0].size(); ++j) {
			cout << mat[i][j] << " ";
		}
		cout << endl;
	}
}

vector<double> gauss_jordan( /* augumented matrix */ vector< vector<double> > mat) {
	int n_variable = mat[0].size() - 1;

	// First processing the ith variable
	for (int i = 0; i < n_variable; ++i) {
		// Find the largest value in ith column, sawp with the current index
		double maxVal = mat[i][i];
		int rowIndex = i;
		for (int j = i + 1; j < n_variable; ++j) {
			if (mat[j][i] > maxVal) {
				maxVal = mat[j][i];
				rowIndex = j;
			}
		}

		// Swap the column with the largest value in the ith variable
		swap(mat, i, rowIndex);

		// Let the coefficient of the ith variable to be 1
		for (int j = i; j <= n_variable; ++j) {
			mat[i][j] /= maxVal;
		}

		printMat(mat);
		for (int j = i + 1; j < n_variable; ++j) {
			int val = mat[j][i];
			// Here I do a small speedup: not clear the item, just modify the next variables
			for (int k = i + 1; k <= n_variable; ++k) {
				mat[j][k] -= val * mat[i][k];
			} 
			mat[j][i] = 0;
		}

		printMat(mat);
	}

	vector<double> reversed_answer;
	int lastIndex = mat[0].size();
	reversed_answer.push_back(mat[n_variable-1][lastIndex-1]);
	cout << mat[n_variable - 1][lastIndex - 1] << endl;

	for (int j = n_variable - 2; j >= 0; --j)
	{
		double ans = mat[j][lastIndex-1];
		for (uint i = 0; i < reversed_answer.size(); ++i) {
			ans -= mat[j][n_variable -i -1] * reversed_answer[i];
			cout << mat[j][n_variable - i-1] << endl;
			cout << reversed_answer[i] << endl;
		}
		reversed_answer.push_back(ans);
	}

	vector<double> answer;
	for (int i = reversed_answer.size() - 1; i >= 0; --i) {
		answer.push_back(reversed_answer[i]);
	}
	return answer;
}

double func3(int n,int hp,vector<int>& damage,vector<int>& edges) {
	// Redo edges
	vector<vector<int>> allEdges;
	// Record the degree
	vector<double> degree(n + 1, 0.0);
	for (int i = 0; i <= n; ++i) {
		vector<int> temp;
		allEdges.push_back(temp); 
	}

	for (uint i = 0; i < edges.size(); i += 2) {
		int start = edges[i];
		int end = edges[i+1];
		// Insert edges
		allEdges[start].push_back(end);
		allEdges[end].push_back(start);
		// Increase degree
		degree[start]++;
		degree[end]++;
	}

	for (uint i = 0; i < allEdges.size(); ++i) {
		std::sort(allEdges[i].begin(), allEdges[i].end());
	}

	// Initialize coefficient array
	vector<vector<double>> cof(n, vector<double>(n, 0.0));
	vector<vector<double>> f(n, vector<double>(n, 0.0));
	// Initialize dp array
	vector<vector<double>> dp(hp + 1, vector<double>(n, 0.0));

	// First initialize the coefficient of the equation
	for (int i = 0; i < n; i++) {
		f[i][i] = 1.0;
		cof[i][i] = 1.0;
		// If there is no damage at this point
		if (damage[i] == 0.0) {
			int from = i + 1;
			// Get the edge from this point
			for (auto to : allEdges[from]) {
				// Not the terminal point
				if (to != n) {
					cof[i][to - 1] -= 1.0 / degree[to];
				}
			}
		}
	}


	// Now begin the gauss jordan algorithm
	for (int i = 0; i < n; ++i) {
		int maxId = i;
		for (int j = i + 1; j < n; ++j) {
			// Get the maxium index
			if (fabs(cof[j][i]) > fabs(cof[maxId][i])) {
				maxId = j;
			}
		}

		// Check maxId and i relationship
		if (maxId != i) {
			for (int j = i; j < n; ++j) {
				swap(cof[maxId][j], cof[i][j]);
			}
			for (int j = 0; j < n; ++j) {
				swap(f[maxId][j], f[i][j]);
			}
		}
		for (int j = 0; j < n; ++j) {
			// Only j != i case
			if (j != i) {
				double t = cof[j][i] / cof[i][i];
				for (int k = i; k < n; ++k) {
					cof[j][k] -= cof[i][k] * t;
				}
				for (int k = 0; k < n; ++k) {
					f[j][k] -= f[i][k] * t;
				}
			}
		}
	}
	// Last divide by cof matrix
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			f[i][j] /= cof[i][i];
		}
	}


	// Now let's begin the dp algorithm
	// Begin with the highest hp
	double sum = 0.0;
	for (int k = hp; k > 0; k--) {
		// Constant vector
		vector<double> constant(n, 0.0);
		// Highest hp (start from 0 node)=> constant[0] = 1.0;
		if (k == hp) {
			constant[0] = 1.0;
		}
		for (int i = 0; i < n; ++i) {
			// Have damage and not exceeds maximum hp
			if (damage[i] && hp >= k + damage[i]) {
				int from = i + 1;
				for (auto to : allEdges[from]) {
					// Not terminal point
					if (to != n) {
						constant[i] += dp[k + damage[i]][to - 1] / degree[to];
					}
				}
			}
		}
		// Solve the dp equations
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				dp[k][i] += constant[j] * f[i][j];
			}
		}
		// The average times of terminal points
		sum += dp[k][n - 1];
	}
	return sum;
}