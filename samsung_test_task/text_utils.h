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

	string_t ngram_from_flat_index(const size_t idx, const size_t sequence_length) const {
		size_t idx10 = idx;
		string_t ngram;
		for (int n = sequence_length - 1; n >= 0; --n) {
			size_t e = size_t(std::pow(size(), n));
			size_t letter_idx = idx10 / e;
			char c = letter_from_index(letter_idx);
			ngram += c;
			idx10 = std::max(idx10 - letter_idx * e, 0U);
		}
		return ngram;
	}

	size_t flat_index_from_ngram(const string_t& ngram) const {
		size_t flat_idx = 0;
		size_t ngram_length = ngram.length();
		for (size_t i = 0; i < ngram_length; ++i) {
			size_t e = size_t(std::pow(size(), ngram_length - i - 1));
			flat_idx += index_from_letter(ngram[i]) * e;
		}
		return flat_idx;
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

	string_t ngram_from_flat_index(const size_t idx, const size_t ngram_length) const {
		size_t idx10 = idx;
		string_t ngram;
		for (int n = ngram_length - 1; n >= 0; --n) {
			size_t e = size_t(std::pow(size(), n));
			size_t letter_idx = idx10 / e;
			char c = letter_from_index(letter_idx);
			ngram += c;
			idx10 = std::max(idx10 - letter_idx * e, 0U);
		}
		return ngram;
	}

	size_t flat_index_from_ngram(const string_t& ngram) const {
		size_t flat_idx = 0;
		size_t ngram_length = ngram.length();
		for (size_t i = 0; i < ngram_length; ++i) {
			size_t e = size_t(std::pow(size(), ngram_length - i - 1));
			flat_idx += index_from_letter(ngram[i]) * e;
		}
		return flat_idx;
	}

	//template<class It>
	//size_t flat_index_from_ngram(It ngram_start, It ngram_back) const {
	//	size_t flat_idx = 0;
	//	size_t ngram_pos = 0);
	//	for (;; --ngram_end) {
	//		size_t e = size_t(std::pow(size(), ngram_pos++));
	//		flat_idx += index_from_letter(*ngram_end) * e;
	//		if (ngram_end == ngram_start) {
	//			break;
	//		}
	//	}
	//	return flat_idx;
	//}
};

using ngram_frequencies_flat_list = std::vector<unsigned int>;
using ngram_occurencies_flat_list = std::vector<std::vector<unsigned int>>;

template<class AlphabetT>
struct sequence_frequencies_flat {
	AlphabetT alphabet;
	size_t	  sequence_length;
	ngram_frequencies_flat_list frequencies;
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
sequence_frequencies_flat<AlphabetT> count_letter_occurence(const StringT& text, size_t step = 1, size_t start_pos = 0, AlphabetT alphabet = AlphabetT()) {
	std::vector<unsigned int> freq(alphabet.size());
	size_t text_letters_count = 0;
	size_t text_pos = start_pos;
	size_t text_length = text.length();
	do {
		const CharT& ch = text[text_pos];
		if (alphabet.contains(ch)) {
			text_letters_count += 1;
			freq[alphabet.index_from_letter(ch)] += 1;
		}
		text_pos += step;
	} while (text_pos < text_length);
	return { alphabet , 1 , freq, text_letters_count };
}

template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
sequence_frequencies_flat<AlphabetT> count_letter_frequencies(const StringT& text, size_t step = 1, size_t start_pos = 0, AlphabetT alphabet = AlphabetT()) {
	auto res = count_letter_occurence(text, step, start_pos, alphabet);
	for(const )
	return { alphabet , 1 , freq, text_letters_count };
}


template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
std::vector<sequence_frequencies_flat<AlphabetT>>
count_letter_occurence_multi_step(const StringT& text, size_t max_step, size_t start_pos = 0, AlphabetT alphabet = AlphabetT()) {
	std::vector<sequence_frequencies_flat<AlphabetT>> seq_freq;
	for (size_t i = 1; i <= max_step; ++i) seq_freq.push_back({ alphabet, i, ngram_frequencies_flat_list(alphabet.size()) , 0 });

	size_t text_letters_count = 0;
	size_t text_pos = start_pos;
	size_t text_length = text.length();
	for(const CharT& ch : text) {

		if (alphabet.contains(ch)) {
			text_letters_count += 1;
			size_t char_idx = alphabet.index_from_letter(ch);
			seq_freq[0].frequencies[char_idx] += 1;
			seq_freq[0].text_letters_count += 1;


			for (size_t step = 2; step <= max_step; ++step) {
				if (text_pos%step == 0) {
					seq_freq[step-1].frequencies[char_idx] += 1;
					seq_freq[step-1].text_letters_count += 1;

				}
			}
		}
		++text_pos;
	} 
	return seq_freq;
}
using shifted_coincedences_list = std::vector<int>;
template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
shifted_coincedences_list
count_shifted_letters_coincedences_multi(const StringT& text, size_t max_step, AlphabetT alphabet = AlphabetT()) {
	max_step = std::min(text.length(), max_step);
	if (max_step == 1) return shifted_coincedences_list();
	shifted_coincedences_list cl(max_step+1);

	size_t text_letters_count = 0;
	int text_pos = 0;
	size_t text_length = text.length();
	for (const CharT& ch : text) {

		if (alphabet.contains(ch)) {
			text_letters_count += 1;
			size_t char_idx = alphabet.index_from_letter(ch);

			for (int step = 1; step <= (int)max_step && text_pos-step >= 0; ++step) {
				size_t step_char_idx = alphabet.index_from_letter(text[text_pos - step]);
				if (char_idx == step_char_idx) {
					cl[step] += 1;
				}
			}
		}
		++text_pos;
	}
	cl[0] = text_letters_count;
	return cl;
}

using shifted_coincedences_prop_list = std::vector<double>;
template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
shifted_coincedences_prop_list
count_shifted_letters_coincedences_prop_multi(const StringT& text, size_t max_step, AlphabetT alphabet = AlphabetT()) {
	max_step = std::min(text.length(), max_step);
	if (max_step == 1) return shifted_coincedences_prop_list();
	shifted_coincedences_prop_list cl(max_step + 1);

	size_t text_letters_count = 0;
	int text_pos = 0;
	size_t text_length = text.length();
	for (const CharT& ch : text) {

		if (alphabet.contains(ch)) {
			text_letters_count += 1;
			size_t char_idx = alphabet.index_from_letter(ch);

			for (int step = 1; step <= (int)max_step && text_pos - step >= 0; ++step) {
				size_t step_char_idx = alphabet.index_from_letter(text[text_pos - step]);
				if (char_idx == step_char_idx) {
					cl[step] += 1;
				}
			}
		}
		++text_pos;
	}
	cl[0] = 1;
	for (size_t i = 1; i < cl.size(); ++i) {
		cl[i] = cl[i] / double(text_letters_count);
	}
	return cl;
}
template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
sequence_frequencies_flat<AlphabetT> text_seq_freq(size_t seq_size, const StringT& text, AlphabetT alphabet = AlphabetT()) {
	seq_size = std::min(seq_size, text.length());
	if (seq_size == 1) return count_letter_occurence(text, 1, 0, alphabet);
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
			//seq_length = 0;
		}
	}
	return { alphabet , seq_size , freq, text_letters_count };
}

template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
sequence_frequencies_flat<AlphabetT>  
count_ngram_occurence(size_t ngram_size, const StringT& text, AlphabetT alphabet = AlphabetT()) {
	ngram_size = std::min(ngram_size, text.length());
	std::vector<unsigned int> counts(static_cast<size_t>(std::pow(alphabet.size(), ngram_size)));
	std::deque<CharT> q;
	size_t text_letters_count = 0;

	for (const CharT& ch : text) {
		if (alphabet.contains(ch)) {
			q.push_front(ch);
			text_letters_count += 1;

			if (q.size() == ngram_size) {
				size_t ngram_idx10 = 0;
				size_t idx = 0;
				for (const CharT& c : q) {
					ngram_idx10 += alphabet.index_from_letter(c) * static_cast<size_t>(std::pow(alphabet.size(), idx++));
				}
				counts[ngram_idx10] += 1;
				q.pop_back();
			}
		}
		else {
			q.clear();
		}
	}
	return { alphabet , ngram_size , counts, text_letters_count };
}

template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
sequence_occurence_flat<AlphabetT>  get_ngram_positions(size_t ngram_size, const StringT& text, AlphabetT alphabet = AlphabetT()) {
	ngram_size = std::min(ngram_size, text.length());
	//std::vector<unsigned int> possible_amount_of_occurence
	std::vector<unsigned int> counts(static_cast<size_t>(std::pow(alphabet.size(), ngram_size)));
	std::vector<std::vector<unsigned int>> occurencies(static_cast<size_t>(std::pow(alphabet.size(), ngram_size)));
	std::deque<CharT> q;
	size_t text_pos = 0;
	size_t text_letters_count = 0;
	for (const CharT& ch : text) {
		if (alphabet.contains(ch)) {
			q.push_front(ch);
			++text_letters_count;
			if (q.size() == ngram_size) {
				size_t ngram_idx10 = 0;
				size_t idx = 0;
				for (const CharT& c : q) {
					ngram_idx10 += alphabet.index_from_letter(c) * static_cast<size_t>(std::pow(alphabet.size(), idx++));
				}
				counts[ngram_idx10] += 1;
				occurencies[ngram_idx10].push_back(text_pos);
				q.pop_back();
			}
		}
		else {
			q.clear();
		}
		++text_pos;
	}
	return { alphabet , ngram_size , counts , occurencies, text_letters_count };
}

template<class AlphabetT = english_case_less, class StringT = typename AlphabetT::string_t, class CharT = typename StringT::value_type>
std::map<unsigned int, std::vector<StringT> , std::greater<unsigned int> > 
flat_freq_to_freq_dict(ngram_frequencies_flat_list flat_frequencies, size_t sequence_length, AlphabetT alphabet = english_case_less()) {
	assert(flat_frequencies.size() == static_cast<size_t>(std::pow(alphabet.size(), sequence_length)));
	size_t idx = 0;
	std::map<unsigned int, std::vector<StringT>, std::greater<unsigned int> > frequency_map;
	for (const unsigned int f : flat_frequencies) {
		if (f > 0) {
			size_t idx10 = idx;
			StringT letter_idxs;
			for (int n = sequence_length - 1; n >= 0; --n) {
				size_t e = size_t(std::pow(alphabet.size(), n));
				size_t letter_idx = idx10 / e;
				CharT c = alphabet.letter_from_index(letter_idx);
				letter_idxs += c;
				idx10 = std::max(idx10 - letter_idx * e, 0U);
			}
			frequency_map[f].push_back(letter_idxs);
		}
		++idx;
	}
	return frequency_map;
}


template<class AlphabetT = english_case_less, class StringT = typename AlphabetT::string_t, class CharT = typename StringT::value_type>
std::map<StringT, unsigned int> 
flat_freq_to_letters_dict(ngram_frequencies_flat_list flat_frequencies, size_t sequence_length, AlphabetT alphabet = english_case_less()) {
	assert(flat_frequencies.size() == static_cast<size_t>(std::pow(alphabet.size(), sequence_length)));
	size_t idx = 0;
	std::map<StringT, unsigned int> frequency_map;
	for (const unsigned int f : flat_frequencies) {
		if (f > 0) {
			size_t idx10 = idx;
			StringT letter_idxs;
			for (int n = sequence_length - 1; n >= 0; --n) {
				size_t e = size_t(std::pow(alphabet.size(), n));
				size_t letter_idx = idx10 / e;
				CharT c = alphabet.letter_from_index(letter_idx);
				letter_idxs += c;
				idx10 = std::max(idx10 - letter_idx * e, 0U);
			}
			frequency_map[letter_idxs] = f;
		}
		++idx;
	}
	return frequency_map;
}

template< class AlphabetT = english_case_less >
double text_index_of_coincidence(const sequence_frequencies_flat<AlphabetT> &text_stats)
{
	double ci = 0;
	double N = text_stats.text_letters_count;

	for (const auto& lf : text_stats.frequencies) {
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