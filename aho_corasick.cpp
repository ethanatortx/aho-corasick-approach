#include "aho_corasick.h"

aho_corasick::node::node():
	_color('g'), _name(' ') {}
aho_corasick::node::node(char c):
	_color('g'), _name(c) {}
aho_corasick::node::~node()
	{}

inline char aho_corasick::node::color() const { return this->_color; }
inline void aho_corasick::node::color(const char& c) { this->_color = c; }

inline char aho_corasick::node::name() const { return this->_name; }
inline void aho_corasick::node::name(const char& c) { this->_name = c; }

aho_corasick::node* aho_corasick::createGraph(const std::vector<std::string>& dict)
{
	node *root, *current, *temp;
	root = new node();
	for(std::vector<std::string>::const_iterator i = dict.cbegin(); i != dict.cend(); ++i)
	{
		current = root;
		for(char c : (*i))
		{
			if(current->black.size() > 0){
				std::vector<node*>::iterator black_it = current->black.begin();
				while((black_it != current->black.end()) && ((*black_it)->name() != c))
					++black_it;
				if(black_it == current->black.end())
				{
					temp = new node(c);
					current->black.push_back(temp);
					current = temp;
				}
				else
				{
					current = *black_it;
				}
			}
			else
			{
				temp = new node(c);
				current->black.push_back(temp);
				current = temp;
			}

		}

		if(current->color() != 'b')
			current->color('b');
	}

	return root;
}

// print character before traversal
void aho_corasick::printGraph(aho_corasick::node* root)
{
	std::cout << root->name() << ' ' << root->color() << '\n';

	if(root->black.size() > 0)
		for(std::vector<node*>::const_iterator i = root->black.begin(); i != root->black.end(); ++i)
			printGraph(*i);
}

void aho_corasick::deleteGraph(aho_corasick::node* root)
{
	if(root->black.size() > 0)
		for(std::vector<node*>::const_iterator i = root->black.begin(); i != root->black.end(); ++i)
			deleteGraph(*i);

	delete root;
}