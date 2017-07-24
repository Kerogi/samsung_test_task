#pragma once
#include <string>
#include <cassert>
#include <set>
#include <vector>
#include <map>
#include <deque>
#include <functional>
#include <iostream>
#include <iomanip>

inline int gcd(int num1, int num2)
{
	bool found = false;
	int test = 0;

	if (std::abs(num1) < std::abs(num2))
		test = std::abs(num1);
	else
		test = std::abs(num2);

	while (num1%test != 0 || num2%test != 0)  //If the number divides evenly into both.
	{
		--test;
	}

	return test;
}

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
			idx10 = std::max(idx10 - letter_idx * e, 0UL);
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
			idx10 = std::max(idx10 - letter_idx * e, size_t(0));
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

using letter_coincedences_list = std::vector<int>;
using ngram_frequensy_flat_list = std::vector<double>;
using ngram_occurrence_flat_list = std::vector<int>;
using ngram_positions_flat_list = std::vector<std::vector<unsigned int>>;

template<class AlphabetT>
struct sequence_frequencies_flat {
	AlphabetT alphabet;
	size_t	  sequence_length;
	ngram_frequensy_flat_list frequencies;
	size_t		text_letters_count;
};

template<class AlphabetT>
struct sequence_occurence_flat {
	AlphabetT alphabet;
	size_t	  sequence_length;
	//std::vector<unsigned int> frequencies;
	ngram_occurrence_flat_list occurence;
	size_t		text_letters_count;
};

template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
sequence_occurence_flat<AlphabetT> count_letter_occurence(const StringT& text, size_t step = 1, size_t start_pos = 0, AlphabetT alphabet = AlphabetT()) {
	ngram_occurrence_flat_list occ(alphabet.size());
	size_t text_letters_count = 0;
	size_t text_pos = start_pos;
	size_t text_length = text.length();
	do {
		const CharT& ch = text[text_pos];
		if (alphabet.contains(ch)) {
			text_letters_count += 1;
			occ[alphabet.index_from_letter(ch)] += 1;
		}
		text_pos += step;
	} while (text_pos < text_length);
	return { alphabet , 1 , occ, text_letters_count };
}

template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
sequence_frequencies_flat<AlphabetT> count_letter_frequencies(const StringT& text, size_t step = 1, size_t start_pos = 0, AlphabetT alphabet = AlphabetT()) {
	auto res = count_letter_occurence(text, step, start_pos, alphabet);
	ngram_frequensy_flat_list freq(alphabet.size());
	size_t letter_index = 0;
	for (const auto& occ : res.occurence) {
		freq[letter_index++] = double(occ) / res.text_letters_count;
	}
	return { alphabet , 1 , freq,  res.text_letters_count };
}


template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
std::vector<sequence_frequencies_flat<AlphabetT>>
count_letter_occurence_multi_step(const StringT& text, size_t max_step, size_t start_pos = 0, AlphabetT alphabet = AlphabetT()) {
	std::vector<sequence_frequencies_flat<AlphabetT>> seq_freq;
	for (size_t i = 1; i <= max_step; ++i) seq_freq.push_back({ alphabet, i, ngram_occurrence_flat_list(alphabet.size()) , 0 });

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

template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
letter_coincedences_list
count_shifted_letters_coincedences_multi(const StringT& text, size_t max_step, AlphabetT alphabet = AlphabetT()) {
	max_step = std::min(text.length(), max_step);
	if (max_step == 1) return letter_coincedences_list();
	letter_coincedences_list cl(max_step+1);

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

// function to calculate texts index of coincidence 
// 
template< class AlphabetT = english_case_less >
double text_index_of_coincidence(const sequence_occurence_flat<AlphabetT> &text_stats)
{
	double IC = 0;
	double N = text_stats.text_letters_count;
	double c = 1;//text_stats.alphabet.size(); // normalization coof

	for (const auto& lf : text_stats.occurence) {
		double ni = lf;
		IC += (ni *(ni - 1.0));
	}
	IC = IC / (N * (N - 1) / c);

	return IC  ;
}


template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
size_t guess_key_length_ci(std::ostream & os, const StringT& text, double etalon_CI, size_t max_key_length, AlphabetT alphabet = AlphabetT())
{
	auto distance_to_etalon_CI = [&etalon_CI]
	(const double& l_ci, const double& r_ci) -> bool {
		return std::abs(l_ci - etalon_CI) < std::abs(r_ci - etalon_CI);
	};

	std::map<double, int, decltype(distance_to_etalon_CI) > ci_interleaved_for_keys_lengths(distance_to_etalon_CI);

	for (size_t possible_key_length = 1; possible_key_length <= max_key_length; ++possible_key_length) {
		double ci_avg = 0;
		for (size_t shift = 0; shift < max_key_length; ++shift) {
			double ci = text_index_of_coincidence(count_letter_occurence(text, possible_key_length, shift, alphabet));
			ci_avg = (ci + (ci_avg*shift)) / (shift + 1);
		}
		ci_interleaved_for_keys_lengths[ci_avg] = possible_key_length;
	}
	os << "\tCI interleaved for keys lengths from 1 to " << max_key_length << std::endl;
	double lowest_diff_from_etalon_ci = std::max_element(ci_interleaved_for_keys_lengths.begin(), ci_interleaved_for_keys_lengths.end(),
		[&distance_to_etalon_CI](const std::pair<double, int>& l, const std::pair<double, int>& r) {return distance_to_etalon_CI(l.first, r.first); })->first;
	for (const auto& kv : ci_interleaved_for_keys_lengths) {
		size_t bar_length = 20;
		std::string bar(size_t(double(bar_length) * (std::abs(kv.first - etalon_CI) / lowest_diff_from_etalon_ci)), '|');
		os << "\t\tfor each " << std::setw(2) << kv.second << "-nth char CI: " << std::setw(14) << kv.first << "( +/-" << std::setw(14) << abs(etalon_CI - kv.first) << " from "<<std::setw(5)<<etalon_CI<<" common) " << " [" << std::setw(bar_length) << bar << "]" << std::endl;
	}
	size_t best_of_possible_keys = 0;
	
	auto iter1 = ci_interleaved_for_keys_lengths.begin();
	auto iter2 = ci_interleaved_for_keys_lengths.begin();
	std::advance(iter2, 1);
	for (; iter2 != ci_interleaved_for_keys_lengths.end(); ++iter1, ++iter2) {
		if (iter1->second < iter2->second) {
			best_of_possible_keys = iter1->second;
			break;
		}
		int cd = gcd(iter1->second, iter2->second);
		if (cd >= iter2->second) {
			best_of_possible_keys = cd;
			break;
		}
	}
	os << "\t best CI for text interleved by period " << best_of_possible_keys << std::endl;

	return best_of_possible_keys;
}


template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
size_t guess_key_length_by_shifted_coincedences(std::ostream & os, const StringT& text, size_t max_key_length, AlphabetT alphabet = AlphabetT())
{
	shifted_coincedences_prop_list sh_c = count_shifted_letters_coincedences_prop_multi(text, std::min(max_key_length*max_key_length, text.length()), alphabet);
	os << "\tProportinal coincedences for chars in shifted text: " << std::endl;

	std::map<double, int, std::greater<double> > best_shifted_c;
	double max_co_prop = *std::max_element(sh_c.begin() + 1, sh_c.end());
	if (max_co_prop < 0.00001) {
		return 0;
	}
	double wtot = 0;
 	double wsum = 0;
	wtot = accumulate(sh_c.begin() + 1, sh_c.end(), wtot, [max_co_prop](double v, double x) {return v + x * (x / max_co_prop); });
	wsum = accumulate(sh_c.begin() + 1, sh_c.end(), wsum, [max_co_prop](double v, double x) {return v + (x / max_co_prop); });

	double wavg = wtot / wsum; 
	std::vector<int> most_significant_indeces;
	for (int i = 1; i < (int)sh_c.size(); ++i) {
		double x = sh_c[i];
		char symb = (x > wavg) ? '+' : '-';
		size_t bar_length = 20;
		std::string bar(size_t(double(bar_length) * (x / max_co_prop)), symb);
		os << "\t\tfor shift " << std::setw(2) << i << ": " << std::setw(14) << x << " [" << std::setw(bar_length) << bar << "] " << std::endl;
		///cout <<" [" << std::setw(10) << string(10 * ((x*x) / (max_co_prop*max_co_prop)), '|') << "]"<< std::endl;
		//	cout << "x/max: " << std::setw(14) << x / max_co_prop << ", x/avg: " << std::setw(14) << x / avg << ", x>awg: " << (x > avg) << ", x/wavg: " << std::setw(14) << x / wavg << ", x>wawg: " << (x > wavg) << std::endl;
		if (x > wavg) most_significant_indeces.push_back(i);
	}
	os << "\t\tWeighted average: " << wavg << std::endl;
	if (most_significant_indeces.size() < 2) {
		os << "\tTo few significant indeces" << std::endl;
		return 0;
	}
	size_t best_period = 0;
	
	std::map<int, int> count_distances_between_best;
	auto iter1 = most_significant_indeces.begin();
	auto iter2 = most_significant_indeces.begin();
	auto end = most_significant_indeces.end();
	//std::advance(iter1, 1);
	std::advance(iter2, 1);
	//std::advance(end, best_shifted_c);
	for (; iter2 != end; ++iter1, ++iter2) {
		count_distances_between_best[*iter2 - *iter1] += 1;
	}
	os << "\tDistances found betwen significant indeces: " << std::endl;

	for (const auto& kv : count_distances_between_best) {

		os << "\t\t distance: " << kv.first <<" found " << kv.second << " times" << std::endl;
	}

	best_period = std::max_element(count_distances_between_best.begin(), count_distances_between_best.end(), [](const std::pair<int, int>& kvl, const std::pair<int, int>& kvr) {return kvl.second < kvr.second; })->first;
	os << "\tMost common distance(period): " <<best_period<< std::endl;
	return best_period;
}


template<class StringT, class CharT = typename StringT::value_type, class AlphabetT = english_case_less >
StringT guess_key(std::ostream & os, const StringT& text, size_t key_length, const std::vector<double>& expected_letters_frequecies, AlphabetT alphabet = AlphabetT())
{
	assert(expected_letters_frequecies.size() == alphabet.size());

	StringT key(key_length, alphabet.letter_from_index(0)); //empty key

	// Chi-squared function witch tell how far one distribution from another
	auto chi_squared_stat = [](double ci, double Ei) { 
		// IC - actual number of letters, 
		// Ei - expected (calculated based on expected_letters_frequecies param, and number of chars in coset(interleaved by key char pos) of text
		return std::pow(ci - Ei, 2) / Ei;
	};

	// for each key char we calculate text stats by jumping over chars, and by starting from differen pos 
	for (size_t key_char = 0; key_char < key_length; ++key_char) {
		// a coset interleved by key length and key chars pos chipered message)
		// message: hellowordl
		// key length 3
		// so there were 3 cosets for each key char
		//       1: h  l  o  d : start at 0 jump 3
		//       2:  e  o  r   : start at 1 jump 3
		//       3:   l  w  l  : start at 2 jump 3
		auto stats_interleaved_by_key_char = count_letter_occurence(text, //text to interleave
			key_length, //the size of the jump
			key_char, //start pos
			alphabet);

		ngram_occurrence_flat_list&  shiftes_letter_occ = stats_interleaved_by_key_char.occurence; // for readenes
		ngram_frequensy_flat_list    expected_letters_occ(shiftes_letter_occ.size());

		//fill expected letters count in c
		for (size_t i = 0; i < shiftes_letter_occ.size(); ++i)
			expected_letters_occ[i] = expected_letters_frequecies[i] * stats_interleaved_by_key_char.text_letters_count;

		std::map<double, size_t > ordered_fitness;

		for (size_t shift = 1; shift < alphabet.size(); ++shift) {
			// using the special iterator to compare the coset in a thifted manner with the etalon letter stats
			auto shifted_range = cycle_range(1, shiftes_letter_occ.begin(), shiftes_letter_occ.begin() + shift, shiftes_letter_occ.end());
			// common stats    S = [S1,     S2, ... , Sn-2,   Sn-1,   Sn]
			// shisftes coaset C = [C1+s, C2+s, ... ,   Cn,   Cs-1, Cs-2]
 			// where n = size of C and S, and  Cn/Sn = end , C1/S1 = begin
			// and 0 < s < n  = shift
			// calculate a sum of chi_squared_stat results on pairs of common stats el and corresponding shifted coset 
			double fitness = 0;
			fitness = std::inner_product(shifted_range.first, shifted_range.second, expected_letters_occ.begin(), fitness, std::plus<double>(),  chi_squared_stat);

			//store fitness value with the shift witch used in cals
			// in order from best at begining of container to worst at the end of container 
			ordered_fitness[fitness] = shift;
		}
		// get the best scored by fintess shift and assign it to the sorrespondong key letter
		key[key_char] = alphabet.letter_from_index(ordered_fitness.begin()->second); 
	}

	return key;
}
