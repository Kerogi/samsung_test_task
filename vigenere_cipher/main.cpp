#include <iostream> 
#include <iterator>

#include "misc_utils.h"
#include "vigenere_cipher.h"
#include "text_utils.h"

void test_vigenere()
{
	std::string original_text = "Attack at dawn!";
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

}


int main(int argc, char* argv[])
{
	//test_vigenere();

	if (argc > 1) {
		std::string key(argv[1]);

		if (argc > 2) {
			std::string act_decoded_text;
			decrypt_vigenere(key,
				std::istreambuf_iterator<char>(std::cin),
				std::istreambuf_iterator<char>(),
				std::ostream_iterator<char>(std::cout)
			);
		} else {
			encrypt_vigenere(key,
				std::istreambuf_iterator<char>(std::cin),
				std::istreambuf_iterator<char>(),
				std::ostream_iterator<char>(std::cout)
			);
		}
	} else {
		std::cout << R"(Util for encrypt/decrypt input text by given key using vigenere chipher and output
Usage:
	vigenere_cipher KEY [decrypt]
	where KEY - is encryption/decryption key
	decrypt - optional arg presence of whitch switches to decryption mode (any arg will accepted))

inputs from stdin
outputs to stdout
)" << std::endl;
	}
}