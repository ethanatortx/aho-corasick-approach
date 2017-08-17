#include <iostream>
#include "aho_corasick.hpp"

int main()
{
	std::vector<std::string> svec = { "he", "she", "his", "he", "hers" };
	std::vector<int> health = { 1, 2, 3, 4, 5 };

	auto t = aho_corasick::trie(svec, health);

	std::cout << t.check("hehe");

	return 0;
}