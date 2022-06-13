/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022 JiangXueqiao <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef JHC_STRING_HELPER_HPP__
#define JHC_STRING_HELPER_HPP__
#include "jhc/config.hpp"
#include "jhc/arch.hpp"
#include <string>
#include <vector>

namespace jhc {
class StringHelper {
   public:
    static char ToLower(const char& in);

    static char ToUpper(const char& in);

    static wchar_t ToLower(const wchar_t& in);

    static wchar_t ToUpper(const wchar_t& in);

    static std::string ToLower(const std::string& s);

    static std::wstring ToLower(const std::wstring& s);

    static std::string ToUpper(const std::string& s);

    static std::wstring ToUpper(const std::wstring& s);

    static bool IsDigit(const std::string& s);

    static bool IsDigit(const std::wstring& s);

    static bool IsLetterOrDigit(const char& c);

    static bool IsLetterOrDigit(const wchar_t& c);

    static bool IsLetterOrDigit(const std::string& s);

    static bool IsLetterOrDigit(const std::wstring& s);

    static std::string Trim(const std::string& s, const std::string& whitespaces = " \t\f\v\n\r");

    static std::wstring Trim(const std::wstring& s, const std::wstring& whitespaces = L" \t\f\v\n\r");

    static std::string LeftTrim(const std::string& s, const std::string& whitespaces = " \t\f\v\n\r");

    static std::wstring LeftTrim(const std::wstring& s, const std::wstring& whitespaces = L" \t\f\v\n\r");

    static std::string RightTrim(const std::string& s, const std::string& whitespaces = " \t\f\v\n\r");

    static std::wstring RightTrim(const std::wstring& s, const std::wstring& whitespaces = L" \t\f\v\n\r");

    static bool IsStartsWith(const std::string& s, const std::string& prefix);

    static bool IsStartsWith(const std::wstring& s, const std::wstring& prefix);

    static bool IsEndsWith(const std::string& s, const std::string& suffix);

    static bool IsEndsWith(const std::wstring& s, const std::wstring& suffix);

    static bool IsContains(const std::string& str, const std::string& substring);

    static bool IsContains(const std::wstring& str, const std::wstring& substring);

    static size_t ContainTimes(const std::string& str, const std::string& substring);

    static size_t ContainTimes(const std::wstring& str, const std::wstring& substring);

    static std::string ReplaceFirst(const std::string& s, const std::string& from, const std::string& to);

    static std::wstring ReplaceFirst(const std::wstring& s, const std::wstring& from, const std::wstring& to);

    static std::string ReplaceLast(const std::string& s, const std::string& from, const std::string& to);

    static std::wstring ReplaceLast(const std::wstring& s, const std::wstring& from, const std::wstring& to);

    static std::string Replace(const std::string& s, const std::string& from, const std::string& to);

    static std::wstring Replace(const std::wstring& s, const std::wstring& from, const std::wstring& to);

    static std::vector<std::string> Split(const std::string& src, const std::string& delimiter, bool includeEmptyStr = true);

    static std::vector<std::wstring> Split(const std::wstring& src, const std::wstring& delimiter, bool includeEmptyStr = true);

    static std::string Join(const std::vector<std::string>& src, const std::string& delimiter, bool includeEmptyStr = true);

    static std::wstring Join(const std::vector<std::wstring>& src, const std::wstring& delimiter, bool includeEmptyStr = true);

    static bool IsEqual(const std::string& s1, const std::string& s2, bool ignoreCase = false);

    static bool IsEqual(const std::wstring& s1, const std::wstring& s2, bool ignoreCase = false);

    // format a string
    static bool StringPrintfV(const char* format, va_list argList, std::string& output);

    static bool StringPrintfV(const wchar_t* format, va_list argList, std::wstring& output);

    static std::string StringPrintf(const char* format, ...);

    static std::wstring StringPrintf(const wchar_t* format, ...);

    static std::string StringPrintfV(const char* format, va_list argList);

    static std::wstring StringPrintfV(const wchar_t* format, va_list argList);
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/string_helper.cc"
#endif
#endif  // !JHC_STRING_HELPER_HPP__
