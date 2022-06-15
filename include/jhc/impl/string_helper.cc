#include "jhc/config.hpp"

#ifdef JHC_NOT_HEADER_ONLY
#include "../string_helper.hpp"
#endif

#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cwctype>
#include <iterator>
#include <sstream>
#include <cassert>
#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif  // !_INC_WINDOWS
#include <strsafe.h>
#endif

#if defined(__GNUC__)
#define VA_COPY(a, b) (va_copy(a, b))
#else
#define VA_COPY(a, b) (a = b)
#endif

#ifndef COUNT_OF
#define COUNT_OF(array) (sizeof(array) / sizeof(array[0]))
#endif

namespace jhc {
namespace stringhelper_detail {
struct EqualA {
    EqualA() = delete;
    EqualA(bool caseInsensitive) :
        caseInsensitive_(caseInsensitive) {}

    bool operator()(char ch1, char ch2) {
        if (caseInsensitive_)
            return StringHelper::ToUpper(ch1) == StringHelper::ToUpper(ch2);
        return ch1 == ch2;
    }

   private:
    bool caseInsensitive_ = false;
};

struct EqualW {
    EqualW() = delete;
    EqualW(bool caseInsensitive) :
        caseInsensitive_(caseInsensitive) {}

    bool operator()(wchar_t ch1, wchar_t ch2) {
        if (caseInsensitive_)
            return StringHelper::ToUpper(ch1) == StringHelper::ToUpper(ch2);
        return ch1 == ch2;
    }

   private:
    bool caseInsensitive_ = false;
};
}  // namespace stringhelper_detail
JHC_INLINE char StringHelper::ToLower(const char& in) {
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}

JHC_INLINE char StringHelper::ToUpper(const char& in) {
    if (in <= 'z' && in >= 'a')
        return in + ('Z' - 'z');
    return in;
}

JHC_INLINE wchar_t StringHelper::ToLower(const wchar_t& in) {
    if (in <= 'Z' && in >= 'A')
        return in - (L'Z' - L'z');
    return in;
}

JHC_INLINE wchar_t StringHelper::ToUpper(const wchar_t& in) {
    if (in <= L'z' && in >= L'a')
        return in + (L'Z' - L'z');
    return in;
}

JHC_INLINE std::string StringHelper::ToLower(const std::string& s) {
    std::string d = s;
    char (*pf)(const char&) = StringHelper::ToLower;
    std::transform(d.begin(), d.end(), d.begin(), pf);
    return d;
}

JHC_INLINE std::wstring StringHelper::ToLower(const std::wstring& s) {
    std::wstring d = s;
    wchar_t (*pf)(const wchar_t&) = StringHelper::ToLower;
    std::transform(d.begin(), d.end(), d.begin(), pf);
    return d;
}

JHC_INLINE std::string StringHelper::ToUpper(const std::string& s) {
    std::string d = s;
    char (*pf)(const char&) = StringHelper::ToUpper;
    std::transform(d.begin(), d.end(), d.begin(), pf);
    return d;
}

JHC_INLINE std::wstring StringHelper::ToUpper(const std::wstring& s) {
    std::wstring d = s;
    wchar_t (*pf)(const wchar_t&) = StringHelper::ToUpper;
    std::transform(d.begin(), d.end(), d.begin(), pf);
    return d;
}

JHC_INLINE bool StringHelper::IsDigit(const std::string& s) {
    return !s.empty() &&
           std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

JHC_INLINE bool StringHelper::IsDigit(const std::wstring& s) {
    return !s.empty() &&
           std::find_if(s.begin(), s.end(), [](wchar_t c) { return !std::iswdigit(c); }) == s.end();
}

JHC_INLINE bool StringHelper::IsLetterOrDigit(const char& c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

JHC_INLINE bool StringHelper::IsLetterOrDigit(const wchar_t& c) {
    return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z');
}

JHC_INLINE bool StringHelper::IsLetterOrDigit(const std::string& s) {
    return !s.empty() &&
           std::find_if(s.begin(), s.end(), [](char c) { return !IsLetterOrDigit(c); }) == s.end();
}

JHC_INLINE bool StringHelper::IsLetterOrDigit(const std::wstring& s) {
    return !s.empty() &&
           std::find_if(s.begin(), s.end(), [](wchar_t c) { return !IsLetterOrDigit(c); }) == s.end();
}

JHC_INLINE std::string StringHelper::Trim(const std::string& s, const std::string& whitespaces) {
    const std::string::size_type pos = s.find_first_not_of(whitespaces);
    if (pos == std::string::npos) {
        return std::string();
    }

    std::string::size_type n = s.find_last_not_of(whitespaces) - pos + 1;
    return s.substr(pos, n);
}

JHC_INLINE std::wstring StringHelper::Trim(const std::wstring& s, const std::wstring& whitespaces) {
    const std::wstring::size_type pos = s.find_first_not_of(whitespaces);
    if (pos == std::wstring::npos) {
        return std::wstring();
    }

    std::wstring::size_type n = s.find_last_not_of(whitespaces) - pos + 1;
    return s.substr(pos, n);
}

JHC_INLINE std::string StringHelper::LeftTrim(const std::string& s, const std::string& whitespaces) {
    const std::string::size_type pos = s.find_first_not_of(whitespaces);
    if (pos == std::string::npos) {
        return std::string();
    }

    return s.substr(pos);
}

JHC_INLINE std::wstring StringHelper::LeftTrim(const std::wstring& s, const std::wstring& whitespaces) {
    const std::wstring::size_type pos = s.find_first_not_of(whitespaces);
    if (pos == std::wstring::npos) {
        return std::wstring();
    }

    return s.substr(pos);
}

JHC_INLINE std::string StringHelper::RightTrim(const std::string& s, const std::string& whitespaces) {
    const std::string::size_type pos = s.find_last_not_of(whitespaces);
    if (pos == 0) {
        return std::string();
    }

    return s.substr(0, pos + 1);
}

JHC_INLINE std::wstring StringHelper::RightTrim(const std::wstring& s, const std::wstring& whitespaces) {
    const std::wstring::size_type pos = s.find_last_not_of(whitespaces);
    if (pos == 0) {
        return std::wstring();
    }

    return s.substr(0, pos + 1);
}

JHC_INLINE bool StringHelper::IsStartsWith(const std::string& s, const std::string& prefix) {
    return s.compare(0, prefix.length(), prefix) == 0;
}

JHC_INLINE bool StringHelper::IsStartsWith(const std::wstring& s, const std::wstring& prefix) {
    return s.compare(0, prefix.length(), prefix) == 0;
}

JHC_INLINE bool StringHelper::IsEndsWith(const std::string& s, const std::string& suffix) {
    if (suffix.length() <= s.length()) {
        return s.compare(s.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
    return false;
}

JHC_INLINE bool StringHelper::IsEndsWith(const std::wstring& s, const std::wstring& suffix) {
    if (suffix.length() <= s.length()) {
        return s.compare(s.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
    return false;
}

JHC_INLINE bool StringHelper::IsContains(const std::string& str, const std::string& substring) {
    return (str.find(substring) != std::string::npos);
}

JHC_INLINE bool StringHelper::IsContains(const std::wstring& str, const std::wstring& substring) {
    return (str.find(substring) != std::wstring::npos);
}

JHC_INLINE size_t StringHelper::ContainTimes(const std::string& str, const std::string& substring) {
    size_t times = 0;
    size_t pos = std::string::npos;
    size_t offset = 0;

    if (substring.length() == 0)
        return 0;

    do {
        pos = str.find(substring, offset);
        if (pos == std::string::npos)
            break;

        offset = pos + substring.length();
        times++;
    } while (true);

    return times;
}

JHC_INLINE size_t StringHelper::ContainTimes(const std::wstring& str, const std::wstring& substring) {
    size_t times = 0;
    size_t pos = std::wstring::npos;
    size_t offset = 0;

    if (substring.length() == 0)
        return 0;

    do {
        pos = str.find(substring, offset);
        if (pos == std::wstring::npos)
            break;

        offset = pos + substring.length();
        times++;
    } while (true);

    return times;
}

JHC_INLINE std::string::size_type StringHelper::Find(const std::string& str, const std::string& substring, std::string::size_type offset, bool caseInsensitive) {
    if (offset >= str.length())
        return std::wstring::npos;

    std::string::const_iterator src_it = str.begin() + offset;
    std::string::const_iterator it = std::search(src_it,
                                                 str.end(),
                                                 substring.begin(),
                                                 substring.end(),
                                                 stringhelper_detail::EqualA(caseInsensitive));
    if (it != str.end())
        return it - str.begin();

    return std::string::npos;  // Not found
}

std::wstring::size_type StringHelper::Find(const std::wstring& str, const std::wstring& substring, std::wstring::size_type offset, bool caseInsensitive) {
    if (offset >= str.length())
        return std::wstring::npos;

    std::wstring::const_iterator src_it = str.begin() + offset;
    std::wstring::const_iterator it = std::search(src_it,
                                                  str.end(),
                                                  substring.begin(),
                                                  substring.end(),
                                                  stringhelper_detail::EqualW(caseInsensitive));
    if (it != str.end())
        return it - str.begin();

    return std::wstring::npos;  // Not found
}

JHC_INLINE std::string StringHelper::ReplaceFirst(const std::string& s, const std::string& from, const std::string& to) {
    const size_t start_pos = s.find(from);
    if (start_pos == std::string::npos) {
        return s;
    }

    std::string ret = s;
    ret.replace(start_pos, from.length(), to);
    return ret;
}

JHC_INLINE std::wstring StringHelper::ReplaceFirst(const std::wstring& s, const std::wstring& from, const std::wstring& to) {
    const size_t start_pos = s.find(from);
    if (start_pos == std::wstring::npos) {
        return s;
    }

    std::wstring ret = s;
    ret.replace(start_pos, from.length(), to);
    return ret;
}

JHC_INLINE std::string StringHelper::ReplaceLast(const std::string& s, const std::string& from, const std::string& to) {
    const size_t start_pos = s.rfind(from);
    if (start_pos == std::string::npos) {
        return s;
    }

    std::string ret = s;
    ret.replace(start_pos, from.length(), to);
    return ret;
}

JHC_INLINE std::wstring StringHelper::ReplaceLast(const std::wstring& s, const std::wstring& from, const std::wstring& to) {
    const size_t start_pos = s.rfind(from);
    if (start_pos == std::wstring::npos) {
        return s;
    }

    std::wstring ret = s;
    ret.replace(start_pos, from.length(), to);
    return ret;
}

JHC_INLINE std::string StringHelper::Replace(const std::string& s, const std::string& from, const std::string& to, std::wstring::size_type offset, bool caseInsensitive) {
    if (from.empty()) {
        return s;
    }

    const bool found_substring = StringHelper::Find(s, from, offset, caseInsensitive) != std::string::npos;
    if (!found_substring) {
        return s;
    }

    size_t start_pos = 0;
    std::string ret = s;
    while ((start_pos = StringHelper::Find(ret, from, start_pos, caseInsensitive)) != std::string::npos) {
        ret.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return ret;
}

JHC_INLINE std::wstring StringHelper::Replace(const std::wstring& s, const std::wstring& from, const std::wstring& to, std::wstring::size_type offset, bool caseInsensitive) {
    if (from.empty()) {
        return s;
    }

    const bool found_substring = StringHelper::Find(s, from, offset, caseInsensitive) != std::wstring::npos;
    if (!found_substring) {
        return s;
    }

    size_t start_pos = 0;
    std::wstring ret = s;
    while ((start_pos = StringHelper::Find(ret, from, start_pos, caseInsensitive)) != std::wstring::npos) {
        ret.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return ret;
}

JHC_INLINE std::vector<std::string> StringHelper::Split(const std::string& src, const std::string& delimiter, bool includeEmptyStr) {
    std::vector<std::string> fields;
    typename std::string::size_type offset = 0;
    typename std::string::size_type pos = src.find(delimiter, 0);

    while (pos != std::string::npos) {
        std::string t = src.substr(offset, pos - offset);
        if ((t.length() > 0) || (t.length() == 0 && includeEmptyStr))
            fields.push_back(t);
        offset = pos + delimiter.length();
        pos = src.find(delimiter, offset);
    }

    const std::string t = src.substr(offset);
    if ((t.length() > 0) || (t.length() == 0 && includeEmptyStr))
        fields.push_back(t);
    return fields;
}

JHC_INLINE std::vector<std::wstring> StringHelper::Split(const std::wstring& src, const std::wstring& delimiter, bool includeEmptyStr) {
    std::vector<std::wstring> fields;
    typename std::wstring::size_type offset = 0;
    typename std::wstring::size_type pos = src.find(delimiter, 0);

    while (pos != std::wstring::npos) {
        std::wstring t = src.substr(offset, pos - offset);
        if ((t.length() > 0) || (t.length() == 0 && includeEmptyStr))
            fields.push_back(t);
        offset = pos + delimiter.length();
        pos = src.find(delimiter, offset);
    }

    const std::wstring t = src.substr(offset);
    if ((t.length() > 0) || (t.length() == 0 && includeEmptyStr))
        fields.push_back(t);
    return fields;
}

JHC_INLINE std::string StringHelper::Join(const std::vector<std::string>& src, const std::string& delimiter, bool includeEmptyStr) {
    std::stringstream ss;
    for (std::vector<std::string>::const_iterator it = src.cbegin(); it != src.cend(); ++it) {
        if (it->length() > 0) {
            ss << *it;
        }
        else {
            if (includeEmptyStr) {
                ss << *it;
            }
        }

        if (it + 1 != src.cend()) {
            ss << delimiter;
        }
    }
    return ss.str();
}

JHC_INLINE std::wstring StringHelper::Join(const std::vector<std::wstring>& src, const std::wstring& delimiter, bool includeEmptyStr) {
    std::wstringstream ss;
    for (std::vector<std::wstring>::const_iterator it = src.cbegin(); it != src.cend(); ++it) {
        if (it->length() > 0) {
            ss << *it;
        }
        else {
            if (includeEmptyStr) {
                ss << *it;
            }
        }

        if (it + 1 != src.cend()) {
            ss << delimiter;
        }
    }
    return ss.str();
}

JHC_INLINE bool StringHelper::IsEqual(const std::string& s1, const std::string& s2, bool ignoreCase) {
    const std::string::size_type s1_len = s1.length();
    if (s1_len != s2.length())
        return false;

    for (std::string::size_type i = 0; i < s1_len; i++) {
        if (ignoreCase) {
            if (ToLower(s1[i]) != ToLower(s2[i]))
                return false;
        }
        else {
            if (s1[i] != s2[i])
                return false;
        }
    }

    return true;
}

JHC_INLINE bool StringHelper::IsEqual(const std::wstring& s1, const std::wstring& s2, bool ignoreCase) {
    const std::wstring::size_type s1_len = s1.length();
    if (s1_len != s2.length())
        return false;

    for (std::wstring::size_type i = 0; i < s1_len; i++) {
        if (ignoreCase) {
            if (ToLower(s1[i]) != ToLower(s2[i]))
                return false;
        }
        else {
            if (s1[i] != s2[i])
                return false;
        }
    }

    return true;
}

// format a string
JHC_INLINE bool StringHelper::StringPrintfV(const char* format, va_list argList, std::string& output) {
    if (!format)
        return false;

#ifdef JHC_WIN
    char* pMsgBuffer = NULL;
    size_t iMsgBufCount = 0;

    HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
        iMsgBufCount += 1024;
        if (pMsgBuffer) {
            free(pMsgBuffer);
            pMsgBuffer = NULL;
        }

        pMsgBuffer = (char*)malloc(iMsgBufCount * sizeof(char));
        if (!pMsgBuffer) {
            break;
        }
        hr = StringCchVPrintfA(pMsgBuffer, iMsgBufCount, format, argList);
    }

    if (hr == S_OK && pMsgBuffer) {
        output.assign(pMsgBuffer);
    }

    if (pMsgBuffer) {
        free(pMsgBuffer);
        pMsgBuffer = NULL;
    }

    return (hr == S_OK);
#else
    bool result = false;
    char* msgBuf = nullptr;
    size_t msgBufSize = 1024;

    do {
        if (msgBuf) {
            free(msgBuf);
            msgBuf = nullptr;
        }
        msgBuf = (char*)malloc(msgBufSize * sizeof(char));
        if (!msgBuf) {
            break;
        }
        memset(msgBuf, 0, msgBufSize * sizeof(char));

        va_list va_copy;
        VA_COPY(va_copy, argList);
        const int err = vsnprintf(msgBuf, msgBufSize, format, va_copy);
        if (err >= 0 && err < msgBufSize) {
            result = true;
            break;
        }

        msgBufSize *= 2;
    } while (true);

    if (result && msgBuf) {
        output.assign(msgBuf);
    }

    if (msgBuf) {
        free(msgBuf);
        msgBuf = nullptr;
    }

    return result;
#endif
}

JHC_INLINE bool StringHelper::StringPrintfV(const wchar_t* format, va_list argList, std::wstring& output) {
    if (!format)
        return false;

#ifdef JHC_WIN
    wchar_t* pMsgBuffer = NULL;
    size_t iMsgBufCount = 0;

    HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
        iMsgBufCount += 1024;
        if (pMsgBuffer) {
            free(pMsgBuffer);
            pMsgBuffer = NULL;
        }

        pMsgBuffer = (wchar_t*)malloc(iMsgBufCount * sizeof(wchar_t));
        if (!pMsgBuffer) {
            break;
        }
        hr = StringCchVPrintfW(pMsgBuffer, iMsgBufCount, format, argList);
    }

    if (hr == S_OK && pMsgBuffer) {
        output.assign(pMsgBuffer);
    }

    if (pMsgBuffer) {
        free(pMsgBuffer);
        pMsgBuffer = NULL;
    }

    return (hr == S_OK);
#else
    bool result = false;
    wchar_t* msgBuf = nullptr;
    size_t msgBufSize = 1024;

    do {
        if (msgBuf) {
            free(msgBuf);
            msgBuf = nullptr;
        }
        msgBuf = (wchar_t*)malloc(msgBufSize * sizeof(wchar_t));
        if (!msgBuf) {
            break;
        }
        memset(msgBuf, 0, msgBufSize * sizeof(wchar_t));

        va_list va_copy;
        VA_COPY(va_copy, argList);
        const int err = vswprintf(msgBuf, msgBufSize, format, va_copy);
        if (err >= 0 && err < msgBufSize) {
            result = true;
            break;
        }

        msgBufSize *= 2;
    } while (true);

    if (result && msgBuf) {
        output.assign(msgBuf);
    }

    if (msgBuf) {
        free(msgBuf);
        msgBuf = nullptr;
    }

    return result;
#endif
}

JHC_INLINE std::string StringHelper::StringPrintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    std::string output;
    StringPrintfV(format, args, output);
    va_end(args);

    return output;
}

JHC_INLINE std::wstring StringHelper::StringPrintf(const wchar_t* format, ...) {
    va_list args;
    va_start(args, format);
    std::wstring output;
    StringPrintfV(format, args, output);
    va_end(args);

    return output;
}

JHC_INLINE std::string StringHelper::StringPrintfV(const char* format, va_list argList) {
    std::string output;
    StringPrintfV(format, argList, output);
    return output;
}

JHC_INLINE std::wstring StringHelper::StringPrintfV(const wchar_t* format, va_list argList) {
    std::wstring output;
    StringPrintfV(format, argList, output);
    return output;
}
}  // namespace jhc