#pragma once
#include <string>
#include <cassert>
#include <set>
#include <vector>
#include <map>

//general case alphabet
template<class StringT, class CharT = typename StringT::value_type>
struct custom_alphabet {
	using string_t = StringT;
	using char_t = CharT;

	std::set<CharT> letters;
	custom_alphabet(const string_t& sample_text_or_letters) {
		for (const char_t& c : sample_text_or_letters) {
			letters.insert(c);
		}
	}

	bool contains(const char_t& ch) const {
		return letters.find(ch) != letters.end();
	}

	size_t index_from_letter(const char_t& ch) const {
		auto it = letters.find(ch);
		assert(it != letters.end());
		return std::distance(letters.begin(), it);
	}

	char_t letter_from_index(const size_t& idx) const {
		assert(idx >= 0 && idx <= letters.size());
		auto it = letters.begin();
		std::advance(it, idx);
		return *it;
	}

	size_t size() const {
		return letters.size();
	}

};

template<class StringT, class CharT = typename StringT::value_type>
custom_alphabet<StringT, CharT> alphabet(const StringT &sample_text_or_letters) {
	return custom_alphabet<StringT, CharT>(sample_text_or_letters);
}

//english case-less alphabet
struct english_case_less {
	using string_t = std::string;
	using char_t = char;

	english_case_less() {
	}
	bool contains(const char& ch) const {
		return !!::isalpha(ch);
	}

	size_t index_from_letter(const char& ch) const {
		assert(::isalpha(ch));
		return ::tolower(ch) - 'a';
	}

	char letter_from_index(const size_t& idx) const {
		assert(idx >= 0 && idx <= 26);
		return 'a' + idx;
	}

	size_t size() const {
		return 26;
	}
};

template<class AlphabetT>
struct sequence_frequencies_flat {
	AlphabetT alphabet;
	size_t	  sequence_length;
	std::vector<unsigned int> frequencies;
};




template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_lower_case<StringT, CharT> >
sequence_frequencies_flat<AlphabetT> text_seq_freq(size_t seq_size, const StringT& text, AlphabetT alphabet = AlphabetT()) {
	seq_size = std::min(seq_size, text.length());
	std::vector<unsigned int> freq(static_cast<size_t>(std::pow(alphabet.size(), seq_size)));
	std::vector<CharT> seq(seq_size);
	size_t seq_length = 0;
	size_t seq_offset = 0;

	for (const CharT& ch : text) {
		if (alphabet.contains(ch)) {
			seq_length += 1;
			seq[seq_offset] = ch;
			seq_offset = (seq_offset + 1) % seq_size;
			if (seq_length >= seq_size) {
				size_t freq_idx = 0;
				for (size_t i = 0; i < seq_size; ++i) {
					freq_idx += alphabet.index_from_letter(seq[(i + seq_offset) % seq_size]) * std::pow(alphabet.size(), seq_size - i - 1);
				}
				freq[freq_idx] += 1;
			}

		}
		else {
			//reset the seq
			seq_length = 0;
		}
	}
	return { alphabet , seq_size , freq };
}

template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_lower_case<StringT, CharT> >
sequence_frequencies_flat<AlphabetT>  text_que_freq(size_t seq_size, const StringT& text, AlphabetT alphabet = AlphabetT()) {
	seq_size = std::min(seq_size, text.length());
	std::vector<unsigned int> freq(static_cast<size_t>(std::pow(alphabet.size(), seq_size)));
	std::deque<CharT> q;

	for (const CharT& ch : text) {
		if (alphabet.contains(ch)) {
			q.push_front(ch);

			if (q.size() == seq_size) {
				size_t freq_idx = 0;
				size_t idx = 0;
				for (const CharT& c : q) {
					freq_idx += alphabet.index_from_letter(c) * static_cast<size_t>(std::pow(alphabet.size(), idx++));
				}
				freq[freq_idx] += 1;
				q.pop_back();
			}
		}
		else {
			q.clear();
		}
	}
	return { alphabet , seq_size , freq };
}

template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_lower_case<StringT, CharT> >
std::multimap<unsigned int, StringT> flat_freq_to_dict(sequence_frequencies_flat<AlphabetT> flat_freq) {
	assert(flat_freq.frequencies.size() == static_cast<size_t>(std::pow(flat_freq.alphabet.size(), flat_freq.sequence_length)));
	size_t idx = 0;
	std::multimap<unsigned int, StringT> frequency_map;
	for (const auto& f : flat_freq.frequencies) {
		if (f > 0) {
			size_t idx10 = idx++;
			StringT letter_idxs;
			//std::cout << " idx: " << idx10 << std::endl;
			for (int n = flat_freq.sequence_length - 1; n >= 0; --n) {
				size_t e = std::pow(flat_freq.alphabet.size(), n);
				size_t letter_idx = idx10 / e;
				CharT c = flat_freq.alphabet.letter_from_index(letter_idx);
				letter_idxs += c;
				//std::cout << "      e: " << e << ", letter_idx: " << letter_idx << ", idx10: " << idx10 << ", c: " << c << std::endl;
				idx10 = std::max(idx10 - letter_idx * e, 0U);
			}
			frequency_map[letter_idxs] = f;
		}
	}
	return frequency_map;
}