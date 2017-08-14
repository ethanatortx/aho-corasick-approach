#include <iostream>
#include "aho_corasick.hpp"

namespace ac = aho_corasick;

int main()
{
	
	int n, i, k, p, q;
	std::string t;
	std::cin >> n;
	std::vector<std::string> svec;
	std::vector<int> health;

	for(i = 0; i < n; ++i)
	{
		std::cin >> t;
		svec.push_back(t);
	}
	for(i = 0; i < n; ++i)
	{
		std::cin >> p;
		health.push_back(p);
	}

	std::cin >> k;
	for(i = 0; i < k; ++i)
	{
		std::cin>>p>>q;
		ac::trie actree;
		for(; p < (q+1); ++p)
			actree.insert(svec[p]);
		std::string s;
		std::cin >> s;
		auto em_col = actree.parse_text(s);
		for(const auto& e : em_col)
			std::cout << e.get_keyword() << ' ';
		std::cout << '\n';
	}
	return 0;
}