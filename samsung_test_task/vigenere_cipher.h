#pragma once
#include <string>
#include <algorithm>
#include "text_utils.h"

inline int modulo(int a, int b) {
	const int result = a % b;
	return result >= 0 ? result : result + b;
}

template<class AlphabetT = english_case_less>
struct vigenere_cipher
{
	const AlphabetT& alphabet;

	using string_t = typename AlphabetT::string_t;
	using char_t = typename AlphabetT::char_t;
	string_t key;
	mutable size_t keyIndex;
	enum Op {
		Encrypt,
		Decrypt
	} operation;
	int key_shift_mult = 1;

	vigenere_cipher(const AlphabetT& alphabet, const string_t& key, Op operation = Op::Encrypt)
		: alphabet(alphabet)
		, key(key)
		, keyIndex(0)
		, operation(operation)
		, key_shift_mult((operation == Op::Encrypt)?1:-1)
	{
	}

	vigenere_cipher(const string_t& key, Op operation = Op::Encrypt)
		: alphabet(AlphabetT())
		, key(key)
		, keyIndex(0)
		, operation(operation)
		, key_shift_mult((operation == Op::Encrypt) ? 1 : -1)
	{
	}

	char_t operator()(const char_t& ch) const
	{
		if (alphabet.contains(ch)) {
			int ch_idx = (int) alphabet.index_from_letter(ch);
			int key_idx_shift = alphabet.index_from_letter(key[keyIndex]) * key_shift_mult ;
			int enc_ch_idx = modulo(ch_idx + key_idx_shift, alphabet.size());
			keyIndex = (keyIndex + 1) % key.size();
			return alphabet.letter_from_index(enc_ch_idx);
		} else {
			return ch;
		}
	}
};

template<class NextOpT>
struct reset_case
{
	NextOpT op;
	char operator()(const char& ch) const
	{
		if (::isalpha(ch)) {
			bool upper = ::isupper(ch);
			char trans_ch = op(ch);
			if (upper) {
				return ::toupper(trans_ch);
			} else {
				return ::tolower(trans_ch);
			}
		} else {
			return op(ch);
		}
	}
};

template<class NextOpT>
reset_case<NextOpT> perserve_case(const NextOpT& op) {
	return { op };
}

template<typename IterInT, typename IterOutT>
void encrypt_vigenere(std::string const& key, IterInT begin, IterInT end, IterOutT dst)
{
	std::transform(begin, end, dst, perserve_case(vigenere_cipher<english_case_less>(english_case_less(), key)));
}

template<typename IterInT, typename IterOutT>
void decrypt_vigenere(std::string const& key, IterInT begin, IterInT end, IterOutT dst)
{
	std::transform(begin, end, dst, perserve_case(vigenere_cipher<english_case_less>(english_case_less(), key, vigenere_cipher<english_case_less>::Decrypt)));
}
