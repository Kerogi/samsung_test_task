#include <iostream>   //console io
#include <iterator>
#include <string>     //string handling
#include <chrono>
#include <iomanip>
#include <numeric>
#include <sstream>

#include "misc_utils.h"
#include "vigenere_cipher.h"
#include "text_utils.h"
#include "lorem_ipsum.h"


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

template<typename IterInT, typename IterOutT>
bool dechipher_vigenere(size_t max_key_length, IterInT begin, IterInT end, IterOutT dst, std::ostream& os)
{
	std::string chiper_text(begin, end); // save to restore non alphabet chars and case

	// BEGIN some autoinit
	// todo move to static place
	// todo move to more general class Language (Alphabet + Common stats)
		english_case_less en;
		auto in_en_alphabet = [&en](const char ch) { return en.contains(ch); };
		std::string common_text(common_english_text);
		std::string common_text_only_alphabet;
		std::copy_if(common_text.begin(), common_text.end(), std::back_inserter(common_text_only_alphabet), in_en_alphabet);
		const double common_CI = text_index_of_coincidence(count_letter_occurence(common_text_only_alphabet, 1, 0, en));
		auto common_letter_occurence = count_letter_occurence(common_text_only_alphabet, 1, 0, en);

		std::vector<double>  common_letter_frequency(en.size());
		double total_letters_freq = 0;

		for (size_t i = 0; i < common_letter_occurence.frequencies.size(); ++i) {
			common_letter_frequency[i] = double(common_letter_occurence.frequencies[i]) / double(common_letter_occurence.text_letters_count);
			total_letters_freq += common_letter_frequency[i];
		}
	// END 

	
	std::string chiper;
	//clean up text
	std::copy_if(chiper_text.begin(), chiper_text.end(), std::back_inserter(chiper), in_en_alphabet);

	//try different was to estimate key lengths
	size_t found_key_length1 = guess_key_length_ci(os, chiper, common_CI, max_key_length*2, en);
	size_t found_key_length2 = guess_key_length_by_shifted_coincedences(os, chiper, max_key_length*2, en);

	if (found_key_length1 || found_key_length2) {
		// have some keys

		std::set<size_t> key_lengths;

		// filter the same lengths
		key_lengths.insert(found_key_length1);
		key_lengths.insert(found_key_length2);
		key_lengths.insert(gcd(found_key_length1, found_key_length2));
		// filter the nonsence lengths 
		key_lengths.erase(1);
		key_lengths.erase(0);

		//try different key kength from smallest ones
		for (size_t key_length : key_lengths) {
			
			auto guessed_key = guess_key(os, chiper, key_length, common_letter_frequency, en);


		    //try to decode text
			std::string decoded_text;
			decrypt_vigenere(guessed_key,
				chiper.begin(),
				chiper.end(),
				std::back_inserter(decoded_text)
			);
			// and couts its CI coff
			const double decrypt_CI = text_index_of_coincidence(count_letter_occurence(decoded_text, 1, 0, en));
			os << "key: " << guessed_key << std::endl;
			os << "decrypt_CI: " << decrypt_CI << std::endl;
			os<< "common_CI: " << common_CI << std::endl;
			os << "diff: " << std::abs(decrypt_CI - common_CI) << std::endl;

			//if CI of decoded key with sample test is good enought accept key 
			if (std::abs(decrypt_CI - common_CI) < 0.01) {

				// out key to destination 
				std::copy(guessed_key.begin(), guessed_key.end(), dst);
				*dst++ = '\n';
				// out decoded text with all chars and proper case
				decrypt_vigenere(guessed_key,
					chiper_text.begin(),
					chiper_text.end(),
					dst
				);

				return true;
			} else {
				continue;
			}

		}
		std::cerr << " nomore good keys " << std::endl;
	} else {
		std::cerr << " cant guess Key length " << std::endl;;
	}

	return false;
}

int main(int argc, char* argv[])
{
	std::stringstream dummy_os;
	bool res = false;
	// if arga more than 1 out debug info to cerr 
	if (argc > 1) {
		res = dechipher_vigenere(12, 
			std::istreambuf_iterator<char>(std::cin),
			std::istreambuf_iterator<char>(),
			std::ostream_iterator<char>(std::cout),
			std::cerr
		);
	} else {
		res = dechipher_vigenere(12,
			std::istreambuf_iterator<char>(std::cin),
			std::istreambuf_iterator<char>(),
			std::ostream_iterator<char>(std::cout),
			dummy_os
		);
	}

	return (res)?0:1;
}
