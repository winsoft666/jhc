/*******************************************************************************
 * Copyright (C) 2021 - 2026, winsoft666, <winsoft666@outlook.com>.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Expect bugs
 *
 * Please use and enjoy. Please let me know of any bugs/improvements
 * that you have found/implemented and I will fix/incorporate them into this
 * file.
 *******************************************************************************/

#ifndef AKALI_STRING_HELPER_HPP__
#define AKALI_STRING_HELPER_HPP__

#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <iterator>

namespace akali_hpp {
class StringHelper {
   public:
    static std::string ToLowerCase(const std::string& s) {
        std::string d;
        std::transform(s.begin(), s.end(),
                       std::insert_iterator<std::string>(d, d.begin()), tolower);
        return d;
    }

    static std::wstring ToLowerCase(const std::wstring& s) {
        std::wstring d;
#if defined(_MSC_VER)
        std::transform(s.begin(), s.end(),
                       std::insert_iterator<std::wstring>(d, d.begin()),
                       towlower);
#else
        std::transform(s.begin(), s.end(),
                       std::insert_iterator<std::wstring>(d, d.begin()),
                       (int (*)(int))std::tolower);
#endif
        return d;
    }

    static std::string Trim(const std::string& s, char c = ' ') {
        const std::string::size_type pos = s.find_first_not_of(c);
        if (pos == std::string::npos) {
            return std::string();
        }

        std::string::size_type n = s.find_last_not_of(c) - pos + 1;
        return s.substr(pos, n);
    }

    static std::wstring Trim(const std::wstring& s, wchar_t c = L' ') {
        const std::wstring::size_type pos = s.find_first_not_of(c);
        if (pos == std::wstring::npos) {
            return std::wstring();
        }

        std::wstring::size_type n = s.find_last_not_of(c) - pos + 1;
        return s.substr(pos, n);
    }

    static std::string LeftTrim(const std::string& s, char c = ' ') {
        const std::string::size_type pos = s.find_first_not_of(c);
        if (pos == std::string::npos) {
            return std::string();
        }

        return s.substr(pos);
    }

    static std::wstring LeftTrim(const std::wstring& s, wchar_t c = L' ') {
        const std::wstring::size_type pos = s.find_first_not_of(c);
        if (pos == std::wstring::npos) {
            return std::wstring();
        }

        return s.substr(pos);
    }

    static std::string RightTrim(const std::string& s, char c = ' ') {
        const std::string::size_type pos = s.find_last_not_of(c);
        if (pos == 0) {
            return std::string();
        }

        return s.substr(0, pos + 1);
    }

    static std::wstring RightTrim(const std::wstring& s, wchar_t c = L' ') {
        const std::wstring::size_type pos = s.find_last_not_of(c);
        if (pos == 0) {
            return std::wstring();
        }

        return s.substr(0, pos + 1);
    }

    static bool IsStartsWith(const std::string& s, const std::string& prefix) {
        return s.compare(0, prefix.length(), prefix) == 0;
    }

    static bool IsStartsWith(const std::wstring& s, const std::wstring& prefix) {
        return s.compare(0, prefix.length(), prefix) == 0;
    }

    static bool IsEndsWith(const std::string& s, const std::string& suffix) {
        if (suffix.length() <= s.length()) {
            return s.compare(s.length() - suffix.length(), suffix.length(), suffix) == 0;
        }
        return false;
    }

    static bool IsEndsWith(const std::wstring& s, const std::wstring& suffix) {
        if (suffix.length() <= s.length()) {
            return s.compare(s.length() - suffix.length(), suffix.length(), suffix) == 0;
        }
        return false;
    }

    static bool IsEqualsIgnoreCase(const std::string& s1, const char* upper, const char* lower) {
        for (std::string::const_iterator iter = s1.begin();
             iter != s1.end();
             iter++, upper++, lower++) {
            if (*iter != *upper && *iter != *lower)
                return false;
        }
        return (*upper == 0);
    }

    static bool IsEqualsIgnoreCase(const std::wstring& s1, const wchar_t* upper, const wchar_t* lower) {
        for (std::wstring::const_iterator iter = s1.begin();
             iter != s1.end();
             iter++, upper++, lower++) {
            if (*iter != *upper && *iter != *lower)
                return false;
        }
        return (*upper == 0);
    }

    static bool IsContains(const std::string& str, const std::string& substring) {
        return (str.find(substring) != std::string::npos);
    }

    static bool IsContains(const std::wstring& str, const std::wstring& substring) {
        return (str.find(substring) != std::wstring::npos);
    }

    static size_t IncludeTimes(const std::string& str, const std::string& substring) {
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

    static size_t IncludeTimes(const std::wstring& str, const std::wstring& substring) {
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

    static std::string ReplaceFirst(const std::string& s, const std::string& from, const std::string& to) {
        const size_t start_pos = s.find(from);
        if (start_pos == std::string::npos) {
            return s;
        }

        std::string ret = s;
        ret.replace(start_pos, from.length(), to);
        return ret;
    }

    static std::wstring ReplaceFirst(const std::wstring& s, const std::wstring& from, const std::wstring& to) {
        const size_t start_pos = s.find(from);
        if (start_pos == std::wstring::npos) {
            return s;
        }

        std::wstring ret = s;
        ret.replace(start_pos, from.length(), to);
        return ret;
    }

    static std::string ReplaceLast(const std::string& s, const std::string& from, const std::string& to) {
        const size_t start_pos = s.rfind(from);
        if (start_pos == std::string::npos) {
            return s;
        }

        std::string ret = s;
        ret.replace(start_pos, from.length(), to);
        return ret;
    }

    static std::wstring ReplaceLast(const std::wstring& s, const std::wstring& from, const std::wstring& to) {
        const size_t start_pos = s.rfind(from);
        if (start_pos == std::wstring::npos) {
            return s;
        }

        std::wstring ret = s;
        ret.replace(start_pos, from.length(), to);
        return ret;
    }

    static std::string Replace(const std::string& s, const std::string& from, const std::string& to) {
        if (from.empty()) {
            return s;
        }

        size_t start_pos = 0;
        const bool found_substring = s.find(from, start_pos) != std::string::npos;
        if (!found_substring) {
            return s;
        }

        std::string ret = s;
        while ((start_pos = ret.find(from, start_pos)) != std::string::npos) {
            ret.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }

        return ret;
    }

    static std::wstring Replace(const std::wstring& s, const std::wstring& from, const std::wstring& to) {
        if (from.empty()) {
            return s;
        }

        size_t start_pos = 0;
        const bool found_substring = s.find(from, start_pos) != std::string::npos;
        if (!found_substring) {
            return s;
        }

        std::wstring ret = s;
        while ((start_pos = ret.find(from, start_pos)) != std::string::npos) {
            ret.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }

        return ret;
    }

    template <typename T>
    static typename std::enable_if<
        std::is_same<std::string, T>::value || std::is_same<std::wstring, T>::value ||
            std::is_same<std::u16string, T>::value || std::is_same<std::u32string, T>::value,
        std::vector<T> >::type
    Split(const T& src, const T& delimiter, bool include_empty_string = true) {
        std::vector<T> fields;
        typename T::size_type offset = 0;
        typename T::size_type pos = src.find(delimiter, 0);

        while (pos != T::npos) {
            T t = src.substr(offset, pos - offset);
            if ((t.length() > 0) || (t.length() == 0 && include_empty_string))
                fields.push_back(t);
            offset = pos + delimiter.length();
            pos = src.find(delimiter, offset);
        }

        T t = src.substr(offset);
        if ((t.length() > 0) || (t.length() == 0 && include_empty_string))
            fields.push_back(t);
        return fields;
    }
};
}  // namespace akali_hpp
#endif  // !AKALI_STRING_HELPER_HPP__
