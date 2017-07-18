#pragma once
#include <string>
#include <cassert>
#include <set>
#include <vector>
#include <map>
#include <deque>
#include <functional>

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
		return ::toupper(ch) - 'A';
	}

	char letter_from_index(const size_t& idx) const {
		assert(idx >= 0 && idx <= 26);
		return char('A' + idx);
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
	size_t		text_letters_count;
};

template<class AlphabetT>
struct sequence_occurence_flat {
	AlphabetT alphabet;
	size_t	  sequence_length;
	std::vector<unsigned int> frequencies;
	std::vector<std::vector<unsigned int>> occurence;
	size_t		text_letters_count;
};

template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
sequence_frequencies_flat<AlphabetT> text_letter_freq(const StringT& text, AlphabetT alphabet = AlphabetT()) {
	std::vector<unsigned int> freq(alphabet.size());
	size_t text_letters_count = 0;

	for (const CharT& ch : text) {
		if (alphabet.contains(ch)) {
			text_letters_count += 1;
			freq[alphabet.index_from_letter(ch)] += 1;
		}
	}
	return { alphabet , 1 , freq, text_letters_count };
}

template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
sequence_frequencies_flat<AlphabetT> text_seq_freq(size_t seq_size, const StringT& text, AlphabetT alphabet = AlphabetT()) {
	seq_size = std::min(seq_size, text.length());
	std::vector<unsigned int> freq(static_cast<size_t>(std::pow(alphabet.size(), seq_size)));
	std::vector<CharT> seq(seq_size);
	size_t seq_length = 0;
	size_t seq_offset = 0;
	size_t text_letters_count = 0;

	for (const CharT& ch : text) {
		if (alphabet.contains(ch)) {
			seq_length += 1;
			text_letters_count += 1;
			seq[seq_offset] = ch;
			seq_offset = (seq_offset + 1) % seq_size;
			if (seq_length >= seq_size) {
				size_t freq_idx = 0;
				for (size_t i = 0; i < seq_size; ++i) {
					size_t e = size_t(std::pow(alphabet.size(), seq_size - i - 1));
					freq_idx += alphabet.index_from_letter(seq[(i + seq_offset) % seq_size]) * e;
				}
				freq[freq_idx] += 1;
			}

		}
		else {
			//reset the seq
			seq_length = 0;
		}
	}
	return { alphabet , seq_size , freq, text_letters_count };
}

template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
sequence_frequencies_flat<AlphabetT>  count_ngram_occurence(size_t seq_size, const StringT& text, AlphabetT alphabet = AlphabetT()) {
	seq_size = std::min(seq_size, text.length());
	std::vector<unsigned int> freq(static_cast<size_t>(std::pow(alphabet.size(), seq_size)));
	std::deque<CharT> q;
	size_t text_letters_count = 0;

	for (const CharT& ch : text) {
		if (alphabet.contains(ch)) {
			q.push_front(ch);
			text_letters_count += 1;

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
	return { alphabet , seq_size , freq, text_letters_count };
}

template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
sequence_occurence_flat<AlphabetT>  get_ngram_positions(size_t seq_size, const StringT& text, AlphabetT alphabet = AlphabetT()) {
	seq_size = std::min(seq_size, text.length());
	//std::vector<unsigned int> possible_amount_of_occurence
	std::vector<unsigned int> freq(static_cast<size_t>(std::pow(alphabet.size(), seq_size)));
	std::vector<std::vector<unsigned int>> occ(static_cast<size_t>(std::pow(alphabet.size(), seq_size)));
	std::deque<CharT> q;
	size_t text_pos = 0;
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
				occ[freq_idx].push_back(text_pos);
				q.pop_back();
			}
		}
		else {
			q.clear();
		}
		++text_pos;
	}
	return { alphabet , seq_size , freq , occ, text_letters_count };
}

template<class AlphabetT = english_case_less, class StringT = typename AlphabetT::string_t, class CharT = typename StringT::value_type>
std::map<unsigned int, std::vector<StringT> , std::greater<unsigned int> > flat_freq_to_freq_dict(sequence_frequencies_flat<AlphabetT> flat_freq) {
	assert(flat_freq.frequencies.size() == static_cast<size_t>(std::pow(flat_freq.alphabet.size(), flat_freq.sequence_length)));
	size_t idx = 0;
	std::map<unsigned int, std::vector<StringT>, std::greater<unsigned int> > frequency_map;
	for (const unsigned int f : flat_freq.frequencies) {
		if (f > 0) {
			size_t idx10 = idx;
			StringT letter_idxs;
			//std::cout << " idx: " << idx10 << std::endl;
			for (int n = flat_freq.sequence_length - 1; n >= 0; --n) {
				size_t e = size_t(std::pow(flat_freq.alphabet.size(), n));
				size_t letter_idx = idx10 / e;
				CharT c = flat_freq.alphabet.letter_from_index(letter_idx);
				letter_idxs += c;
				//std::cout << "      e: " << e << ", letter_idx: " << letter_idx << ", idx10: " << idx10 << ", c: " << c << std::endl;
				idx10 = std::max(idx10 - letter_idx * e, 0U);
			}
			frequency_map[f].push_back(letter_idxs);
		}
		++idx;
	}
	return frequency_map;
}


template<class AlphabetT = english_case_less, class StringT = typename AlphabetT::string_t, class CharT = typename StringT::value_type>
std::map<StringT, unsigned int> flat_freq_to_letters_dict(sequence_frequencies_flat<AlphabetT> flat_freq) {
	assert(flat_freq.frequencies.size() == static_cast<size_t>(std::pow(flat_freq.alphabet.size(), flat_freq.sequence_length)));
	size_t idx = 0;
	std::map<StringT, unsigned int> frequency_map;
	for (const unsigned int f : flat_freq.frequencies) {
		if (f > 0) {
			size_t idx10 = idx;
			StringT letter_idxs;
			//std::cout << " idx: " << idx10 << std::endl;
			for (int n = flat_freq.sequence_length - 1; n >= 0; --n) {
				size_t e = size_t(std::pow(flat_freq.alphabet.size(), n));
				size_t letter_idx = idx10 / e;
				CharT c = flat_freq.alphabet.letter_from_index(letter_idx);
				letter_idxs += c;
				//std::cout << "      e: " << e << ", letter_idx: " << letter_idx << ", idx10: " << idx10 << ", c: " << c << std::endl;
				idx10 = std::max(idx10 - letter_idx * e, 0U);
			}
			frequency_map[letter_idxs] = f;
		}
		++idx;
	}
	return frequency_map;
}

template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
double text_index_of_coincidence(const StringT& text, AlphabetT alphabet = AlphabetT())
{
	auto f1 = text_letter_freq(text, alphabet);
	assert(alphabet.size() == f1.frequencies.size());
	double ci = 0;
	double N = f1.text_letters_count;
	for (const auto& lf : f1.frequencies) {
		double ni = lf;
		ci += (ni *(ni - 1.0)) / (N * (N - 1));
	}

	return ci;
}
/*
template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
double text_measure_of_roughness(const StringT& text, AlphabetT alphabet = AlphabetT())
{
	auto f1 = text_letter_freq(text, alphabet);
	assert(alphabet.size() == f1.frequencies.size());
	double mr = 0;
	double Pavg = 1.0/alphabet.size();
	for (const auto& lf : f1.frequencies) {
		double ni = lf;
		ci += (ni *(ni - 1.0)) / (N * (N - 1));
	}

	return ci;
}
*/