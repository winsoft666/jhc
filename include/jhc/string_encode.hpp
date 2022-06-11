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

#ifndef JHC_STRING_ENCODE_HPP_
#define JHC_STRING_ENCODE_HPP_

#include <string>
#include "jhc/arch.hpp"

namespace jhc {
class StringEncode {
   public:
#ifdef JHC_WIN
    static std::string UnicodeToAnsi(const std::wstring& str, unsigned int code_page = 0);

    static std::wstring AnsiToUnicode(const std::string& str, unsigned int code_page = 0);

#endif

    static std::string UnicodeToUtf8(const std::wstring& str);

    static std::string UnicodeToUtf8BOM(const std::wstring& str);

    static std::wstring Utf8ToUnicode(const std::string& str);

#ifdef JHC_WIN
    static std::string AnsiToUtf8(const std::string& str, unsigned int code_page = 0);

    static std::string AnsiToUtf8BOM(const std::string& str, unsigned int code_page = 0);

    static std::string Utf8ToAnsi(const std::string& str, unsigned int code_page = 0);
#endif
};

#ifdef JHC_WIN
#if (defined UNICODE || defined _UNICODE)
#define TCHARToAnsi(str) jhc::StringEncode::UnicodeToAnsi((str), 0)
#define TCHARToUtf8(str) jhc::StringEncode::UnicodeToUtf8((str))
#define AnsiToTCHAR(str) jhc::StringEncode::AnsiToUnicode((str), 0)
#define Utf8ToTCHAR(str) jhc::StringEncode::Utf8ToUnicode((str))
#define TCHARToUnicode(str) ((std::wstring)(str))
#define UnicodeToTCHAR(str) ((std::wstring)(str))
#else
#define TCHARToAnsi(str) ((std::string)(str))
#define TCHARToUtf8 jhc::StringEncode::AnsiToUtf8((str), 0)
#define AnsiToTCHAR(str) ((std::string)(str))
#define Utf8ToTCHAR(str) jhc::StringEncode::Utf8ToAnsi((str), 0)
#define TCHARToUnicode(str) jhc::StringEncode::AnsiToUnicode((str), 0)
#define UnicodeToTCHAR(str) jhc::StringEncode::UnicodeToAnsi((str), 0)
#endif
#endif
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/string_encode.cc"
#endif
#endif  // JHC_STRING_ENCODE_HPP_
