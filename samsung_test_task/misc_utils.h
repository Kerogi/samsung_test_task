#pragma once

#include <ostream>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <deque>

namespace quoted_string {
	inline std::ostream& operator<<(std::ostream& os, const std::string& s) {
		std::streamsize width = os.width();
		size_t max_text_length = (size_t)width;
		if (max_text_length == 0) max_text_length = 30;
		os.width(0);
		os << "'";
		if (s.length() <= max_text_length) {
			os << s.c_str();
		}
		else {
			os << s.substr(0, max_text_length - 3).c_str() << "...";
		}
		os << "'";
		os.width(width);
		return os;
	}
}
template<class ItemFirstT, class ItemSecondT>
std::ostream& operator<<(std::ostream& os, const std::pair< ItemFirstT, ItemSecondT> & c) {
	using namespace quoted_string;
	return os << '(' << c.first << "; " << c.second << ')';;
}

template<class ItemT>
std::ostream& operator<<(std::ostream& os, const std::list< ItemT > & c) {
	using namespace quoted_string; 
	os << '(';
	if (!c.empty()) {
		bool first = true;
		for (const ItemT& item : c) {
			if (!first) os << ", ";
			os << item;
			first = false;
		}
	}
	os << ')';
	return os;
}


template<class ItemT>
std::ostream& operator<<(std::ostream& os, const std::vector< ItemT > & c) {
	using namespace quoted_string;
	os << '[';
	if (!c.empty()) {
		bool first = true;
		for (const ItemT& item : c) {
			if (!first) os << ", ";
			os << item;
			first = false;
		}
	}
	os << ']';
	return os;
}

template<class ItemT>
std::ostream& operator<<(std::ostream& os, const std::queue<ItemT> & c) {
	using namespace quoted_string;
	os << '<';
	if (!c.empty()) {
		bool first = true;
		for (const auto& i: c) {
			if (!first) os << ", ";
			os << i ;
			first = false;
		}
	}
	os << '>';
	return os;
}

template<class ItemT>
std::ostream& operator<<(std::ostream& os, const std::deque<ItemT> & c) {
	using namespace quoted_string;
	os << '<';
	if (!c.empty()) {
		bool first = true;
		for (const auto& i : c) {
			if (!first) os << ", ";
			os << i;
			first = false;
		}
	}
	os << '>';
	return os;
}



template<class KeyT, class ItemT, class ...Rest>
std::ostream& operator<<(std::ostream& os, const std::map< KeyT, ItemT,Rest...> & c) {
	using namespace quoted_string;
	os << '{';
	if (!c.empty()) {
		bool first = true;
		for (const auto& kv : c) {
			if (!first) os << ",\n";
			os << kv.first <<": "<< kv.second;
			first = false;
		}
	}
	os << '}';
	return os;
}


template<class KeyT, class ItemT>
std::ostream& operator<<(std::ostream& os, const std::multimap< KeyT, ItemT> & c) {
	using namespace quoted_string;
	os << '{';
	if (!c.empty()) {
		bool first = true;
		for (auto it = c.begin(); it != c.end(); ++it) {
			if (!first) os << ",\n";
			os << it->first << ": (";
			auto range = c.equal_range(it->first);
			os << range;
			/*bool sub_first = true;
			for (auto v = range.first; v != range.second; ++v){
				if (!sub_first) os << ", ";
				os << *v;
				sub_first = false;
			}*/
			os << it->second << ")";
			first = false;
		}
	}
	os << '}';
	return os;
}

template<class KeyT>
std::ostream& operator<<(std::ostream& os, const std::set< KeyT> & c) {
	using namespace quoted_string;
	os << '{';
	if (!c.empty()) {
		bool first = true;
		for (const auto& k: c) {
			if (!first) os << ", ";
			os << k;
			first = false;
		}
	}
	os << '}';
	return os;
}