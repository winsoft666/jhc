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
#include <codecvt>
#include <locale>
#include <string>
#include "jhc/arch.hpp"
#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif // !_INC_WINDOWS
#endif // JHC_WIN

#pragma warning(disable : 4309)

namespace jhc {

class StringEncode {
   public:
#ifdef JHC_WIN
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

#endif

    static std::string UnicodeToUtf8(const std::wstring& str) {
#ifdef JHC_WIN
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
#else
        std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
        return converter.to_bytes(str);
#endif
    }

    static std::string UnicodeToUtf8BOM(const std::wstring& str) {
#ifdef JHC_WIN
        std::string strRes;

        const int iSize = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

        if (iSize == 0)
            return strRes;

        char* szBuf = new (std::nothrow) char[iSize + 3];

        if (!szBuf)
            return strRes;

        memset(szBuf, 0, iSize + 3);

        ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &szBuf[3], iSize, NULL, NULL);
        szBuf[0] = (char)0xef;
        szBuf[1] = (char)0xbb;
        szBuf[2] = (char)0xbf;

        strRes = szBuf;
        delete[] szBuf;

        return strRes;
#else
        std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
        const std::string stru8 = converter.to_bytes(str);
        std::string strRes(3, (const char)0);
        strRes[0] = (char)0xef;
        strRes[1] = (char)0xbb;
        strRes[2] = (char)0xbf;
        strRes += stru8;

        return strRes;
#endif
    }

    static std::wstring Utf8ToUnicode(const std::string& str) {
#ifdef JHC_WIN
        std::wstring strRes;
        const int iSize = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

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
#else
        std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
        return converter.from_bytes(str);
#endif
    }

#ifdef JHC_WIN
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
#endif  // JHC_STRING_ENCODE_HPP_
