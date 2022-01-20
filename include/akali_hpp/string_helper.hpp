﻿/*******************************************************************************
 * Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
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
#pragma once

#include "akali_hpp/arch.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>

namespace akali_hpp {
inline char EasyCharToLowerA(char in) {
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}

inline char EasyCharToUpperA(char in) {
    if (in <= 'z' && in >= 'a')
        return in + ('Z' - 'z');
    return in;
}

inline wchar_t EasyCharToLowerW(wchar_t in) {
    if (in <= 'Z' && in >= 'A')
        return in - (L'Z' - L'z');
    return in;
}

inline wchar_t EasyCharToUpperW(wchar_t in) {
    if (in <= L'z' && in >= L'a')
        return in + (L'Z' - L'z');
    return in;
}

inline bool StringCaseInSensCompare(const std::string& str1, const std::string& str2) {
    return ((str1.size() == str2.size()) && std::equal(str1.begin(),
                                                       str1.end(),
                                                       str2.begin(),
                                                       [](const char& c1, const char& c2) {
                                                           return (c1 == c2 || std::toupper(c1) == std::toupper(c2));
                                                       }));
}

inline bool StringCaseInSensCompare(const std::wstring& str1, const std::wstring& str2) {
    return ((str1.size() == str2.size()) && std::equal(str1.begin(),
                                                       str1.end(),
                                                       str2.begin(),
                                                       [](const wchar_t& c1, const wchar_t& c2) {
                                                           return (c1 == c2 || std::towupper(c1) == std::towupper(c2));
                                                       }));
}

template <typename T, typename Func>
typename std::enable_if<std::is_same<char, T>::value || std::is_same<wchar_t, T>::value,
                        std::basic_string<T, std::char_traits<T>, std::allocator<T>>>::type
StringCaseConvert(const std::basic_string<T, std::char_traits<T>, std::allocator<T>>& str,
                  Func func) {
    std::basic_string<T, std::char_traits<T>, std::allocator<T>> ret = str;
    std::transform(ret.begin(), ret.end(), ret.begin(), func);
    return ret;
}

template <typename T>
typename std::enable_if<
    std::is_same<std::string, T>::value || std::is_same<std::wstring, T>::value ||
        std::is_same<std::u16string, T>::value || std::is_same<std::u32string, T>::value,
    std::vector<T>>::type
StringSplit(const T& src, const T& delimiter, bool include_empty_string = true) {
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
}  // namespace akali_hpp
#endif  // !AKALI_STRING_HELPER_HPP__
