#include <iostream>
#include "aho_corasick.hpp"

namespace ac = aho_corasick;

std::pair<std::vector<std::string>, std::vector<int> > condense(const std::vector<std::string>& dict, const std::vector<int>& health, int first, int last)
{
	std::vector<std::string> ndict;
	std::vector<int> nhealth;
	int p = first;
	int q = last;
	for(; p < q+1; ++p) {
		auto it = std::find(ndict.begin(), ndict.end(), dict[p]);
		if(it != ndict.end())
		{
			nhealth[std::distance(ndict.begin(), it)] += health[p];
		}
		else
		{
			ndict.push_back(dict[p]);
			nhealth.push_back(health[p]);
		}
	}

	return (std::pair<std::vector<std::string>, std::vector<int> >(ndict, nhealth));
}

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

	int max = -1;
	int min = -1;
	std::cin >> k;
	for(i = 0; i < k; ++i)
	{
		std::cin>>p>>q;
		ac::trie actree;
		int total = 0;
		auto nvecs = condense(svec, health, p, q);
		for(q = 0; q < nvecs.first.size(); ++q)
			actree.insert(nvecs.first[q], nvecs.second[q]);
		std::string s;
		std::cin >> s;
		auto em_col = actree.parse_text(s);
		for(const auto& e : em_col)
			total += e.get_health();
		if(min == -1 || total < min)
			min = total;
		if(max == -1 || total > max)
			max = total;
	}

	std::cout << min << ' ' << max;
	return 0;
}