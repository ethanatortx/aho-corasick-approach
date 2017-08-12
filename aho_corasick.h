#ifndef __AHO_CORASICK_H__
#define __AHO_CORASICK_H__

#include <iostream>
#include <string>
#include <vector>

namespace aho_corasick
{
	class node
	{
	public:
		node();
		node(char);
		~node();

		char color() const;
		void color(const char&);

		char name() const;
		void name(const char&);

		friend node* createGraph(const std::vector<std::string>&);
		friend void printGraph(node*);
		friend void deleteGraph(node*);
	private:
		char _color, _name;
		std::vector<node*> black;
		std::vector<node*> blue;
		std::vector<node*> green;
	};

	node* createGraph(const std::vector<std::string>&);
	void printGraph(node*);
	void deleteGraph(node*);
};

#endif //end __AHO_CORASICK_H__