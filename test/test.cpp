﻿/*******************************************************************************
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

#include "akali_hpp.h"
#include <stdio.h>

using namespace std;

#define EXPECT_TRUE(x) do{ if(!(x)) printf("Not Pass: " #x "\n"); } while(false)

int main()
{
    EXPECT_TRUE(akali_hpp::StringHelper::ToLower("1234567890abcdefABCDEF#@!%%") == "1234567890abcdefabcdef#@!%%");
    EXPECT_TRUE(akali_hpp::StringHelper::ToLower(L"1234567890abcdefABCDEF#@!%%") == L"1234567890abcdefabcdef#@!%%");
    EXPECT_TRUE(akali_hpp::StringHelper::ToUpper("1234567890abcdefABCDEF#@!%%") == "1234567890ABCDEFABCDEF#@!%%");
    EXPECT_TRUE(akali_hpp::StringHelper::ToUpper(L"1234567890abcdefABCDEF#@!%%") == L"1234567890ABCDEFABCDEF#@!%%");

    EXPECT_TRUE(akali_hpp::StringHelper::IsEqual("abcdefgxyz123#~/", "abcdefgxyz123#~/", false));
    EXPECT_TRUE(!akali_hpp::StringHelper::IsEqual("abcdefgxyz123#~/", "abcdefgxyZ123#~/", false));
    EXPECT_TRUE(akali_hpp::StringHelper::IsEqual("abcdefgxyz123#~/", "abcdefgxyZ123#~/", true));

    EXPECT_TRUE(akali_hpp::StringHelper::IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyz123#~/", false));
    EXPECT_TRUE(!akali_hpp::StringHelper::IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyZ123#~/", false));
    EXPECT_TRUE(akali_hpp::StringHelper::IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyZ123#~/", true));


    EXPECT_TRUE(akali_hpp::StringHelper::IsStartsWith("1234567890abcdef#@!%%", "1234567890"));
    EXPECT_TRUE(akali_hpp::StringHelper::IsStartsWith(L"1234567890abcdef#@!%%", L"1234567890"));
    EXPECT_TRUE(!akali_hpp::StringHelper::IsStartsWith("1234567890abcdef#@!%%", "abcdefg"));
    EXPECT_TRUE(!akali_hpp::StringHelper::IsStartsWith(L"1234567890abcdef#@!%%", L"abcdefg"));

    EXPECT_TRUE(akali_hpp::StringHelper::ContainTimes("123456712", "12") == 2);
    EXPECT_TRUE(akali_hpp::StringHelper::ContainTimes(L"123456712", L"12") == 2);
    EXPECT_TRUE(akali_hpp::StringHelper::ContainTimes("1234567121", "121") == 1);
    EXPECT_TRUE(akali_hpp::StringHelper::ContainTimes(L"1234567121", L"121") == 1);
    EXPECT_TRUE(akali_hpp::StringHelper::ContainTimes("123 4567 121", " ") == 2);
    EXPECT_TRUE(akali_hpp::StringHelper::ContainTimes(L"123 4567 121", L" ") == 2);

    const std::string strOSVer = akali_hpp::OSVersion::GetOSVersion();
    printf("Current OS Version: %s\n", strOSVer.c_str());


    const std::string u8str = u8"中国china";
    const std::wstring wstr = L"中国china";
    EXPECT_TRUE(akali_hpp::StringEncode::Utf8ToUnicode(u8str) == wstr);
    EXPECT_TRUE(akali_hpp::StringEncode::UnicodeToUtf8(wstr) == u8str);


    return 0;
}
