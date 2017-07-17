#include <iostream>   //console io
#include <iterator>
#include <string>     //string handling
#include <chrono>

#include "misc_utils.h"
#include "vigenere_cipher.h"
#include "text_utils.h"

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

int main()
{
	std::string original_text    = "attack at dawn";
	std::string exp_encoded_text = "lxfopv ef rnhr";
	std::string key                   = "lemon";

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

	/*
	std::string test_text = "As you type in this text box, the graph below will update automatically, (assuming you have javascript enabled in your browser).";
	std::vector<unsigned int> f_expected = { 2, 2, 0, 0, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 0, 0 };
	auto t0 = std::chrono::high_resolution_clock::now();
	auto a = alphabet(std::string("abcd"));
	auto f_actual = text_que_freq(2, test_text);
	auto aaa = flat_freq_to_dict<std::string, char, decltype(f_actual.alphabet)>(f_actual);
	std::cout << "expected freq for text '" << test_text << "' : " << f_expected << std::endl;
    std::cout << "  actual freq for text '" << test_text << "' : " << f_actual.frequencies << std::endl;
	std::cout << "  actual map for text '" << test_text << "' : " << aaa << std::endl;
	*/
	return 0;
}