#ifdef JHC_NOT_HEADER_ONLY
#include "../base64.hpp"
#endif

std::string jhc::Base64::Encode(std::string const& s, bool url) {
	return _Encode(s, url);
}

std::string jhc::Base64::EncodeWithPEM(std::string const& s) {
	return _EncodeWithPEM(s);
}

std::string jhc::Base64::EncodeWithMIME(std::string const& s) {
	return _EncodeWithMIME(s);
}

std::string jhc::Base64::Decode(std::string const& s, bool remove_linebreaks) {
	return _Decode(s, remove_linebreaks);
}

std::string jhc::Base64::Encode(unsigned char const* bytes_to_encode, size_t in_len, bool url) {
	const size_t len_encoded = (in_len + 2) / 3 * 4;
	const unsigned char trailing_char = url ? '.' : '=';

	//
	// Choose set of base64 characters. They differ
	// for the last two positions, depending on the url
	// parameter.
	// A bool (as is the parameter url) is guaranteed
	// to evaluate to either 0 or 1 in C++ therefore,
	// the correct character set is chosen by subscripting
	// base64_chars with url.
	//
	const char* base64_chars = base64Chars(url ? 1 : 0);

	std::string ret;
	ret.reserve(len_encoded);

	unsigned int pos = 0;

	while (pos < in_len) {
		ret.push_back(base64_chars[(bytes_to_encode[pos + 0] & 0xfc) >> 2]);

		if (pos + 1 < in_len) {
			ret.push_back(base64_chars[((bytes_to_encode[pos + 0] & 0x03) << 4) + ((bytes_to_encode[pos + 1] & 0xf0) >> 4)]);

			if (pos + 2 < in_len) {
				ret.push_back(base64_chars[((bytes_to_encode[pos + 1] & 0x0f) << 2) + ((bytes_to_encode[pos + 2] & 0xc0) >> 6)]);
				ret.push_back(base64_chars[bytes_to_encode[pos + 2] & 0x3f]);
			}
			else {
				ret.push_back(base64_chars[(bytes_to_encode[pos + 1] & 0x0f) << 2]);
				ret.push_back(trailing_char);
			}
		}
		else {
			ret.push_back(base64_chars[(bytes_to_encode[pos + 0] & 0x03) << 4]);
			ret.push_back(trailing_char);
			ret.push_back(trailing_char);
		}

		pos += 3;
	}

	return ret;
}

#if __cplusplus >= 201703L
std::string jhc::Base64::Encode(std::string_view s, bool url) {
	return _Encode(s, url);
}

std::string jhc::Base64::EncodeWithPEM(std::string_view s) {
	return _EncodeWithPEM(s);
}

std::string jhc::Base64::EncodeWithMIME(std::string_view s) {
	return _EncodeWithMIME(s);
}

std::string jhc::Base64::Decode(std::string_view s, bool remove_linebreaks) {
	return _Decode(s, remove_linebreaks);
}
#endif  // __cplusplus >= 201703L

const char* jhc::Base64::base64Chars(int index) {
	//
	// Depending on the url parameter in base64_chars, one of
	// two sets of base64 characters needs to be chosen.
	// They differ in their last two characters.
	//
	static const char* base64_chars[2] = {
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789"
		"+/",
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789"
		"-_" };
	return base64_chars[index];
}

unsigned int jhc::Base64::getPosOfChar(const unsigned char chr) {
	//
	// Return the position of chr within base64_encode()
	//

	if (chr >= 'A' && chr <= 'Z')
		return chr - 'A';
	else if (chr >= 'a' && chr <= 'z')
		return chr - 'a' + ('Z' - 'A') + 1;
	else if (chr >= '0' && chr <= '9')
		return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
	else if (chr == '+' || chr == '-')
		return 62;  // Be liberal with input and accept both url ('-') and non-url ('+') base 64 characters (
	else if (chr == '/' || chr == '_')
		return 63;  // Ditto for '/' and '_'
	else
		//
		// 2020-10-23: Throw std::exception rather than const char*
		//(Pablo Martin-Gomez, https://github.com/Bouska)
		//
		throw std::runtime_error("Input is not valid base64-encoded data.");
}

std::string jhc::Base64::insertLineBreaks(std::string str, size_t distance) {
	//
	// Provided by https://github.com/JomaCorpFX, adapted by me.
	//
	if (!str.length()) {
		return "";
	}

	size_t pos = distance;

	while (pos < str.size()) {
		str.insert(pos, "\n");
		pos += distance + 1;
	}

	return str;
}