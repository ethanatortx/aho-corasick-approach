// https://github.com/cjgdev/aho_corasick/blob/master/src/aho_corasick/aho_corasick.hpp
#ifndef AHO_CORASICK_HPP
#define AHO_CORASICK_HPP

#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace aho_corasick
{
	// describes range
	class interval
	{
		size_t m_start;
		size_t m_end;

	public:
		interval(size_t start, size_t end)
			: m_start(start), m_end(end) {}

		inline size_t get_start() const { return m_start; }
		inline size_t get_end() const { return m_end; }
		size_t size() const { return m_end - m_start + 1; }

		bool overlaps_with(const interval& other) const
		{ return this->m_start <= other.m_end && this->m_end >= other.m_start; }
		
		bool overlaps_with(size_t point) const
		{ return this->m_start <= point && point <= this->m_end; }

		friend bool operator<(const interval& lhs, const interval& rhs);
		friend bool operator==(const interval& lhs, const interval& rhs);
		friend bool operator!=(const interval& lhs, const interval& rhs);
	};

	inline bool operator<(const interval& lhs, const interval& rhs)
		{ return lhs.m_start < rhs.m_start; }
	inline bool operator==(const interval& lhs, const interval& rhs)
		{ return lhs.m_start == rhs.m_start && lhs.m_end == rhs.m_end; }
	inline bool operator!=(const interval& lhs, const interval& rhs)
		{ return lhs.m_start != rhs.m_start || lhs.m_end != rhs.m_end; }

	// class interval_tree
	template<typename T>
	class interval_tree
	{
	public:
		using interval_collection = std::vector<T>;

	private:
		// class node
		class node
		{
			enum direction {
				LEFT, RIGHT
			};
			using node_ptr = std::unique_ptr<node>;

			size_t				m_point;
			node_ptr			m_left;
			node_ptr			m_right;
			interval_collection	m_intervals;

		public:
			// construct from interval vector
			node(const interval_collection& intervals):
				m_point(0),
				m_left(nullptr),
				m_right(nullptr),
				m_intervals()
			{
				// find middle of intervals to determine position to split input intervals
				m_point = determine_median(intervals);
				// define recepters for split intervals input
				interval_collection to_left, to_right;

				for(const auto& i : intervals) { // compare all intervals to midpoint
					if(i.get_end() < m_point)
						to_left.push_back(i);		// if the interval ends to the left of the midpoint, assign to left
					else if(i.get_start() > m_point)
						to_right.push_back(i);		// if the interval begins to the right of the midpoint, assign to right
					else
						m_intervals.push_back(i);	// if the interval contains the midpoint, assign to current node
				}

				// create new left and right nodes from split input intervals
				if(to_left.size() > 0)
					m_left.reset(new node(to_left));
				if(to_right.size() > 0)
					m_right.reset(new node(to_right));
			}

			// find the median interval in the input interval vector
			size_t determine_median(const interval_collection& intervals) const {
				int start = -1;
				int end = -1;
				// find max and min of interval vector
				for(const auto& i : intervals) {
					int c_start = i.get_start();
					int c_end = i.get_end();
					if(start == -1 || c_start < start)
						start = c_start;
					if(end == -1 || c_end > end)
						end = c_end;
				}
				// return median
				return (start + end)/2;
			}
			// find and return all intervals that overlap the input param
			interval_collection find_overlaps(const T& i) {
				interval_collection overlaps;
				if(m_point < i.get_start()) { 		// if input begins to the right of midpoint, add only this node's overlapps and the right node's overlapps
					add_to_overlaps(i, overlaps, find_overlapping_ranges(m_right, i));
					add_to_overlaps(i, overlaps, check_right_overlaps(i));
				} else if(m_point > i.get_end()) {	// if input ends to the left of midpoint, add only this node's overlapps and the left node's overlapps
					add_to_overlaps(i, overlaps, find_overlapping_ranges(m_left, i));
					add_to_overlaps(i, overlaps, check_left_overlaps(i));
				} else {							// add all overlapps
					add_to_overlaps(i, overlaps, m_intervals);
					add_to_overlaps(i, overlaps, find_overlapping_ranges(m_left, i));
					add_to_overlaps(i, overlaps, find_overlapping_ranges(m_right, i));
				}
				// return overlaps
				return interval_collection(overlaps);
			}


		protected:
			// add intervals from new_overlaps (excluding interval i) into interval collection passed by reference: overlaps
			void add_to_overlaps(const T& i, interval_collection& overlaps, interval_collection new_overlaps) const {
				for(const auto& c : new_overlaps)
					if(c != i)
						overlaps.push_back(c);
			}
			
			// check overlaps in LEFT with interval i
			inline interval_collection check_left_overlaps(const T& i) const
			{ return interval_collection(check_overlaps(i, LEFT)); }
			
			// check overlaps in RIGHT with interval i
			inline interval_collection check_right_overlaps(const T& i) const
			{ return interval_collection(check_overlaps(i, RIGHT)); }

			// check overlaps in either subtree with interval i
			interval_collection check_overlaps(const T& i, direction d) const {
				interval_collection overlaps;
				// loop through intervals of this node
				for(const auto& c : m_intervals) {
					// add overlaps based on case check on supplied direction
					switch(d) {
					case LEFT:
						if(c.get_start() <= i.get_end())
							overlaps.push_back(c);
						break;
					case RIGHT:
						if(c.get_end() >= i.get_start())
							overlaps.push_back(c);
						break;
					}
				}
				// return overlaps
				return interval_collection(overlaps);
			}
			// find overlaps with other node with interval i
			interval_collection find_overlapping_ranges(node_ptr& node, const T& i) const {
				// if node pointer is not NULL
				if(node) {
					return interval_collection(node->find_overlaps(i)); // return overlaps in other node with i
				}
				//otherwise return empty overlaps vector
				return interval_collection();
			}
		}; // end class node

		node m_root;

	public:
		// construct from interval vector
		interval_tree(const interval_collection& intervals)
			: m_root(intervals) {}

		// remove overlaps from supplies interval
		interval_collection remove_overlaps(const interval_collection& intervals) {
			// copy input interval
			interval_collection result(intervals.begin(), intervals.end());
			// sort copied interval collection
			std::sort(result.begin(), result.end(), [](const T& a, const T& b) -> bool { // this is my first time seeing a lambda in the wild, how cool!
					if(b.size() - a.size() == 0) {
						return a.get_start() > b.get_start();
					}
					return a.size() > b.size();
			});
			std::set<T> remove_tmp;
			// add overlaps to a set
			for(const auto& i : result) {
				if(remove_tmp.find(i) != remove_tmp.end()) {
					continue;
				}
				auto overlaps = find_overlaps(i);
				for(const auto& overlap : overlaps) {
					remove_tmp.insert(overlap);
				}
			}
			// remove items found in set from return interval collection
			for(const auto& i : remove_tmp) {
				result.erase(
					std::find(result.begin(), result.end(), i)
				);
			}
			// sort it again
			std::sort(result.begin(), result.end(), [](const T& a, const T& b) -> bool {
				return a.get_start() < b.get_start();
			});
			// return interval collection without overlaps
			return interval_collection(result);
		}

		// return overlaps with input interval found in root
		interval_collection find_overlaps(const T& i) {
			return interval_collection(m_root.find_overlaps(i));
		}
	}; // end class interval_tree

	// class emit
	template<typename charType>
	class emit: public interval
	{
	public:
		typedef std::basic_string<charType>		string_type;
		typedef std::basic_string<charType>&	string_ref_type;

	private:
		string_type	m_keyword;
		unsigned 	m_index = 0;

	public:
		emit():
			interval(-1, 1),
			m_keyword() {}

		emit(size_t start, size_t end, string_type keyword, unsigned index):
			interval(start, end),
			m_keyword(keyword), m_index(index) {}

		inline string_type get_keyword() const
			{ return string_type(m_keyword); }
		inline unsigned get_index() const
			{ return m_index; }
		bool is_empty() const
			{ return (get_start() == -1 && get_end() == -1); }
	}; // end class emit

	// class token
	template<typename charType>
	class token
	{
	public:
		enum token_type {
			TYPE_FRAGMENT,
			TYPE_MATCH,
		};

		using string_type 		= std::basic_string<charType>;
		using string_ref_type 	= std::basic_string<charType>&;
		using emit_type			= emit<charType>;

	private:
		token_type	m_type;		//
		string_type m_fragment;	//
		emit_type	m_emit;		//

	public:
		// construct as fragment
		constexpr token(string_ref_type fragment):
			m_type(TYPE_FRAGMENT),
			m_fragment(fragment),
			m_emit() {}

		// construct as match
		constexpr token(string_ref_type fragment, const emit_type& e):
			m_type(TYPE_MATCH),
			m_fragment(fragment),
			m_emit(e) {}

		inline bool is_match() const { return (m_type == TYPE_MATCH); }
		inline string_type get_fragment() const { return string_type(m_fragment); }
		inline emit_type get_emit() const { return m_emit; }
	}; // end class token

	// class state
	template<typename charType>
	class state
	{
	public:
		typedef state<charType>*					ptr;
		typedef std::unique_ptr<state<charType> > 	unique_ptr;
		typedef std::basic_string<charType>			string_type;
		typedef std::basic_string<charType>&		string_ref_type;
		typedef std::pair<string_type, unsigned>	key_index;
		typedef std::set<key_index>					string_collection;
		typedef std::vector<ptr>					state_collection;
		typedef std::vector<charType>				transition_collection;

	private:
		size_t							m_depth;	//
		ptr 							m_root;		//
		std::map<charType, unique_ptr> 	m_success;	//
		ptr 							m_failure;	//
		string_collection 				m_emits;	//

	public:
		constexpr state(): state(0) {}

		state(size_t depth):
			m_depth(depth),
			m_root(depth == 0 ? this : nullptr),
			m_success(),
			m_failure(nullptr),
			m_emits() {}

		inline ptr next_state(charType character) const
		{ return next_state(character, false); }

		inline ptr next_state_ignore_root_state(charType character) const
		{ return next_state(character, true); }

		// get next state for supplied character, or, if no edge exists, creates one from supplied character at depth + 1
		ptr add_state(charType character) {
			// goto next node with edge labeled with input character
			auto next = next_state_ignore_root_state(character);
			if(next == nullptr) {	// if no node with that edge leading to it exists
				// create a new node with this character at one greater depth
				next = new state<charType>(m_depth + 1);
				m_success[character].reset(next);
			}
			// return next state
			return next;
		}

		// return depth of current state
		inline size_t get_depth() const { return m_depth; }

		// adds single key index pair
		void add_emit(string_ref_type keyword, unsigned index) {
			m_emits.insert(std::make_pair(keyword, index));
		}
		// adds all key indexe pairs from set
		void add_emit(const string_collection& emits) {
			for(const auto& e : emits) {
				string_type str(e.first);
				add_emit(str, e.second);
			}
		}
		// return key index pair set
		inline string_collection get_emits() const { return m_emits; }
		// get failure node
		ptr failure() const { return m_failure; }
		// set failure node
		void set_failure(ptr fail_state) { m_failure = fail_state; }

		// get state collection
		state_collection get_states() const {
			state_collection result;
			for(auto it = m_success.cbegin(); it != m_success.cend(); ++it) {
				result.push_back(it->second.get());
			}
			return state_collection(result);
		}

		// return transition vector
		transition_collection get_transitions() const {
			transition_collection result;
			for(auto it = m_success.cbegin(); it != m_success.cend(); ++it){
				result.push_back(it->first);
			}
			return transition_collection(result);
		}

	private:
		ptr next_state(charType character, bool ignore_root_state) const {
			ptr result = nullptr;
			auto found = m_success.find(character);
			if(found != m_success.end()) {
				result = found->second.get();
			} else if(!ignore_root_state && m_root != nullptr) {
				result = m_root;
			}
			return result;
		}
	}; // end class state

	// class basic_trie
	template<typename charType>
	class basic_trie
	{
	public:
		using string_type = std::basic_string<charType>;
		using string_ref_type = std::basic_string<charType>&;

		typedef state<charType>			state_type;
		typedef state<charType>*		state_ptr_type;
		typedef token<charType>			token_type;
		typedef emit<charType>			emit_type;
		typedef std::vector<token_type> token_collection;
		typedef std::vector<emit_type> 	emit_collection;

		// class config
		class config
		{
			bool m_allow_overlaps;
			bool m_only_whole_words;
			bool m_case_insensitive;

		public:
			config():
				m_allow_overlaps(true),
				m_only_whole_words(false),
				m_case_insensitive(false) {}

			inline bool is_allow_overlaps() const { return m_allow_overlaps; }
			inline void set_allow_overlaps(bool val) { m_allow_overlaps = val; }

			inline bool is_only_whole_words() const { return m_only_whole_words; }
			inline void set_only_whole_words(bool val) { m_only_whole_words = val; }

			inline bool is_case_insensitive() const { return m_case_insensitive; }
			inline void set_case_insensitive(bool val) { m_case_insensitive = val; }
		}; // end class config

	private:
		std::unique_ptr<state_type> m_root;
		config						m_config;
		bool 						m_constructed_failure_states;
		unsigned					m_num_keywords = 0;

	public:
		basic_trie(): basic_trie(config()) {}

		basic_trie(const config& c):
			m_root(new state_type()),
			m_config(c),
			m_constructed_failure_states(false) {}

		// FLAGS
		basic_trie& case_insensitive() {			// case insensitive flag
			m_config.set_case_insensitive(true);
			return (*this); }

		basic_trie& remove_overlaps() {				// remove overlaps flag
			m_config.set_allow_overlaps(false);
			return (*this); }

		basic_trie& only_whole_words() {			// only whole words flag
			m_config.set_only_whole_words(true);
			return (*this); }

		// insert string object
		void insert(string_type keyword) {
			if(keyword.empty())
				return;
			state_ptr_type cur_state = m_root.get();
			for(const auto& ch : keyword) {
				cur_state = cur_state->add_state(ch);
			}
			cur_state->add_emit(keyword, m_num_keywords++);
		}

		// insert using iterators
		template<class InputIt>
		void insert(InputIt first, InputIt last) {
			for(InputIt it = first; first != last; ++it) {
				insert(*it);
			}
		}

		// split word into collecion of tokens
		token_collection tokenise(string_type text) {
			token_collection tokens;
			auto collected_emits = parse_text(text);
			size_t last_pos = -1;
			for(const auto& e : collected_emits) {
				if(e.get_start() - last_pos > 1) {
					tokens.push_back(create_fragment(e, text, last_pos));
				}
				tokens.push_back(create_match(e, text));
				last_pos = e.get_end();
			}
			if(text.size() - last_pos > 1) {
				tokens.push_back(create_fragment(typename token_type::emit_type(), text, last_pos));
			}
			return token_collection(tokens);
		}

		// 
		emit_collection parse_text(string_type text) {
			check_construct_failure_states();
			size_t pos = 0; 
			state_ptr_type cur_state = m_root.get();
			emit_collection collected_emits;

			for(auto c : text) {
				if(m_config.is_case_insensitive())
					c = std::tolower(c);
				cur_state = get_state(cur_state, c);
				store_emits(pos, cur_state, collected_emits);
				++pos;
			}
			if(m_config.is_only_whole_words()) {
				remove_partial_matches(text, collected_emits);
			}
			if(!m_config.is_allow_overlaps()) {
				interval_tree<emit_type> tree(typename interval_tree<emit_type>::interval_collection(collected_emits.begin(), collected_emits.end()));
				auto tmp = tree.remove_overlaps(collected_emits);
				collected_emits.swap(tmp);
			}

			return emit_collection(collected_emits);
		}

	private:
		token_type create_fragment(const typename token_type::emit_type& e, string_ref_type text, size_t last_pos) const {
			auto start = last_pos + 1;
			auto end = (e.is_empty()) ? text.size() : e.get_start();
			auto len = end - start;
			typename token_type::string_type str(text.substr(start, len));
			return token_type(str);
		}
		token_type create_match(const typename token_type::emit_type& e, string_ref_type text) const {
			auto start = e.get_start();
			auto end = e.get_end() + 1;
			auto len = end - start;
			typename token_type::string_type str(text.substr(start, len));
			return token_type(str, e);
		}

		void remove_partial_matches(string_ref_type search_text, emit_collection& collected_emits) const {
			size_t size = search_text.size();
			emit_collection remove_emits;
			for(const auto& e : collected_emits) {
				if((e.get_start() == 0 || !std::isalpha(search_text.at(e.get_start() - 1))) &&
					(e.get_end() + 1 == size || !std::isalpha(search_text.at(e.get_end() + 1)))
					) {
					continue;
				}
				remove_emits.push_back(e);
			}
			for(auto& e : remove_emits) {
				collected_emits.erase(
					std::find(collected_emits.begin(), collected_emits.end(), e)
					);
			}
		}

		state_ptr_type get_state(state_ptr_type cur_state, charType c) const {
			state_ptr_type result = cur_state->next_state(c);
			while(result == nullptr) {
				cur_state = cur_state->failure();
				result = cur_state->next_state(c);
			}
			return result;
		}

		void check_construct_failure_states() {
			if(!m_constructed_failure_states)
				construct_failure_states();
		}
		void construct_failure_states() {
			std::queue<state_ptr_type> q;
			for(auto& depth_one_state : m_root->get_states()) {
				depth_one_state->set_failure(m_root.get());
				q.push(depth_one_state);
			}
			m_constructed_failure_states = true;

			while(!q.empty()) {
				auto cur_state = q.front();
				for(const auto& transition : cur_state->get_transitions()) {
					state_ptr_type target_state = cur_state->next_state(transition);
					q.push(target_state);

					state_ptr_type trace_failure_state = cur_state->failure();
					while(trace_failure_state->next_state(transition) == nullptr)
						trace_failure_state = trace_failure_state->failure();

					state_ptr_type new_failure_state = trace_failure_state->next_state(transition);
					target_state->set_failure(new_failure_state);
					target_state->add_emit(new_failure_state->get_emits());
				}
				q.pop();
			}
		}

		void store_emits(size_t pos, state_ptr_type cur_state, emit_collection& collected_emits) const {
			auto emits = cur_state->get_emits();
			if(!emits.empty()) {
				for(const auto& str : emits) {
					auto emit_str = typename emit_type::string_type(str.first);
					collected_emits.push_back(emit_type(pos - emit_str.size() + 1, pos, emit_str, str.second));
				}
			}
		}
	}; // end class basic_trie

	typedef basic_trie<char> trie;
	typedef basic_trie<wchar_t> wtrie;

}; // namespace aho_corasick

#endif // end AHO_CORASICK_HPP