#include <iostream>   //console io
#include <iterator>
#include <string>     //string handling
#include <chrono>
#include <iomanip>
#include <numeric>
#include "misc_utils.h"
#include "vigenere_cipher.h"
#include "text_utils.h"
#include "lorem_ipsum.h"

/*
template<size_t ss, size_t as = 26>
std::vector<size_t> freq(const char* t, size_t tl) {
	//todo: use an alphabet as argument
	// then use it as a reference instead isalpha 
	std::vector<size_t> f(std::pow(as, ss));
	std::vector<char> s(ss);//todo: for the simplicity use a queue and push pop
	size_t off = 0;
	size_t sl = 0;
	for (size_t ti = 0; ti < tl; ++ti) {
		char c = t[ti];
		if (::isalpha(c)) {//todo: use an alphabet as argumen
			s[off] = c;
			off = (off + 1) % ss;
			++sl;
			if (sl >= ss) {
				size_t fi = 0;
				for (size_t si = 0; si < ss; ++si) {
					fi += (s[(si + off) % ss] - 'a') * std::pow(as, ss - si - 1);
				}
				//cout << "fi: " << fi << endl;
				f[fi] += 1;
			}
		}
		else {
			sl = 0;
		}
		//cout << s << ' ' << "sl: " << sl << " ss: " << ss << endl;
	}


	return f;
}
*/


void test_vigenere()
{
	std::string original_text = "attack at dawn";
	std::string exp_encoded_text = "lxfopv ef rnhr";
	std::string key = "lemon";

	std::string act_encoded_text;

	encrypt_vigenere(key,
		original_text.begin(),
		original_text.end(),
		std::back_inserter(act_encoded_text)
	);

	std::string act_decoded_text;
	decrypt_vigenere(key,
		act_encoded_text.begin(),
		act_encoded_text.end(),
		std::back_inserter(act_decoded_text)
	);
	std::cout << "         original text '" << original_text << "'" << std::endl;
	std::cout << " expected encoded text '" << exp_encoded_text << "'" << std::endl;
	std::cout << "   actual encoded text '" << act_encoded_text << "'" << std::endl;
	std::cout << "   actual decoded text '" << act_decoded_text << "'" << std::endl;

	std::string hamlet_key("key");
	std::string hamlet_encodedy(hamlet_encoded_by_key);
	std::string hamlet_decoded;
	std::string hamlet_orig(hamlet);

	decrypt_vigenere(hamlet_key,
		hamlet_encodedy.begin(),
		hamlet_encodedy.end(),
		std::back_inserter(hamlet_decoded)
	);

	if (hamlet_orig != hamlet_decoded) {
		std::cout << "decoded hamlet != original:\n\n" << hamlet_decoded << "\n\n" << hamlet_orig << std::endl;
	} else {
		std::cout << "decoded hamlet == original " << std::endl;
	}

}


void measure_text_frequency_tools()
{
	std::string big_text(lorem_ipsum_wc2000);

	size_t old_ss = 0;
	std::vector<int64_t> que_durations;
	std::vector<int64_t> seq_durations;
	size_t ss = 0;
	for (size_t s : {1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, }) {
		ss = s;

		auto t0 = std::chrono::high_resolution_clock::now();
		auto f_que = count_ngram_occurence(ss, big_text);
		auto t1 = std::chrono::high_resolution_clock::now();
		auto f_seq = text_seq_freq(ss, big_text);
		auto t2 = std::chrono::high_resolution_clock::now();
		int64_t que_duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
		int64_t seq_duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
		if (ss != old_ss && !que_durations.empty()) {
			int64_t que_duration_avg = 0;
			for (auto d : que_durations) que_duration_avg += d;
			que_duration_avg /= que_durations.size();

			int64_t seq_duration_avg = 0;
			for (auto d : seq_durations) seq_duration_avg += d;
			seq_duration_avg /= seq_durations.size();
			bool f_eq = f_que.frequencies == f_seq.frequencies;
			std::cout << "f_que " << ((f_eq) ? "==" : "!=") << " f_seq " << f_que.frequencies.size() << std::endl;
			std::cout << "counting frequencies of " << old_ss << " letters for text of length " << big_text.length() << " chars using deque took:  " << que_duration_avg << " microsec " << std::endl;
			std::cout << "counting frequencies of " << old_ss << " letters for text of length " << big_text.length() << " chars using vector took: " << seq_duration_avg << " microsec " << std::endl;

			auto t3 = std::chrono::high_resolution_clock::now();
			auto fl_map = flat_freq_to_freq_dict(f_seq.frequencies, f_seq.sequence_length, f_seq.alphabet);
			auto t4 = std::chrono::high_resolution_clock::now();
			int64_t map_freq_convert_duration = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
			std::cout << "convert flat frequencies |" << f_que.frequencies.size() << "| to mapped by frequency |" << fl_map.size() << "| took: " << map_freq_convert_duration << " microsec " << std::endl;

			auto t5 = std::chrono::high_resolution_clock::now();
			auto lf_map = flat_freq_to_letters_dict(f_seq.frequencies, f_seq.sequence_length, f_seq.alphabet);
			auto t6 = std::chrono::high_resolution_clock::now();
			int64_t map_letters_convert_duration = std::chrono::duration_cast<std::chrono::microseconds>(t6 - t5).count();
			std::cout << "convert flat frequencies |" << f_que.frequencies.size() << "| to mapped by letters |" << lf_map.size() << "| took: " << map_letters_convert_duration << " microsec " << std::endl;

			que_durations.clear();
			seq_durations.clear();
		}

		que_durations.push_back(que_duration);
		seq_durations.push_back(seq_duration);

		old_ss = ss;
	}
	int64_t que_duration_avg = 0;
	for (auto d : que_durations) que_duration_avg += d;
	que_duration_avg /= que_durations.size();

	int64_t seq_duration_avg = 0;
	for (auto d : seq_durations) seq_duration_avg += d;
	seq_duration_avg /= seq_durations.size();

	std::cout << "counting frequencies of " << ss << " letters for text of length " << big_text.length() << " chars using deque took:  " << que_duration_avg << " microsec " << std::endl;
	std::cout << "counting frequencies of " << ss << " letters for text of length " << big_text.length() << " chars using vector took: " << seq_duration_avg << " microsec " << std::endl;
}

template<typename It, typename Pred>
void for_each_uniq_pair(It begin, It end, Pred pred) {
	for (It i = begin; i != end; ++i) {
		for (It j = i; ++j != end; ) {
			pred(*i, *j);
		}
	}
}

int gcd(int num1, int num2)
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
std::set<int> divisors(int num, int start=3)
{
	std::set<int> s;
	for (int i = start; i <= num; ++i) {
		if (num % i == 0) s.insert(i);
	}
	return s;
}

int main()
{
	using namespace std;
	//test_vigenere();

	//measure_text_frequency_tools();

	english_case_less en;
	std::string common_text(common_english_text);
	std::string common_text_only_alphabet;
	std::string hamlet_encoded;
	std::string hamlet_encoded_only_alphabet_chars;
	std::string hamlet_orig(hamlet);
	std::string hamlet_key("hello");
	std::copy_if(common_text.begin(),
		common_text.end(),
		std::back_inserter(common_text_only_alphabet),
		[&en](const char& ch) {
		return en.contains(ch);
	}
	);
	const double ci_en = text_index_of_coincidence(count_letter_occurence(common_text_only_alphabet, 1, 0, en));
	std::cout << "Common English CI: " << setprecision(10) << ci_en << std::endl;

	auto en_letter_count = count_letter_occurence(common_text_only_alphabet, 1, 0, en);
	std::vector<double>  en_letter_freq(en.size());
	double total_letters_freq = 0;
	std::cout << "Common English letters frequeies:"<< std::endl;

	for (size_t i = 0; i < en_letter_count.frequencies.size(); ++i) { 
		en_letter_freq[i] = double(en_letter_count.frequencies[i])/double(en_letter_count.text_letters_count);
		cout << "\t" <<en.letter_from_index(i) << ": " << en_letter_freq[i] << endl;
		total_letters_freq += en_letter_freq[i];
	}
	cout << string(100, '-') << endl;

	vector<int> key_numbers(hamlet_key.length());
	for (size_t i = 0; i < hamlet_key.length(); ++i) { key_numbers[i] = en.index_from_letter(hamlet_key[i]); }
	std::cout << "Actual Key: |"<< hamlet_key.length()<<"| '" << hamlet_key<< "' " << key_numbers << std::endl;
	cout << string(100, '-') << endl;
	auto end = hamlet_orig.end();
	//advance(end, 100);
	encrypt_vigenere(hamlet_key,
		hamlet_orig.begin(),
		end,
		std::back_inserter(hamlet_encoded)
	);
	std::copy_if(hamlet_encoded.begin(),
		hamlet_encoded.end(),
		std::back_inserter(hamlet_encoded_only_alphabet_chars),
		[&en](const char& ch) {
			return en.contains(ch);
		}
		);

	//auto text_occ = get_ngram_positions(3, hamlet_encoded_only_alphabet_chars, en);

	//auto fl_map = flat_freq_to_freq_dict(text_occ.frequencies, text_occ.sequence_length, text_occ.alphabet);
	//std::map<std::string, std::set<size_t>> distances_by_ngrams;
	//std::map<std::string, std::set<size_t>> gdc_distances_by_ngrams;
	//auto end_at_30perc = fl_map.begin();
	//std::advance(end_at_30perc, 2);
	//for (auto it = fl_map.begin(); it != end_at_30perc; ++it) {
	//	for (const std::string& ngram : it->second) {
	//		size_t flat_idx = en.flat_index_from_ngram(ngram);

	//		//for (std::vector<unsigned int>::iterator iter1 = text_occ.occurence[flat_idx].begin(),
	//		//	iter2 = text_occ.occurence[flat_idx].begin() + 1;
	//		//	iter2 != text_occ.occurence[flat_idx].end(); 
	//		//	++iter1, ++iter2) {
	//		//	distances_by_ngrams[ngram].insert(std::abs((int)*iter2 - (int)*iter1));

	//		//}
	//		
	//		for_each_uniq_pair(text_occ.occurence[flat_idx].begin(),
	//			text_occ.occurence[flat_idx].end(),
	//		[&](int a, int b) {
	//			distances_by_ngrams[ngram].insert(std::abs(b-a));
	//		}
	//		);
	//		
	//		/*
	//		for_each_uniq_pair(distances_by_ngrams[ngram].begin(),
	//			distances_by_ngrams[ngram].end(),
	//			[&](int a, int b) {
	//			gdc_distances_by_ngrams[ngram].insert(gcd(a,b));
	//		}
	//		);*/
	//		for( auto d: distances_by_ngrams[ngram]){
	//			//std::cout << d << ":" << divisors(d) << std::endl;
	//		}
	//	}
	//}
	//std::cout << distances_by_ngrams << std::endl;
	//std::cout << gdc_distances_by_ngrams << std::endl;
	size_t max_key_length = 12;
	auto best_ci_for_lang = [&ci_en]
	(const double& l_ci, const double& r_ci) -> bool { 
		return std::abs(l_ci  - ci_en) < std::abs(r_ci - ci_en);
	};
	std::map<double, int, decltype(best_ci_for_lang) > ci_interleaved_for_keys_lengths(best_ci_for_lang);

	auto t0 = std::chrono::high_resolution_clock::now();
	for (size_t key_length = 1; key_length <= max_key_length; ++key_length) {
		double ci_avg = 0;
		for (size_t shift = 0; shift < 1; ++shift) {
			double ci = text_index_of_coincidence(count_letter_occurence(hamlet_encoded_only_alphabet_chars, key_length, shift, en));
			ci_avg = (ci + (ci_avg*shift))/(shift+1);
		}
		ci_interleaved_for_keys_lengths[ci_avg] = key_length;
	}
	cout << "CI interleaved for keys lengths from 1 to " << max_key_length << endl;
	double max_ci = max_element(ci_interleaved_for_keys_lengths.begin(), ci_interleaved_for_keys_lengths.end())->first;
	for (const auto& kv : ci_interleaved_for_keys_lengths) {
		cout << "\tfor each " <<setw(2) << kv.second << "-nth char CI: " << setw(14)<< kv.first << "( +/-"<< setw(14)<<abs(ci_en - kv.first) <<" from common) " << " [" << setw(10) << string(10 * (kv.first / max_ci), '|') << "]"<< endl;
	}
	int found_key_length1 = -1;
	{
		auto iter1 = ci_interleaved_for_keys_lengths.begin();
		auto iter2 = ci_interleaved_for_keys_lengths.begin();
		std::advance(iter2, 1);
		for (; iter2 != ci_interleaved_for_keys_lengths.end(); ++iter1, ++iter2) {
			if (iter1->second < iter2->second) {
				found_key_length1 = iter1->second;
				break;
			}
			int cd = gcd(iter1->second, iter2->second);
			if (cd >= iter2->second) {
				found_key_length1 = cd;
				break;
			}
		}
	}
	shifted_coincedences_prop_list sh_c = count_shifted_letters_coincedences_prop_multi(hamlet_encoded_only_alphabet_chars, min(max_key_length*max_key_length, hamlet_encoded_only_alphabet_chars.length()), en);
	std::cout << "Shifted text chars proportinal coincedences: "<< std::endl;

	std::map<double, int, std::greater<double> > best_shifted_c;
	double max_co_prop = *max_element(sh_c.begin() + 1, sh_c.end());
	double tot = 0;
	tot = accumulate(sh_c.begin() + 1, sh_c.end(), tot);
	double wtot = 0;
	wtot = accumulate(sh_c.begin() + 1, sh_c.end(), wtot, [max_co_prop](double v, double x) {return v + x * (x/ max_co_prop); });
	double wsum = 0;
	wsum = accumulate(sh_c.begin() + 1, sh_c.end(), wsum, [max_co_prop](double v, double x) {return v + (x / max_co_prop); });
	double avg = tot / (sh_c.size()-1);
	double wavg = wtot / wsum;//// tot / (sh_c.size() - 1);
	std::vector<int> best_period;
	for (int i = 1; i < sh_c.size(); ++i) {
		double x = sh_c[i];
		char symb = (x > wavg) ? '+' : '-';
		int bar_length = 20;
		cout << "\tfor shift " << setw(2) << i << ": " << setw(14) << x << " [" << setw(bar_length) << string(bar_length * (x / max_co_prop), symb) << "] " << endl;
		//cout <<" [" << setw(10) << string(10 * ((x*x) / (max_co_prop*max_co_prop)), '|') << "]"<< endl;
		//	cout << "x/max: " << setw(14) << x / max_co_prop << ", x/avg: " << setw(14) << x / avg << ", x>awg: " << (x > avg) << ", x/wavg: " << setw(14) << x / wavg << ", x>wawg: " << (x > wavg) << endl;
		if (x > wavg) best_period.push_back(i);
	}
	cout << " avg: " << avg << endl;
	cout << " wavg: " << wavg << endl;
	int found_key_length2 = -1;
	{
		std::map<int, int> count_distances_between_best;
		auto iter1 = best_period.begin();
		auto iter2 = best_period.begin();
		auto end = best_period.end();
		//std::advance(iter1, 1);
		std::advance(iter2, 1);
		//std::advance(end, best_shifted_c);
		for (; iter2 != end; ++iter1, ++iter2) {
			count_distances_between_best[*iter2 - *iter1] += 1;
		}
		found_key_length2 = max_element(count_distances_between_best.begin(), count_distances_between_best.end(), [](const auto& kvl, const auto& kvr) {return kvl.second < kvr.second; })->first;
	}
    auto t1 = std::chrono::high_resolution_clock::now();


	std::cout << "Possible key length: " << found_key_length1 << " (most common diviser of the best CI)" << endl;
	std::cout << "Possible key length: " << found_key_length2 << " (most common diviser of most coincedence periods)" << endl;
	std::cout << "Possible key length: " << gcd(found_key_length1, found_key_length2) << " (most common diviser of posiible key 1 and possible key 2)" << endl;
	cout << "Calculation took " << std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() << std::endl;
	/*for (auto p : ci_interleaved_for_keys_lengths) {
		std::cout << "     " << std::setw(3) << p.second << ": " << p.first << std::endl;
	}*/

	return 0;
}
