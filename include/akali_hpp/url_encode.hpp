#ifndef AKALI_URL_ENCODE_HPP__
#define AKALI_URL_ENCODE_HPP__
#include <string>
#include "hex_encode.hpp"

namespace akali_hpp {
class UrlEncode {
   public:
    static std::string Encode(const std::string& str) {
        char hex[] = "0123456789ABCDEF";
        std::string dst;

        for (size_t i = 0; i < str.size(); ++i) {
            unsigned char cc = str[i];

            if (cc >= 'A' && cc <= 'Z' || cc >= 'a' && cc <= 'z' || cc >= '0' && cc <= '9' || cc == '.' ||
                cc == '_' || cc == '-' || cc == '*' || cc == '~') {
                dst += cc;
            }
            else {
                unsigned char c = static_cast<unsigned char>(str[i]);
                dst += '%';
                dst += hex[c / 16];
                dst += hex[c % 16];
            }
        }

        return dst;
    }

    static size_t Decode(char* buffer, size_t buflen, const char* source, size_t srclen) {
        if (nullptr == buffer)
            return srclen + 1;

        if (buflen <= 0)
            return 0;

        unsigned char h1, h2;
        size_t srcpos = 0, bufpos = 0;

        while ((srcpos < srclen) && (bufpos + 1 < buflen)) {
            unsigned char ch = source[srcpos++];

            if (ch == '+') {
                buffer[bufpos++] = ' ';
            }
            else if ((ch == '%') && (srcpos + 1 < srclen) && HexEncode::Decode(source[srcpos], &h1) &&
                     HexEncode::Decode(source[srcpos + 1], &h2)) {
                buffer[bufpos++] = (h1 << 4) | h2;
                srcpos += 2;
            }
            else {
                buffer[bufpos++] = ch;
            }
        }

        buffer[bufpos] = '\0';
        return bufpos;
    }

    static std::string Decode(const std::string& source) { return s_transform(source, Decode); }

   private:
    // Apply any suitable string transform (including the ones above) to an STL
    // string. Stack-allocated temporary space is used for the transformation, so
    // value and source may refer to the same string.
    typedef size_t (*Transform)(char* buffer, size_t buflen, const char* source, size_t srclen);

    // Return the result of applying transform t to source.
    static std::string s_transform(const std::string& source, Transform t) {
        // Ask transformation function to approximate the destination size (returns upper bound)
        size_t maxlen = t(nullptr, 0, source.data(), source.length());
        char* buffer = static_cast<char*>(::malloc((maxlen) * sizeof(char)));
        if (!buffer)
            return "";
        const size_t len = t(buffer, maxlen, source.data(), source.length());
        std::string result(buffer, len);
        free(buffer);
        return result;
    }
};
}  // namespace akali_hpp

#endif  // !AKALI_URL_ENCODE_HPP__