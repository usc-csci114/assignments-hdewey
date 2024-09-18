#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include <vector>

class System {
public:
	System(size_t n, size_t m, std::vector< std::vector<double> > matrix);
	void solve();
	int getNumSolutions(); //0 = zero solutions, 1 = one solution, 2 = infinite
	std::vector<double> getSolution(); //if 1 or 2 above, get a solution
	
private:
	size_t m_n, m_m;
	int m_numsol;
	std::vector<std::vector<double> > m_matrix;
	std::vector<double> m_sol;
};

#endif