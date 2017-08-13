// https://github.com/cjgdev/aho_corasick/blob/master/src/aho_corasick/aho_corasick.hpp
#ifndef AHO_CORASICK_HPP
#define AHO_CORASICK_HPP

#include <algorithm>
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
		constexpr interval(size_t start, size_t end)
			: m_start(start), m_end(end) {}

		inline size_t getm_start() const { return m_start; }
		inline size_t getm_end() const { return m_end; }
		size_t range() const { return m_end - m_start + 1; }

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
					add_to_overlaps(i, overlaps, check_right_overlaps());
				} else if(m_point > i.get_end()) {	// if input ends to the left of midpoint, add only this node's overlapps and the left node's overlapps
					add_to_overlaps(i, overlaps, find_overlapping_ranges(m_left, i));
					add_to_overlaps(i, overlaps, check_left_overlaps());
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
					return interval_collection(node->find_intervals(i)); // return overlaps in other node with i
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
		interval_collection remove_overlapps(const interval_collection& intervals) {
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

	template<typename charType>
	class emit: public interval
	{

	};

	template<typename charType>
	class token
	{

	};

	template<typename charType>
	class state
	{

	};

	template<typename charType>
	class basic_trie
	{
	public:
		class config
		{

		};
	};
};

#endif // end AHO_CORASICK_HPP