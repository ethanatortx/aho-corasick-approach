#ifndef __AHO_CORASICK_H__
#define __AHO_CORASICK_H__

#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace aho_corasick
{
	template<
		range alphaHandler = lowerCase>
	class trie
	{
	public:
		explicit trie();
		trie(const std::vector<std::string>&);

		~trie();

		void add(const std::string&);
		void add(const std::vector<std::string>&);

		// return the state entered from [q] by edge labeled [a], calls fail if no edge is found
		int go(int q, char a);
		// gets the node labeled by the longest proper suffix [w] on path [L] so that w is a prefix of some pattern
		void fail(int q);
		// output set of all patterns recognized when entering state [q]
		std::set<std::string>& out(int q);

	private:
		// node (state) count
		int states;
	};

	class range {
	public:
		int lower() const=0;
		int upper() const=0; };

	class lowerCase : range
	{
		static char first = 'a', last = 'z';
	public:
		static inline int lower() const { return first; }
		static inline int upper() const { return last; }
	};
};

#endif //end __AHO_CORASICK_H__