#include "jhc/config.hpp"

#ifdef JHC_NOT_HEADER_ONLY
#include "../hex_encode.hpp"
#endif

namespace jhc {
char HexEncode::Encode(unsigned char val) {
    const char HEX[] = "0123456789abcdef";
    assert(val < 16);
    return (val < 16) ? HEX[val] : '!';
}

bool HexEncode::Decode(char ch, unsigned char* val) {
    if ((ch >= '0') && (ch <= '9')) {
        *val = ch - '0';
    }
    else if ((ch >= 'A') && (ch <= 'Z')) {
        *val = (ch - 'A') + 10;
    }
    else if ((ch >= 'a') && (ch <= 'z')) {
        *val = (ch - 'a') + 10;
    }
    else {
        return false;
    }

    return true;
}

size_t HexEncode::EncodeWithDelimiter(char* buffer,
                                      size_t buflen,
                                      const char* csource,
                                      size_t srclen,
                                      char delimiter) {
    assert(buffer);
    if (buflen == 0)
        return 0;

    // Init and check bounds.
    const unsigned char* bsource = reinterpret_cast<const unsigned char*>(csource);
    size_t srcpos = 0, bufpos = 0;
    size_t needed = delimiter ? (srclen * 3) : (srclen * 2 + 1);

    if (buflen < needed)
        return 0;

    while (srcpos < srclen) {
        unsigned char ch = bsource[srcpos++];
        buffer[bufpos] = Encode((ch >> 4) & 0xF);
        buffer[bufpos + 1] = Encode((ch)&0xF);
        bufpos += 2;

        // Don't write a delimiter after the last byte.
        if (delimiter && (srcpos < srclen)) {
            buffer[bufpos] = delimiter;
            ++bufpos;
        }
    }

    // Null terminate.
    buffer[bufpos] = '\0';
    return bufpos;
}

std::string HexEncode::Encode(const std::string& str) {
    return Encode(str.c_str(), str.size());
}

std::string HexEncode::Encode(const char* source, size_t srclen) {
    return EncodeWithDelimiter(source, srclen, 0);
}

std::string HexEncode::EncodeWithDelimiter(const char* source, size_t srclen, char delimiter) {
    const size_t kBufferSize = srclen * 3;

    char* buffer = static_cast<char*>(::malloc((kBufferSize) * sizeof(char)));
    if (!buffer)
        return "";

    const size_t length = EncodeWithDelimiter(buffer, kBufferSize, source, srclen, delimiter);
    assert(srclen == 0 || length > 0);
    std::string ret(buffer, length);
    free(buffer);
    return ret;
}

size_t HexEncode::DecodeWithDelimiter(char* cbuffer,
                                      size_t buflen,
                                      const char* source,
                                      size_t srclen,
                                      char delimiter) {
    assert(cbuffer);
    if (buflen == 0)
        return 0;

    // Init and bounds check.
    unsigned char* bbuffer = reinterpret_cast<unsigned char*>(cbuffer);
    size_t srcpos = 0, bufpos = 0;
    size_t needed = (delimiter) ? (srclen + 1) / 3 : srclen / 2;

    if (buflen < needed)
        return 0;

    while (srcpos < srclen) {
        if ((srclen - srcpos) < 2) {
            // This means we have an odd number of bytes.
            return 0;
        }

        unsigned char h1, h2;

        if (!Decode(source[srcpos], &h1) || !Decode(source[srcpos + 1], &h2))
            return 0;

        bbuffer[bufpos++] = (h1 << 4) | h2;
        srcpos += 2;

        // Remove the delimiter if needed.
        if (delimiter && (srclen - srcpos) > 1) {
            if (source[srcpos] != delimiter)
                return 0;

            ++srcpos;
        }
    }

    return bufpos;
}

size_t HexEncode::Decode(char* buffer, size_t buflen, const std::string& source) {
    return DecodeWithDelimiter(buffer, buflen, source, 0);
}

std::string HexEncode::Decode(const std::string& str) {
    if (str.length() == 0)
        return "";
    const size_t kBufferSize = str.length();
    char* buffer = static_cast<char*>(::malloc((kBufferSize) * sizeof(char)));
    if (!buffer)
        return "";
    const size_t dstSize = Decode(buffer, kBufferSize, str);
    std::string ret(buffer, dstSize);
    free(buffer);
    return ret;
}

size_t HexEncode::DecodeWithDelimiter(char* buffer,
                                      size_t buflen,
                                      const std::string& source,
                                      char delimiter) {
    return DecodeWithDelimiter(buffer, buflen, source.c_str(), source.length(), delimiter);
}
}  // namespace jhc