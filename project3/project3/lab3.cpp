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
	for (int i = 0; i < mat.size(); ++i) {
		for (int j = 0; j < mat[0].size(); ++j) {
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
		for (int i = 0; i < reversed_answer.size(); ++i) {
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
	vector< vector<int> > allEdges;
	for (int i = 0; i <= n; ++i) {
		vector<int> temp;
		allEdges.push_back(temp); 
	}

	for (int i = 0; i < edges.size(); i += 2) {
		int start = edges[i];
		int end = edges[i+1];
		allEdges[start].push_back(end);
		allEdges[end].push_back(start);
	}

	for (int i = 0; i < allEdges.size(); ++i) {
		std::sort(allEdges[i].begin(), allEdges[i].end());
	}

	double dp[hp+1][n+1];

	for (int i = 0; i <= hp; ++i) {
		for (int t = 0; t <= n; ++t) {
			dp[i][t] = 0;
		}
	}

	dp[hp][1] = 1;

	vector<int> noDamagePoints, damagePoints;
	for (int i = 0; i < damage.size(); ++i) {
		if (damage[i] == 0) {
			noDamagePoints.push_back(i + 1);
		} else {
			damagePoints.push_back(i + 1);
		}
	}

	for (int end : allEdges[1]) {
		if (damage[end-1] > 0) {
			dp[hp-damage[end-1]][end] = 1.0 / allEdges[1].size();
		}
	}

	for (int cur_hp = hp - 1; cur_hp >= 0; --cur_hp) {
		vector< vector<double> > mat;
		
		for (int i = 0; i < noDamagePoints.size(); ++i) {

			vector<double> row;

			// noDamagePoints variables + 1 constant
			for (int i = 0; i <= noDamagePoints.size(); ++i)
			{
				row.push_back(0);
			}
			row[i] = -1;

			int p = noDamagePoints[i];
			vector<int> edges = allEdges[p];
			
			for (int end : edges) {
				// find
				int idx = -1;
				for (int k = 0; k < noDamagePoints.size(); ++k) {
					if (noDamagePoints[k] == end) {
						idx = k;
						break;
					}
				}

				if (idx != -1) {
					row[idx] += 1.0 / allEdges[end].size();
				} else {
					row[noDamagePoints.size()] -= 1.0 / allEdges[end].size() * dp[cur_hp][end];
				}
			}
			mat.push_back(row);
		}

		vector<double> ans = gauss_jordan(mat);
		for (double n : ans) {
			cout << n << endl;
		}

		for (int i = 0; i < noDamagePoints.size(); ++i) {
			dp[cur_hp][noDamagePoints[i]] = ans[i];
		}

		for (int k : damagePoints) {
			for (int end : allEdges[k]) {
				dp[cur_hp-damage[end-1]][end] += 1.0 / allEdges[end].size() * dp[cur_hp][end];
			}
		}
	}

	double ans = 0;
	for (int i = 1; i < n; ++i) {
		ans += dp[0][i];
	}

	cout << ans << endl;
	return ans;
}