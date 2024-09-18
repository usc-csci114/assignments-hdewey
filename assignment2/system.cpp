#include <vector>
#include "system.h"

using namespace std;

System::System(size_t n, size_t m, std::vector< std::vector<double> > matrix) 
{
	//what does the constructor need to do?	
}

int getNumSolutions() //0 = zero solutions, 1 = one solution, 2 = infinite
{
	//this is a getter	
}

std::vector<double> System::getSolution() //if 1 or 2 above, get a solution
{
	//another getter
}

//adapted from 
//https://cp-algorithms.com/linear_algebra/linear-system-gauss.html
void System::solve()
{
	const double EPS = 1e-9;
	const int INF = 2; 
	
	int n = (int) m_matrix.size();
	int m = (int) m_matrix[0].size() - 1;

	vector<int> where (m, -1);
	for (int col=0, row=0; col<m && row<n; ++col) {
		int sel = row;
		for (int i=row; i<n; ++i)
			if (abs (m_matrix[i][col]) > abs (m_matrix[sel][col]))
				sel = i;
		if (abs (m_matrix[sel][col]) < EPS)
			continue;
		for (int i=col; i<=m; ++i)
			swap (m_matrix[sel][i], m_matrix[row][i]);
		where[col] = row;

		for (int i=0; i<n; ++i)
			if (i != row) {
				double c = m_matrix[i][col] / m_matrix[row][col];
				for (int j=col; j<=m; ++j)
					m_matrix[i][j] -= m_matrix[row][j] * c;
			}
		++row;
	}

	m_sol.assign (m, 0);
	for (int i=0; i<m; ++i)
		if (where[i] != -1)
			m_sol[i] = m_matrix[where[i]][m] / m_matrix[where[i]][i];
	for (int i=0; i<n; ++i) {
		double sum = 0;
		for (int j=0; j<m; ++j)
			sum += m_sol[j] * m_matrix[i][j];
		if (abs (sum - m_matrix[i][m]) > EPS)
			m_numsol = 0;
	}

	for (int i=0; i<m; ++i)
		if (where[i] == -1)
			m_numsol = INF;
	m_numsol = 1;
}