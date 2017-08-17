#ifndef AHO_CORASICK_HPP
#define AHO_CORASICK_HPP

#include <iostream>

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <memory>
#include <queue>
#include <string>
#include <vector>

namespace aho_corasick
{

	class trie
	{
		// type definitions

	public:
		// generate variables and construct arrays
		trie(const std::vector<std::string>& keys, const std::vector<int>& health) {
			KCOUNT = keys.size();

			auto functor = [](const auto& k) {
				int len = 0;
				for(const auto& i : k)
					len += i.size();
				return len;
			};

			MAXS = functor(keys);

			f = std::vector<int>(MAXS, -1);
			out = std::vector<int>(MAXS, 0);
			h = std::vector<int>(MAXS, 0);
			g = std::vector<std::vector<int> >(MAXS, std::vector<int>(MAXC, -1));

			states = 1;

			for(auto i = keys.begin(); i != keys.end(); ++i) {
				int dist = std::distance(keys.begin(), i);
				int cur = 0;

				for(auto j = (*i).begin(); j != (*i).end(); ++j) {
					int c = (*j) - lowestChar;
					if(g[cur][c] == -1) g[cur][c] = states++;

					cur = g[cur][c];
				}

				h[cur] += health[dist];
				out[cur] |= (1 << dist);
			}
			for (int c = 0; c < MAXC; ++c)
		    {
		        if (g[0][c] == -1)
		        {
		            g[0][c] = 0;
		        }
		    }
			// build fail function
			std::queue<int> q;
			for (int c = 0; c <= highestChar - lowestChar; ++c)
		    {
		        if (g[0][c] != -1 and g[0][c] != 0)
		        {
		            f[g[0][c]] = 0;
		            q.push(g[0][c]);
		        }
		    }

			while(q.size())
			{
				int state = q.front();
				q.pop();

				for (int c = 0; c <= highestChar - lowestChar; ++c)
		        {
		            if (g[state][c] != -1)
		            {
		                int failure = f[state];
		                while (g[failure][c] == -1)
		                {
		                    failure = f[failure];
		                }
		                failure = g[failure][c];
		                f[g[state][c]] = failure;
		                out[g[state][c]] |= out[failure];
		                q.push(g[state][c]);
		            }
		        }
			}
		}


		int check(const std::string& s) {
			int cur = 0;
			int health = 0;
			for(const auto& c : s)
			{
				cur = go(cur, c);
				if(out[cur] == 0)
					continue;
				for(int i = 0; i < KCOUNT; ++i) {
					if(out[cur] & (1 << i)) {
						health += h[cur];
					}
				}
			}

			return health;
		}

		~trie() {}

	private:
		int go(int cur, char next) {
			int ans = cur;
			int c = next - lowestChar;
			while(g[ans][c] == -1)
				ans = f[ans];
			return g[ans][c];
		}

		static const int MAXC = 26;
		static const char lowestChar = 'a', highestChar = 'z';
		int MAXS;
		int KCOUNT;
		
		int states;

		std::vector<int> f;
		std::vector<int> out;
		std::vector<int> h;
		std::vector<std::vector<int> > g;
	};
};


#endif // AHO_CORASICK_HPP