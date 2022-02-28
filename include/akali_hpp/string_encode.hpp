/*******************************************************************************
*    Copyright (C) <2022>  <winsoft666@outlook.com>.
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

#ifndef AKALI_STRING_ENCODE_HPP_
#define AKALI_STRING_ENCODE_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <string>
#include <vector>
#include "akali_hpp/arch.hpp"
#ifdef AKALI_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif
#endif

#pragma warning(disable : 4309)

namespace akali_hpp {

class StringEncode {
   public:
#ifdef AKALI_WIN
    static std::string UnicodeToAnsi(const std::wstring& str, unsigned int code_page = 0) {
        std::string strRes;
        int iSize = ::WideCharToMultiByte(code_page, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

        if (iSize == 0)
            return strRes;

        char* szBuf = new (std::nothrow) char[iSize];

        if (!szBuf)
            return strRes;

        memset(szBuf, 0, iSize);

        ::WideCharToMultiByte(code_page, 0, str.c_str(), -1, szBuf, iSize, NULL, NULL);

        strRes = szBuf;
        delete[] szBuf;

        return strRes;
    }

    static std::wstring AnsiToUnicode(const std::string& str, unsigned int code_page = 0) {
        std::wstring strRes;

        int iSize = ::MultiByteToWideChar(code_page, 0, str.c_str(), -1, NULL, 0);

        if (iSize == 0)
            return strRes;

        wchar_t* szBuf = new (std::nothrow) wchar_t[iSize];

        if (!szBuf)
            return strRes;

        memset(szBuf, 0, iSize * sizeof(wchar_t));

        ::MultiByteToWideChar(code_page, 0, str.c_str(), -1, szBuf, iSize);

        strRes = szBuf;
        delete[] szBuf;

        return strRes;
    }

    static std::string UnicodeToUtf8(const std::wstring& str) {
        std::string strRes;

        int iSize = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

        if (iSize == 0)
            return strRes;

        char* szBuf = new (std::nothrow) char[iSize];

        if (!szBuf)
            return strRes;

        memset(szBuf, 0, iSize);

        ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, szBuf, iSize, NULL, NULL);

        strRes = szBuf;
        delete[] szBuf;

        return strRes;
    }

    static std::string UnicodeToUtf8BOM(const std::wstring& str) {
        std::string strRes;

        int iSize = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

        if (iSize == 0)
            return strRes;

        char* szBuf = new (std::nothrow) char[iSize + 3];

        if (!szBuf)
            return strRes;

        memset(szBuf, 0, iSize + 3);

        ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &szBuf[3], iSize, NULL, NULL);
        szBuf[0] = 0xef;
        szBuf[1] = 0xbb;
        szBuf[2] = 0xbf;

        strRes = szBuf;
        delete[] szBuf;

        return strRes;
    }

    static std::wstring Utf8ToUnicode(const std::string& str) {
        std::wstring strRes;
        int iSize = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

        if (iSize == 0)
            return strRes;

        wchar_t* szBuf = new (std::nothrow) wchar_t[iSize];

        if (!szBuf)
            return strRes;

        memset(szBuf, 0, iSize * sizeof(wchar_t));
        ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, szBuf, iSize);

        strRes = szBuf;
        delete[] szBuf;

        return strRes;
    }

    static std::string AnsiToUtf8(const std::string& str, unsigned int code_page = 0) {
        return UnicodeToUtf8(AnsiToUnicode(str, code_page));
    }

    static std::string AnsiToUtf8BOM(const std::string& str, unsigned int code_page = 0) {
        return UnicodeToUtf8BOM(AnsiToUnicode(str, code_page));
    }

    static std::string Utf8ToAnsi(const std::string& str, unsigned int code_page = 0) {
        return UnicodeToAnsi(Utf8ToUnicode(str), code_page);
    }

#endif

   protected:
    // Apply any suitable string transform (including the ones above) to an STL
    // string. Stack-allocated temporary space is used for the transformation, so
    // value and source may refer to the same string.
    typedef size_t (*Transform)(char* buffer, size_t buflen, const char* source, size_t srclen);

    // Return the result of applying transform t to source.
    static std::string s_transform(const std::string& source, Transform t) {
        // Ask transformation function to approximate the destination size (returns upper bound)
        const size_t maxlen = t(nullptr, 0, source.data(), source.length());
        char* buffer = static_cast<char*>(::malloc((maxlen) * sizeof(char)));
        if (!buffer)
            return "";
        const size_t len = t(buffer, maxlen, source.data(), source.length());
        std::string result(buffer, len);
        free(buffer);
        return result;
    }
};

#if (defined UNICODE || defined _UNICODE)
#define TCHARToAnsi(str) akali_hpp::StringEncode::UnicodeToAnsi((str), 0)
#define TCHARToUtf8(str) akali_hpp::StringEncode::UnicodeToUtf8((str))
#define AnsiToTCHAR(str) akali_hpp::StringEncode::AnsiToUnicode((str), 0)
#define Utf8ToTCHAR(str) akali_hpp::StringEncode::Utf8ToUnicode((str))
#define TCHARToUnicode(str) ((std::wstring)(str))
#define UnicodeToTCHAR(str) ((std::wstring)(str))
#else
#define TCHARToAnsi(str) ((std::string)(str))
#define TCHARToUtf8 akali_hpp::StringEncode::AnsiToUtf8((str), 0)
#define AnsiToTCHAR(str) ((std::string)(str))
#define Utf8ToTCHAR(str) akali_hpp::StringEncode::Utf8ToAnsi((str), 0)
#define TCHARToUnicode(str) akali_hpp::StringEncode::AnsiToUnicode((str), 0)
#define UnicodeToTCHAR(str) akali_hpp::StringEncode::UnicodeToAnsi((str), 0)
#endif
}  // namespace akali_hpp
#endif  // AKALI_STRING_ENCODE_HPP_
