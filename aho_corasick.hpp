#ifndef AHO_CORASICK_HPP
#define AHO_CORASICK_HPP

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace aho_corasick
{
	class trie
	{
		typedef std::pair<int, int> state;

	public:
		trie(const std::vector<std::string>& keys, const std::vector<int>& health)
		{
			auto getRealLength = [](const auto& arr) {
				int ret = 0;
				for(const auto& s : arr)
					ret += s.size();
				return ret;
			};
		}
		~trie();

	private:
		static const int MAXC = 26;
		static const char lowesctChar = 'a', highestChar = 'z';

		const int keyCount, MAXS;

		int states;
		state* matchingMachine;
	};
};

#endif // AHO_CORASICK_HPP