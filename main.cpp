#include "aho_corasick.h"

namespace ac = aho_corasick;

int main()
{
	std::vector<std::string> svec = {
		"a", "ab", "bab", "bc", "bca", "c", "caa"
	};
	ac::node* n = ac::createGraph(svec);
	ac::printGraph(n);
	ac::deleteGraph(n);
	return 0;
}