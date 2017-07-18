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
std::set<int> divisors(int num, int start = 3)
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
	std::string hamlet_orig(hamlet);
	std::string hamlet_key("key");
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
	std::cout << "Common English letters frequeies:" << std::endl;

	for (size_t i = 0; i < en_letter_count.frequencies.size(); ++i) {
		en_letter_freq[i] = double(en_letter_count.frequencies[i]) / double(en_letter_count.text_letters_count);
		cout << "\t" << en.letter_from_index(i) << ": " << en_letter_freq[i] << endl;
		total_letters_freq += en_letter_freq[i];
	}
	cout << string(100, '-') << endl;

	vector<int> key_numbers(hamlet_key.length());
	for (size_t i = 0; i < hamlet_key.length(); ++i) { key_numbers[i] = en.index_from_letter(hamlet_key[i]); }
	std::cout << "Actual Key: |" << hamlet_key.length() << "| '" << hamlet_key << "' " << key_numbers << std::endl;


	cout << string(100, '-') << endl;
	
	bool key_found = false;
	size_t found_key_length = 0;
	size_t max_key_length = 12;


	auto end = hamlet_orig.end();
	std::string hamlet_encoded;
	std::string hamlet_encoded_only_alphabet_chars;

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

	std::cout << "Text length: " << hamlet_encoded_only_alphabet_chars.length() << endl;
	size_t found_key_length1 = guess_key_length_ci(cout, hamlet_encoded_only_alphabet_chars, ci_en, max_key_length, en);
	size_t found_key_length2 = guess_key_length_by_shifted_coincedences(cout, hamlet_encoded_only_alphabet_chars, max_key_length, en);
	//std::cout << " Possible key lengths: " << found_key_length1 << " (most common diviser of the best CI) " << found_key_length2 << " (most common diviser of most coincedence periods)" << endl;
	std::cout << " Possible key lengths: " << found_key_length1 << "  " << found_key_length2;

	if (found_key_length1 && found_key_length2) {

		found_key_length = gcd(found_key_length1, found_key_length2);

		//std::cout << "Key length: " << gcd(found_key_length1, found_key_length2) << " (most common diviser of posiible key 1 and possible key 2)" << endl;

		if (found_key_length == hamlet_key.length()) {
			std::cout << " found propper Key length = " << found_key_length;
			key_found = true;
		}
		else {
			std::cout << " found wrong Key length = " << found_key_length;

		}
	}
	else {
		std::cout << " cant guess Key length";
	}
	std::cout << endl;

	if (key_found) {
		auto key = guess_key(cout, hamlet_encoded_only_alphabet_chars, found_key_length, en_letter_freq, en);
		std::cout << " guess Key: " << key << endl;
	}
	
	//for (size_t min_guessable_text_length = hamlet_key.length(); min_guessable_text_length < hamlet_orig.length()/2 && !key_found; ++min_guessable_text_length) {
	//	auto end = hamlet_orig.begin();
	//	advance(end, min_guessable_text_length);
	//	std::string hamlet_encoded;
	//	std::string hamlet_encoded_only_alphabet_chars;

	//	encrypt_vigenere(hamlet_key,
	//		hamlet_orig.begin(),
	//		end,
	//		std::back_inserter(hamlet_encoded)
	//	);
	//	std::copy_if(hamlet_encoded.begin(),
	//		hamlet_encoded.end(),
	//		std::back_inserter(hamlet_encoded_only_alphabet_chars),
	//		[&en](const char& ch) {
	//		return en.contains(ch);
	//	}
	//	);

	//	size_t max_key_length = 12;
	//	std::cout << "Text length: " << min_guessable_text_length << endl;
	//	size_t found_key_length1 = guess_key_length_ci(cout, hamlet_encoded_only_alphabet_chars, ci_en, max_key_length, en);
	//	size_t found_key_length2 = guess_key_length_by_shifted_coincedences(cout, hamlet_encoded_only_alphabet_chars, max_key_length, en);
	//	//std::cout << " Possible key lengths: " << found_key_length1 << " (most common diviser of the best CI) " << found_key_length2 << " (most common diviser of most coincedence periods)" << endl;
	//	std::cout << " Possible key lengths: " << found_key_length1 << "  " << found_key_length2;

	//	if (found_key_length1 && found_key_length2) {

	//		size_t found_key_length = gcd(found_key_length1, found_key_length2);

	//		//std::cout << "Key length: " << gcd(found_key_length1, found_key_length2) << " (most common diviser of posiible key 1 and possible key 2)" << endl;

	//		if (found_key_length == hamlet_key.length()) {
	//			std::cout << " found propper Key length = " << found_key_length;
	//			key_found = true;
	//		}
	//		else {
	//			std::cout << " found wrong Key length = " << found_key_length;

	//		}
	//	}
	//	else {
	//		std::cout << " cant guess Key length";
	//	}

	//	std::cout << endl;
	//}

	return 0;
}
