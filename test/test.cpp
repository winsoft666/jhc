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

#include <inttypes.h>
#include <stdio.h>
#include <time.h>
//
// It is strongly recommended not to include this file directly!!!
#include "akali_hpp.h"

#define EXPECT_TRUE(x)                                 \
    do {                                               \
        if ((x))                                       \
            printf("\033[32mOK:\033[0m " #x "\n");     \
        else                                           \
            printf("\033[31mFailed:\033[0m " #x "\n"); \
    } while (false)

void Md5Test() {
    const std::string str = "hello world!!!";
    EXPECT_TRUE(akali_hpp::Md5::GetStringMd5((void*)str.c_str(), str.length()) == "f6835168c4823ac89c1bc97154a675a8");
}

void Base64Test() {
    EXPECT_TRUE(akali_hpp::Base64::Encode("hello world!") == "aGVsbG8gd29ybGQh");
    EXPECT_TRUE(akali_hpp::Base64::Decode("aGVsbG8gd29ybGQh") == "hello world!");
}

void StringHelperTest() {
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

    const std::string s1 = akali_hpp::StringHelper::StringPrintf("%s's age is %d", "jack", 18);
    EXPECT_TRUE(s1 == "jack's age is 18");

    const std::wstring ws1 = akali_hpp::StringHelper::StringPrintf(L"%s's age is %d", L"jack", 18);
    EXPECT_TRUE(ws1 == L"jack's age is 18");

    const std::string bigStr(2048, 'a');
}

void StringEncodeTest() {
    const std::string u8str = u8"中国china";
    const std::wstring wstr = L"中国china";
    EXPECT_TRUE(akali_hpp::StringEncode::Utf8ToUnicode(u8str) == wstr);
    EXPECT_TRUE(akali_hpp::StringEncode::UnicodeToUtf8(wstr) == u8str);
}

void SpdlogTest() {
    EXPECT_TRUE(akali_hpp::SpdlogWrapper::GlobalRegister("akali_hpp_tester"));
    akali_hpp::SpdlogWrapper::Trace("this is trace log");
    akali_hpp::SpdlogWrapper::Info("this is info log");
    akali_hpp::SpdlogWrapper::Warn("this is warn log");
    akali_hpp::SpdlogWrapper::Error("this is error log");
    akali_hpp::SpdlogWrapper::Critical("this is critical log");

    akali_hpp::Trace::MsgA("[%ld] this message output by akali_hpp::Trace::MsgA\n", time(0));
    akali_hpp::Trace::MsgW(L"[%ld] this message output by akali_hpp::Trace::MsgW\n", time(0));
}

void CmdLineParserTest() {
    std::wstring wparam = L"\"C:\\Program Files (x86)\\Google\\Chrome.exe\" -k1=v1 -k2:v2 /k3=v3 /k4:v4 /k5 -k6=v6= /k7=\"v7 /v=-'\"";
    akali_hpp::CmdLineParser clp(wparam);
    printf("Key-Value list:\n");
    for (akali_hpp::CmdLineParser::ITERPOS it = clp.begin(); it != clp.end(); ++it)
        printf("Key:%ls, Value:%ls\n", it->first.c_str(), it->second.c_str());
    printf("\n");

    EXPECT_TRUE(clp.getVal(L"k1") == L"v1");
    EXPECT_TRUE(clp.getVal(L"k2") == L"v2");
    EXPECT_TRUE(clp.getVal(L"k3") == L"v3");
    EXPECT_TRUE(clp.getVal(L"k4") == L"v4");
    EXPECT_TRUE(clp.getVal(L"k5") == L"");
    EXPECT_TRUE(clp.getVal(L"k6") == L"v6=");
    EXPECT_TRUE(clp.getVal(L"k7") == L"v7 /v=-'");
}

void CreateJsonMethod1Test() {
    const std::string expectJSON = R"({"answer":{"everything":42},"happy":true,"list":[1,0,2],"name":"Niels","nothing":null,"object":{"currency":"USD","value":42.99},"pi":3.141})";

    akali_hpp::json j;
    j["pi"] = 3.141;
    j["happy"] = true;
    j["name"] = "Niels";
    j["nothing"] = nullptr;
    j["answer"]["everything"] = 42;
    j["list"] = {1, 0, 2};
    j["object"] = {{"currency", "USD"}, {"value", 42.99}};

    EXPECT_TRUE(j.dump() == expectJSON);
}

void CreateJsonMethod2Test() {
    const std::string expectJSON = R"({"answer":{"everything":42},"happy":true,"list":[1,0,2],"name":"Niels","nothing":null,"object":{"currency":"USD","value":42.99},"pi":3.141})";

    akali_hpp::json j2 = {
        {"pi", 3.141},
        {"happy", true},
        {"name", "Niels"},
        {"nothing", nullptr},
        {"answer", {{"everything", 42}}},
        {"list", {1, 0, 2}},
        {"object", {{"currency", "USD"}, {"value", 42.99}}}};

    EXPECT_TRUE(j2.dump() == expectJSON);
}

void CreateJsonMethod3Test() {
    const std::string expectJSON = R"({"answer":{"everything":42},"happy":true,"list":[1,0,2],"name":"Niels","nothing":null,"object":{"currency":"USD","value":42.99},"pi":3.141})";

    akali_hpp::json j3;
    j3["pi"] = 3.141;
    j3["happy"] = true;
    j3["name"] = "Niels";
    j3["nothing"] = nullptr;

    akali_hpp::json j3_answer;
    j3_answer["everything"] = 42;

    j3["answer"] = j3_answer;

    akali_hpp::json j3_list = akali_hpp::json::array();
    j3_list.push_back(1);
    j3_list.push_back(0);
    j3_list.push_back(2);

    j3["list"] = j3_list;

    akali_hpp::json j3_object;
    j3_object["currency"] = "USD";
    j3_object["value"] = 42.99;

    j3["object"] = j3_object;

    EXPECT_TRUE(j3.dump() == expectJSON);
}

void ParseJsonMethod1Test() {
    // create json object
    akali_hpp::json j;
    j["pi"] = 3.141;
    j["happy"] = true;
    j["name"] = "Niels";
    j["nothing"] = nullptr;
    j["answer"]["everything"] = 42;
    j["list"] = {1, 0, 2};
    j["object"] = {{"currency", "USD"}, {"value", 42.99}};

    const std::string strJson = j.dump();

    // parse json string
    auto jsonObj = akali_hpp::json::parse(strJson);
    EXPECT_TRUE(IS_NEARLY_EQUAL(jsonObj["pi"].get<float>(), 3.141f));
    EXPECT_TRUE(IS_NEARLY_EQUAL(jsonObj["pi"].get<double>(), 3.141));
    EXPECT_TRUE(jsonObj["happy"].get<bool>() == true);
    EXPECT_TRUE(jsonObj["name"].get<std::string>() == "Niels");
    EXPECT_TRUE(jsonObj["nothing"] == nullptr);
    EXPECT_TRUE(jsonObj["answer"]["everything"].get<int>() == 42);
    EXPECT_TRUE(jsonObj["list"].size() == 3);
    EXPECT_TRUE(jsonObj["list"][0].get<int>() == 1);
    EXPECT_TRUE(jsonObj["list"][1].get<int>() == 0);
    EXPECT_TRUE(jsonObj["list"][2].get<int>() == 2);
    EXPECT_TRUE(jsonObj["object"]["currency"].get<std::string>() == "USD");
    EXPECT_TRUE(IS_NEARLY_EQUAL(jsonObj["object"]["value"].get<float>(), 42.99f));
}


int main() {
    printf("Current timestamp(by microseconds): %" PRId64 "\n", akali_hpp::TimeUtil::GetCurrentTimestampByMicroSec());

    const std::string strOSVer = akali_hpp::OSVersion::GetOSVersion();
    printf("Current OS Version: %s\n", strOSVer.c_str());

    const std::string strCurExePath = akali_hpp::ProcessUtil::GetCurrentProcessPath();
    printf("Current Path: %s\n", strCurExePath.c_str());

    Md5Test();
    Base64Test();
    StringHelperTest();
    StringEncodeTest();
    CmdLineParserTest();
    SpdlogTest();

    CreateJsonMethod1Test();
    CreateJsonMethod2Test();
    CreateJsonMethod3Test();
    ParseJsonMethod1Test();

    return 0;
}
