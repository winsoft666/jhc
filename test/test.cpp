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

#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <map>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
//
// It is strongly recommended not to include this file directly in header-only mode!!!
#include "jhc_all.hpp"

// Test: uuid generator.
//
TEST_CASE("UUIDTest") {
    std::map<std::string, int> values;
    const int totalNum = 100000;

    for (int i = 0; i < totalNum; i++) {
        values[jhc::UUID::Create()] = i;
    }

    REQUIRE(values.size() == totalNum);
}

// Test: write/read big file.
//
TEST_CASE("FileTest1", "[w/r big file]") {
    constexpr int64_t bytes4gb = 4LL * 1024LL * 1024LL * 1024LL;
    const std::string str1K(1024, 'a');

    jhc::fs::path path1(u8"__file_test_文件测试1__.dat");
    jhc::fs::path openMode1(u8"ab+");

    jhc::File file1(path1);
    REQUIRE(file1.path() == path1);
    if (jhc::fs::exists(file1.path()))
        REQUIRE(jhc::fs::remove(file1.path()));

    REQUIRE(jhc::fs::exists(file1.path()) == false);
    REQUIRE(file1.isOpen() == false);
    REQUIRE(file1.exist() == false);
    REQUIRE(file1.open(openMode1));

    // write 4GB file
    for (size_t i = 0; i < 4 * 1024 * 1024; i++) {
        file1.writeFrom(str1K.c_str(), str1K.size());
    }

    REQUIRE(file1.flush());
    REQUIRE(file1.fileSize() == bytes4gb);

    std::string strRead(1024, '\0');
    REQUIRE(file1.readFrom((void*)&strRead[0], 1024, 0) == 1024);
    REQUIRE(strRead == str1K);

    std::string strRead2(1024, '\0');
    REQUIRE(file1.readFrom((void*)&strRead2[0], 1024, 3LL * 1024LL * 1024LL * 1024LL) == 1024);
    REQUIRE(strRead2 == str1K);

    REQUIRE(file1.fileSize() == bytes4gb);
    REQUIRE(file1.close());
    REQUIRE(jhc::fs::remove(file1.path()));
}

// Test: read all of file content
TEST_CASE("FileTest2", "[read all content]") {
    constexpr int64_t bytes4mb = 4LL * 1024LL * 1024LL;
    const std::string str1K(1024, 'a');
    jhc::fs::path path2(u8"__file_test_文件测试2__.dat");
    if (jhc::fs::exists(path2))
        REQUIRE(jhc::fs::remove(path2));

    jhc::File file2(path2);
    REQUIRE(file2.open("ab+"));

    // write 4MB file
    for (size_t i = 0; i < 4 * 1024; i++) {
        file2.writeFrom(str1K.c_str(), str1K.size());
    }

    REQUIRE(file2.flush());
    REQUIRE(file2.fileSize() == bytes4mb);
    std::string strAll;
    REQUIRE(file2.readAll(strAll));
    REQUIRE(strAll.size() == bytes4mb);
}

// Test: string hash.
//
TEST_CASE("HashTest1", "[stirng hash]") {
    const std::string str = "hello world!!!";
    REQUIRE(jhc::CRC32::GetDataCRC32((const unsigned char*)str.c_str(), str.length()) == "2416a280");
    REQUIRE(jhc::MD5::GetDataMD5((const unsigned char*)str.c_str(), str.length()) == "f6835168c4823ac89c1bc97154a675a8");
    REQUIRE(jhc::SHA1::GetDataSHA1((const unsigned char*)str.c_str(), str.length()) == "8233f28c479ff758b3b4ba9ad66069db68811e59");
    REQUIRE(jhc::SHA256::GetDataSHA256((const unsigned char*)str.c_str(), str.length()) == "a5f4396b45548597f81681147f53c66065d5137f2fbd85e6758a8983107228e4");
    REQUIRE(jhc::SHA512::GetDataSHA512((const unsigned char*)str.c_str(), str.length()) == "dfa6b727753e96a9dffb10947f99b8457ef51a634ea8baa0db0f22712f0bb19ac6719d4446109dc19dfc9629a3b225c0aeeebd61175464a95e35c437cd979a64");
}

// Test: file hash.
//
TEST_CASE("HashTest2", "[file hash]") {
    const std::string str1K(1024, 'a');
    jhc::File file1("hash_test_file.dat");
    REQUIRE(file1.open("wb"));
    REQUIRE(file1.writeFrom((void*)str1K.c_str(), str1K.size(), 0) == str1K.size());
    REQUIRE(file1.close());
#ifdef JHC_WIN
    REQUIRE(jhc::CRC32::GetFileCRC32(file1.path()) == "7c5597b9");
    REQUIRE(jhc::MD5::GetFileMD5(file1.path()) == "c9a34cfc85d982698c6ac89f76071abd");
    REQUIRE(jhc::SHA1::GetFileSHA1(file1.path()) == "8eca554631df9ead14510e1a70ae48c70f9b9384");
    REQUIRE(jhc::SHA256::GetFileSHA256(file1.path()) == "2edc986847e209b4016e141a6dc8716d3207350f416969382d431539bf292e4a");
    REQUIRE(jhc::SHA512::GetFileSHA512(file1.path()) == "74b22492e3b9a86a9c93c23a69f821ebafa429302c1f4054b4bc37356a4bae056d9ccbc6f24093a25704faaa72bd21a5f337ca9ec92f32369d24e6b9fae954d8");
#else
    REQUIRE(jhc::CRC32::GetFileCRC32(file1.path().string()) == "7c5597b9");
    REQUIRE(jhc::MD5::GetFileMD5(file1.path().string()) == "c9a34cfc85d982698c6ac89f76071abd");
    REQUIRE(jhc::SHA1::GetFileSHA1(file1.path().string()) == "8eca554631df9ead14510e1a70ae48c70f9b9384");
    REQUIRE(jhc::SHA256::GetFileSHA256(file1.path().string()) == "2edc986847e209b4016e141a6dc8716d3207350f416969382d431539bf292e4a");
    REQUIRE(jhc::SHA512::GetFileSHA512(file1.path().string()) == "74b22492e3b9a86a9c93c23a69f821ebafa429302c1f4054b4bc37356a4bae056d9ccbc6f24093a25704faaa72bd21a5f337ca9ec92f32369d24e6b9fae954d8");
#endif
}

// Test: string base64 encode/decode.
//
TEST_CASE("Base64Test") {
    REQUIRE(jhc::Base64::Encode("hello world!") == "aGVsbG8gd29ybGQh");
    REQUIRE(jhc::Base64::Decode("aGVsbG8gd29ybGQh") == "hello world!");
}

// Test: ip address check.
//
TEST_CASE("IpAddressTest") {
    REQUIRE(jhc::IPAddress::IPIsLoopback(jhc::IPAddress("192.168.50.12")) == false);
    REQUIRE(jhc::IPAddress::IPIsLoopback(jhc::IPAddress("127.0.0.1")));
}

// Test: string operate.
//
TEST_CASE("StringHelperTest1") {
    REQUIRE(jhc::StringHelper::ToLower('c') == 'c');
    REQUIRE(jhc::StringHelper::ToLower('C') == 'c');
    REQUIRE(jhc::StringHelper::ToUpper('A') == 'A');
    REQUIRE(jhc::StringHelper::ToUpper('a') == 'A');
    REQUIRE(jhc::StringHelper::ToLower(L'c') == L'c');
    REQUIRE(jhc::StringHelper::ToLower(L'C') == L'c');
    REQUIRE(jhc::StringHelper::ToUpper(L'A') == L'A');
    REQUIRE(jhc::StringHelper::ToUpper(L'a') == L'A');
    REQUIRE(jhc::StringHelper::ToLower("1234567890abcdefABCDEF#@!%%") == "1234567890abcdefabcdef#@!%%");
    REQUIRE(jhc::StringHelper::ToLower(L"1234567890abcdefABCDEF#@!%%") == L"1234567890abcdefabcdef#@!%%");
    REQUIRE(jhc::StringHelper::ToUpper("1234567890abcdefABCDEF#@!%%") == "1234567890ABCDEFABCDEF#@!%%");
    REQUIRE(jhc::StringHelper::ToUpper(L"1234567890abcdefABCDEF#@!%%") == L"1234567890ABCDEFABCDEF#@!%%");

    REQUIRE(jhc::StringHelper::IsDigit("3.14") == false);
    REQUIRE(jhc::StringHelper::IsDigit("3a12") == false);
    REQUIRE(jhc::StringHelper::IsDigit("134") == true);

    REQUIRE(jhc::StringHelper::IsDigit(L"3.14") == false);
    REQUIRE(jhc::StringHelper::IsDigit(L"3a12") == false);
    REQUIRE(jhc::StringHelper::IsDigit(L"134") == true);

    REQUIRE(jhc::StringHelper::Trim("\r\f erase\n\t white-spaces   \n") == "erase\n\t white-spaces");
    REQUIRE(jhc::StringHelper::Trim(L"\r\f erase\n\t white-spaces   \n") == L"erase\n\t white-spaces");
    REQUIRE(jhc::StringHelper::LeftTrim(L"\r\f erase\n\t white-spaces   \n") == L"erase\n\t white-spaces   \n");
    REQUIRE(jhc::StringHelper::LeftTrim("\r\f erase\n\t white-spaces   \n") == "erase\n\t white-spaces   \n");
    REQUIRE(jhc::StringHelper::RightTrim("\r\f erase\n\t white-spaces   \n") == "\r\f erase\n\t white-spaces");
    REQUIRE(jhc::StringHelper::RightTrim("\r\f erase\n\t white-spaces   \n") == "\r\f erase\n\t white-spaces");
    REQUIRE(jhc::StringHelper::RightTrim("\r\f erase\n\t white-spaces   \n", "") == "\r\f erase\n\t white-spaces   \n");
    REQUIRE(jhc::StringHelper::Trim("\r\f erase\n\t white-spaces   \n", "") == "\r\f erase\n\t white-spaces   \n");
    REQUIRE(jhc::StringHelper::Trim("\r\f erase\n\t white-spaces   \n", "\r\f") == " erase\n\t white-spaces   \n");

    REQUIRE(jhc::StringHelper::IsEqual("abcdefgxyz123#~/", "abcdefgxyz123#~/", false));
    REQUIRE(!jhc::StringHelper::IsEqual("abcdefgxyz123#~/", "abcdefgxyZ123#~/", false));
    REQUIRE(jhc::StringHelper::IsEqual("abcdefgxyz123#~/", "abcdefgxyZ123#~/", true));

    REQUIRE(jhc::StringHelper::IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyz123#~/", false));
    REQUIRE(!jhc::StringHelper::IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyZ123#~/", false));
    REQUIRE(jhc::StringHelper::IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyZ123#~/", true));

    REQUIRE(jhc::StringHelper::IsStartsWith("1234567890abcdef#@!%%", "1234567890"));
    REQUIRE(jhc::StringHelper::IsStartsWith(L"1234567890abcdef#@!%%", L"1234567890"));
    REQUIRE(!jhc::StringHelper::IsStartsWith("1234567890abcdef#@!%%", "abcdefg"));
    REQUIRE(!jhc::StringHelper::IsStartsWith(L"1234567890abcdef#@!%%", L"abcdefg"));

    REQUIRE(jhc::StringHelper::ContainTimes("123456712", "12") == 2);
    REQUIRE(jhc::StringHelper::ContainTimes(L"123456712", L"12") == 2);
    REQUIRE(jhc::StringHelper::ContainTimes("1234567121", "121") == 1);
    REQUIRE(jhc::StringHelper::ContainTimes(L"1234567121", L"121") == 1);
    REQUIRE(jhc::StringHelper::ContainTimes("123 4567 121", " ") == 2);
    REQUIRE(jhc::StringHelper::ContainTimes(L"123 4567 121", L" ") == 2);

    const std::string s1 = jhc::StringHelper::StringPrintf("%s's age is %d", "jack", 18);
    REQUIRE(s1 == "jack's age is 18");

    const std::wstring ws1 = jhc::StringHelper::StringPrintf(L"%ls's age is %d", L"jack", 18);
    REQUIRE(ws1 == L"jack's age is 18");

    std::string bigStrA(2048, 'a');
    bigStrA += "[end]";
    const std::string s2 = jhc::StringHelper::StringPrintf("%s length is %d", bigStrA.c_str(), bigStrA.length());
    REQUIRE(s2 == bigStrA + " length is " + std::to_string(bigStrA.length()));

    std::wstring bigStrW(2048, L'a');
    bigStrW += L"[end]";
    const std::wstring ws2 = jhc::StringHelper::StringPrintf(L"%ls length is %d", bigStrW.c_str(), bigStrW.length());
    REQUIRE(ws2 == bigStrW + L" length is " + std::to_wstring(bigStrW.length()));

    std::string bigStrA2(1024, 'c');
    const std::string s3 = jhc::StringHelper::StringPrintf("%s", bigStrA2.c_str());
    REQUIRE(s3 == bigStrA2);

    std::wstring bigStrW2(1024, L'c');
    const std::wstring sw3 = jhc::StringHelper::StringPrintf(L"%ls", bigStrW2.c_str());
    REQUIRE(sw3 == bigStrW2);
}

TEST_CASE("StringHelperTest2", "Find") {
    std::wstring s = L"@%SystemRoot%\\system32\\shell32.dll,-10113";
    std::wstring::size_type pos = jhc::StringHelper::Find(s, L"%systemroot%", 0, true);
    REQUIRE(pos == 1);

    std::wstring::size_type pos2 = jhc::StringHelper::Find(s, L"%systemroot%", 0, false);
    REQUIRE(pos2 == std::wstring::npos);

    std::wstring::size_type pos3 = jhc::StringHelper::Find(s, L"%SystemRoot%", 2, true);
    REQUIRE(pos3 == std::wstring::npos);

    std::wstring::size_type pos4 = jhc::StringHelper::Find(s, L"%SystemRoot%", 1, false);
    REQUIRE(pos4 == 1);

    std::wstring::size_type pos5 = jhc::StringHelper::Find(s, L"32", 23, false);
    REQUIRE(pos5 == 28);

    std::wstring::size_type pos6 = jhc::StringHelper::Find(s, L"32", 230, false);
    REQUIRE(pos6 == std::wstring::npos);
}

TEST_CASE("StringHelperTest3", "Repace") {
    std::wstring s = L"@%SystemRoot%\\system32\\%systemroot%.dll,-10113";
    std::wstring s1 = jhc::StringHelper::Replace(s, L"%systemroot%", L"c:", 0, false);
    REQUIRE(s1 == L"@%SystemRoot%\\system32\\c:.dll,-10113");

    std::wstring s2 = jhc::StringHelper::Replace(s, L"%systemroot%", L"c:", 0, true);
    REQUIRE(s2 == L"@c:\\system32\\c:.dll,-10113");

    std::wstring s3 = jhc::StringHelper::Replace(s, L"%systemroot%", L"c:", 200, true);
    REQUIRE(s3 == L"@%SystemRoot%\\system32\\%systemroot%.dll,-10113");
}

// Test: string encode, utf8/utf16
//
TEST_CASE("StringEncodeTest") {
    const std::string u8str = u8"中国china";
    const std::wstring wstr = L"中国china";
    REQUIRE(jhc::StringEncode::Utf8ToUnicode(u8str) == wstr);
    REQUIRE(jhc::StringEncode::UnicodeToUtf8(wstr) == u8str);
}

// Test: command line parser.
//
TEST_CASE("CommandLineParseTest") {
    std::wstring wparam = L"\"C:\\Program Files (x86)\\Google\\Chrome.exe\" -k1=v1 -k2:v2 /k3=v3 /k4:v4 /k5 -k6=v6= /k7=\"v7 /v=-'\"";
    jhc::CmdLineParser clp(wparam);
    printf("Key-Value list:\n");
    for (jhc::CmdLineParser::ITERPOS it = clp.begin(); it != clp.end(); ++it)
        printf("Key:%ls, Value:%ls\n", it->first.c_str(), it->second.c_str());
    printf("\n");

    REQUIRE(clp.getVal(L"k1") == L"v1");
    REQUIRE(clp.getVal(L"k2") == L"v2");
    REQUIRE(clp.getVal(L"k3") == L"v3");
    REQUIRE(clp.getVal(L"k4") == L"v4");
    REQUIRE(clp.getVal(L"k5") == L"");
    REQUIRE(clp.getVal(L"k6") == L"v6=");
    REQUIRE(clp.getVal(L"k7") == L"v7 /v=-'");
}

// Test: one way to create json.
//
TEST_CASE("JSONTest1", "[create1]") {
    const std::string expectJSON = R"({"answer":{"everything":42},"happy":true,"list":[1,0,2],"name":"Niels","nothing":null,"object":{"currency":"USD","value":42.99},"pi":3.141})";

    jhc::json j;
    j["pi"] = 3.141;
    j["happy"] = true;
    j["name"] = "Niels";
    j["nothing"] = nullptr;
    j["answer"]["everything"] = 42;
    j["list"] = {1, 0, 2};
    j["object"] = {{"currency", "USD"}, {"value", 42.99}};

    REQUIRE(j.dump() == expectJSON);
}

// Test: one way to create json.
//
TEST_CASE("JSONTest2", "[create2]") {
    const std::string expectJSON = R"({"answer":{"everything":42},"happy":true,"list":[1,0,2],"name":"Niels","nothing":null,"object":{"currency":"USD","value":42.99},"pi":3.141})";

    jhc::json j2 = {
        {"pi", 3.141},
        {"happy", true},
        {"name", "Niels"},
        {"nothing", nullptr},
        {"answer", {{"everything", 42}}},
        {"list", {1, 0, 2}},
        {"object", {{"currency", "USD"}, {"value", 42.99}}}};

    REQUIRE(j2.dump() == expectJSON);
}

// Test: one way to create json.
//
TEST_CASE("JSONTest3", "[create3]") {
    const std::string expectJSON = R"({"answer":{"everything":42},"happy":true,"list":[1,0,2],"name":"Niels","nothing":null,"object":{"currency":"USD","value":42.99},"pi":3.141})";

    jhc::json j3;
    j3["pi"] = 3.141;
    j3["happy"] = true;
    j3["name"] = "Niels";
    j3["nothing"] = nullptr;

    jhc::json j3_answer;
    j3_answer["everything"] = 42;

    j3["answer"] = j3_answer;

    jhc::json j3_list = jhc::json::array();
    j3_list.push_back(1);
    j3_list.push_back(0);
    j3_list.push_back(2);

    j3["list"] = j3_list;

    jhc::json j3_object;
    j3_object["currency"] = "USD";
    j3_object["value"] = 42.99;

    j3["object"] = j3_object;

    REQUIRE(j3.dump() == expectJSON);
}

// Test: one way to parse json.
//
TEST_CASE("JSONTest4", "[parse]") {
    // create json object
    jhc::json j;
    j["pi"] = 3.141;
    j["happy"] = true;
    j["name"] = "Niels";
    j["nothing"] = nullptr;
    j["answer"]["everything"] = 42;
    j["list"] = {1, 0, 2};
    j["object"] = {{"currency", "USD"}, {"value", 42.99}};

    const std::string strJson = j.dump();

    // parse json string
    auto jsonObj = jhc::json::parse(strJson);
    REQUIRE(IS_NEARLY_EQUAL(jsonObj["pi"].get<float>(), 3.141f));
    REQUIRE(IS_NEARLY_EQUAL(jsonObj["pi"].get<double>(), 3.141));
    REQUIRE(jsonObj["happy"].get<bool>() == true);
    REQUIRE(jsonObj["name"].get<std::string>() == "Niels");
    REQUIRE(jsonObj["nothing"] == nullptr);
    REQUIRE(jsonObj["answer"]["everything"].get<int>() == 42);
    REQUIRE(jsonObj["list"].size() == 3);
    REQUIRE(jsonObj["list"][0].get<int>() == 1);
    REQUIRE(jsonObj["list"][1].get<int>() == 0);
    REQUIRE(jsonObj["list"][2].get<int>() == 2);
    REQUIRE(jsonObj["object"]["currency"].get<std::string>() == "USD");
    REQUIRE(IS_NEARLY_EQUAL(jsonObj["object"]["value"].get<float>(), 42.99f));
}

// Test: path attribute getter/convert
//
TEST_CASE("FileSystemTest1", "[path attribute]") {
#ifdef JHC_WIN
    jhc::fs::path path1(u8"C:/testoi89hk8/abc/__filesystem_test1__.dat");
    REQUIRE(path1.wstring() == L"C:\\testoi89hk8\\abc\\__filesystem_test1__.dat");
    REQUIRE(path1.generic_wstring() == L"C:/testoi89hk8/abc/__filesystem_test1__.dat");

    REQUIRE(path1.string() == u8"C:\\testoi89hk8\\abc\\__filesystem_test1__.dat");
    REQUIRE(path1.generic_string() == u8"C:/testoi89hk8/abc/__filesystem_test1__.dat");

    REQUIRE(path1.has_extension());
    REQUIRE(path1.has_filename());
    REQUIRE(path1.has_parent_path());
    REQUIRE(path1.has_relative_path());
    REQUIRE(path1.has_root_directory());
    REQUIRE(path1.has_root_name());
    REQUIRE(path1.has_root_path());
    REQUIRE(path1.has_stem());

    REQUIRE(path1.extension().u8string() == u8".dat");
    REQUIRE(path1.filename() == u8"__filesystem_test1__.dat");
    REQUIRE(path1.stem() == u8"__filesystem_test1__");

    REQUIRE(path1.parent_path().wstring() == L"C:\\testoi89hk8\\abc");
    REQUIRE(path1.parent_path().generic_wstring() == L"C:/testoi89hk8/abc");

    REQUIRE(path1.relative_path().wstring() == L"testoi89hk8\\abc\\__filesystem_test1__.dat");
    REQUIRE(path1.root_name() == "C:");
    REQUIRE(path1.root_directory() == "\\");
    REQUIRE(path1.root_path() == "C:\\");

    path1.replace_filename(L"abctest.txt");
    REQUIRE(path1.string() == u8"C:\\testoi89hk8\\abc\\abctest.txt");

    path1.replace_extension(".txt");
    REQUIRE(path1.string() == u8"C:\\testoi89hk8\\abc\\abctest.txt");

    path1.replace_filename(u8"__filesystem_test1__.dat");
    REQUIRE(path1.generic_string() == u8"C:/testoi89hk8/abc/__filesystem_test1__.dat");

    path1 = L"C:\\testoi89hk8\\..\\123\\.\\__filesystem_test1__.dat";
    jhc::fs::path path2 = jhc::fs::absolute(path1);
    REQUIRE(path2.wstring() == L"C:\\123\\__filesystem_test1__.dat");
#else
    jhc::fs::path path1(u8"./testoi89hk8/abc/__filesystem_test1__.dat");
    REQUIRE(path1.wstring() == L"./testoi89hk8/abc/__filesystem_test1__.dat");
    REQUIRE(path1.generic_wstring() == L"./testoi89hk8/abc/__filesystem_test1__.dat");

    REQUIRE(path1.string() == u8"./testoi89hk8/abc/__filesystem_test1__.dat");
    REQUIRE(path1.generic_string() == u8"./testoi89hk8/abc/__filesystem_test1__.dat");

    REQUIRE(path1.has_extension());
    REQUIRE(path1.has_filename());
    REQUIRE(path1.has_stem());

    REQUIRE(path1.extension().u8string() == u8".dat");
    REQUIRE(path1.filename() == u8"__filesystem_test1__.dat");
    REQUIRE(path1.stem() == u8"__filesystem_test1__");

    REQUIRE(path1.parent_path().wstring() == L"./testoi89hk8/abc");
    REQUIRE(path1.parent_path().generic_wstring() == L"./testoi89hk8/abc");

    std::cout << std::endl;
    std::cout << "relative_path:" << path1.relative_path().string() << std::endl;
    std::cout << "root_name:" << path1.root_name().string() << std::endl;
    std::cout << "root_directory:" << path1.root_directory().string() << std::endl;
    std::cout << "root_path:" << path1.root_path().string() << std::endl;

    REQUIRE(path1.relative_path().string() == "./testoi89hk8/abc/__filesystem_test1__.dat");
    REQUIRE(path1.root_name() == "");
    REQUIRE(path1.root_directory() == "");
    REQUIRE(path1.root_path() == "");

    path1.replace_filename(L"abctest.txt");
    REQUIRE(path1.string() == u8"./testoi89hk8/abc/abctest.txt");

    path1.replace_extension(".txt");
    REQUIRE(path1.string() == u8"./testoi89hk8/abc/abctest.txt");

    path1.replace_filename(u8"__filesystem_test1__.dat");
    REQUIRE(path1.generic_string() == u8"./testoi89hk8/abc/__filesystem_test1__.dat");

    jhc::fs::path path2 = jhc::fs::absolute(path1);
    std::wcout << "absolute:" << path2.wstring() << std::endl;
    REQUIRE(path2.wstring().length() > 4);
#endif
}

// Test: remove empty directory.
//
TEST_CASE("FileSystemTest2", "[remove empty dir]") {
#ifdef JHC_WIN
    std::error_code ec;
    jhc::fs::path path2(L"C:\\testuy763e\\abc\\__filesystem_test2_" + std::to_wstring(time(nullptr)));
    REQUIRE(jhc::fs::exists(path2, ec) == false);
    REQUIRE(jhc::fs::create_directories(path2, ec));
    REQUIRE(jhc::fs::exists(path2, ec) == true);
    REQUIRE(jhc::fs::remove(path2, ec) == true);
#else
    std::error_code ec;
    jhc::fs::path path2("./testuy763e/abc/__filesystem_test2_" + std::to_string(time(nullptr)));
    REQUIRE(jhc::fs::exists(path2, ec) == false);
    REQUIRE(jhc::fs::create_directories(path2, ec));
    REQUIRE(jhc::fs::exists(path2, ec) == true);
    REQUIRE(jhc::fs::remove(path2, ec) == true);
#endif
}

// Test: remove non-empty directory.
//
TEST_CASE("FileSystemTest3", "[remove non-empty dir]") {
#ifdef JHC_WIN
    std::error_code ec;
    jhc::fs::path path3(L"C:\\testuy763e\\abc\\__filesystem_test3_" + std::to_wstring(time(nullptr)));
    REQUIRE(jhc::fs::exists(path3, ec) == false);
    REQUIRE(jhc::fs::create_directories(path3, ec));
    REQUIRE(jhc::fs::exists(path3, ec) == true);

    const std::string strWritten = "hello world";

    jhc::fs::path path4 = path3;
    path4.append("path4.txt");

    jhc::File file4(path4);
    REQUIRE(file4.open("wb") == true);
    REQUIRE(file4.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(file4.close());

    REQUIRE(jhc::fs::remove(path3, ec) == false);  // remove can only delete empty directory
    REQUIRE(jhc::fs::remove_all(path3, ec) == 2);  // return remove item count
    REQUIRE(jhc::fs::exists(path3, ec) == false);
    REQUIRE(jhc::fs::exists(path3.parent_path(), ec) == true);
#else
    std::error_code ec;
    jhc::fs::path path3(L"./testuy763e/abc/__filesystem_test3_" + std::to_wstring(time(nullptr)));
    REQUIRE(jhc::fs::exists(path3, ec) == false);
    REQUIRE(jhc::fs::create_directories(path3, ec));
    REQUIRE(jhc::fs::exists(path3, ec) == true);

    const std::string strWritten = "hello world";

    jhc::fs::path path4 = path3;
    path4.append("path4.txt");

    jhc::File file4(path4);
    REQUIRE(file4.open("wb") == true);
    REQUIRE(file4.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(file4.close());

    REQUIRE(jhc::fs::remove(path3, ec) == false);  // remove can only delete empty directory
    REQUIRE(jhc::fs::remove_all(path3, ec) == 2);  // return remove item count
    REQUIRE(jhc::fs::exists(path3, ec) == false);

    REQUIRE(jhc::fs::exists(path3.parent_path(), ec) == true);
#endif
}

// Test: Recursive remove non-empty directory.
//
TEST_CASE("FileSystemTest4", "[recursive remove non-empty attribute]") {
#ifdef JHC_WIN
    std::error_code ec;
    jhc::fs::path p1(L"C:\\test87w232\\abc\\__filesystem_test4_" + std::to_wstring(time(nullptr)));
    REQUIRE(jhc::fs::exists(p1, ec) == false);
    REQUIRE(jhc::fs::create_directories(p1, ec));
    REQUIRE(jhc::fs::exists(p1, ec) == true);

    const std::string strWritten = "hello world";

    // create C:\test\abc\__filesystem_test4_xxx\p2.txt
    jhc::fs::path p2 = p1;
    p2.append("p2.txt");

    jhc::File f2(p2);
    REQUIRE(f2.open("wb") == true);
    REQUIRE(f2.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(f2.close());

    // create C:\test\abc\p3.txt
    jhc::fs::path p3 = p1;
    p3.append("..\\p2.txt");

    jhc::File f3(p3);
    REQUIRE(f3.open("wb") == true);
    REQUIRE(f3.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(f3.close());

    // create C:\test\p4.txt
    jhc::fs::path p4 = p1;
    p4.append("..\\..\\p4.txt");

    jhc::File f4(p4);
    REQUIRE(f4.open("wb") == true);
    REQUIRE(f4.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(f4.close());

    REQUIRE(jhc::fs::remove(L"C:\\test87w232", ec) == false);  // remove can only delete empty directory
    REQUIRE(jhc::fs::remove_all(L"C:\\test87w232", ec) == 6);  // return remove item count
    REQUIRE(jhc::fs::exists(L"C:\\test87w232", ec) == false);
#else
    std::error_code ec;
    jhc::fs::path p1("~/test87w232/abc/__filesystem_test4_" + std::to_string(time(nullptr)));
    REQUIRE(jhc::fs::exists(p1, ec) == false);
    REQUIRE(jhc::fs::create_directories(p1, ec));
    REQUIRE(jhc::fs::exists(p1, ec) == true);

    const std::string strWritten = "hello world";

    // create ~/test87w232/abc/__filesystem_test4_xxx/p2.txt
    jhc::fs::path p2 = p1;
    p2.append("p2.txt");

    jhc::File f2(p2);
    REQUIRE(f2.open("wb") == true);
    REQUIRE(f2.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(f2.close());

    // create ~/test87w232/abc/p3.txt
    jhc::fs::path p3 = p1;
    p3.append("../p2.txt");

    jhc::File f3(p3);
    REQUIRE(f3.open("wb") == true);
    REQUIRE(f3.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(f3.close());

    // create ~/test87w232/p4.txt
    jhc::fs::path p4 = p1;
    p4.append("../../p4.txt");

    jhc::File f4(p4);
    REQUIRE(f4.open("wb") == true);
    REQUIRE(f4.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(f4.close());

    REQUIRE(jhc::fs::remove("~/test87w232", ec) == false);  // remove can only delete empty directory
    REQUIRE(jhc::fs::remove_all("~/test87w232", ec) == 6);  // return remove item count
    REQUIRE(jhc::fs::exists("~/test87w232", ec) == false);
#endif
}

#ifdef JHC_WIN
// Test: create process, send data to process's input and get process's output.
//
TEST_CASE("ProcessTest") {
#ifdef JHC_WIN
    jhc::Process proc(
        L"cmd.exe",
        jhc::Process::string_type(),
        [](const char* bytes, size_t n) {
            const std::string str(bytes, n);
            printf("%s", str.c_str());
        },
        [](const char* bytes, size_t n) {
            const std::string str(bytes, n);
            printf("%s", str.c_str());
        },
        true);

    REQUIRE(proc.successed());

    int exitStatus = 0;
    REQUIRE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    REQUIRE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    REQUIRE(proc.write(std::string("ping 127.0.0.1\n")));
    REQUIRE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    proc.killProcessTree();
    REQUIRE(proc.getExitStatus() > 0);
#else
    jhc::Process proc(
        "bash",
        jhc::Process::string_type(),
        [](const char* bytes, size_t n) {
            const std::string str(bytes, n);
            printf("%s", str.c_str());
        },
        [](const char* bytes, size_t n) {
            const std::string str(bytes, n);
            printf("%s", str.c_str());
        },
        true);

    REQUIRE(proc.successed());

    int exitStatus = 0;
    REQUIRE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    REQUIRE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    REQUIRE(proc.write(std::string("ping -c 6 127.0.0.1\n")));
    std::this_thread::sleep_for(std::chrono::seconds(3));
    proc.killProcessTree(true);
    REQUIRE(proc.getExitStatus() > 0);
#endif
}

// Test: Windows http get request
//
TEST_CASE("WinHttpTest1", "[get]") {
    jhc::WinHttp winHttp;
    REQUIRE(winHttp.openSession());
    REQUIRE(winHttp.openConnect(L"https://www.baidu.com"));
    REQUIRE(winHttp.openRequest(false));
    REQUIRE(winHttp.sendRequest());
    REQUIRE(winHttp.receiveResponse());

    const std::vector<BYTE> body = winHttp.getResponseBody();
    REQUIRE(body.size() > 0);

    const DWORD dwStatusCode = winHttp.getResponseStatusCode();
    REQUIRE(dwStatusCode == 200);

    const std::wstring strStatusTxt = winHttp.getResponseStatusText();
    REQUIRE(strStatusTxt == L"OK");

    const std::wstring strRawHeaders = winHttp.getResponseRawHeaders();
    REQUIRE(strRawHeaders.size() > 0);

    const std::unordered_map<std::wstring, std::wstring> headerMap = winHttp.getResponseHeaders();
    REQUIRE(headerMap.size() > 0);
}

// Test: Windows http post request
//
TEST_CASE("WinHttpTest2", "[post]") {
    std::string data = "test";

    jhc::WinHttp winHttp;
    REQUIRE(winHttp.openSession());
    REQUIRE(winHttp.openConnect(L"https://www.baidu.com"));
    REQUIRE(winHttp.openRequest(true));
    REQUIRE(winHttp.sendRequest((LPVOID)data.c_str(), data.size()));
    REQUIRE(winHttp.receiveResponse());

    const std::vector<BYTE> body = winHttp.getResponseBody();
    REQUIRE(body.size() > 0);

    const DWORD dwStatusCode = winHttp.getResponseStatusCode();
    REQUIRE(dwStatusCode == 200);

    const std::wstring strStatusTxt = winHttp.getResponseStatusText();
    REQUIRE(strStatusTxt == L"OK");

    const std::wstring strRawHeaders = winHttp.getResponseRawHeaders();
    REQUIRE(strRawHeaders.size() > 0);

    const std::unordered_map<std::wstring, std::wstring> headerMap = winHttp.getResponseHeaders();
    REQUIRE(headerMap.size() > 0);
}
#endif

// Test: Version compare
//
TEST_CASE("VersionCompareTest") {
    jhc::Version v1("1.2.3.4");
    jhc::Version v2("4.3.2");
    jhc::Version v3(L"4.3.2.1");
    jhc::Version v4 = v3;

    REQUIRE(v1.isValid());
    REQUIRE(v2.isValid());
    REQUIRE(v1.isSameFormat(v2) == false);
    REQUIRE(v2.isSameFormat(v1) == false);

    REQUIRE((v1 != v3));
    REQUIRE((v3 == v4));
    REQUIRE((v3 >= v4));
    REQUIRE((v3 <= v4));
    REQUIRE((v1 <= v4));
    REQUIRE((v1 < v4));
    REQUIRE((v4 >= v1));
    REQUIRE((v4 > v1));
}

// Test: Process singleton
//
TEST_CASE("SingletonProcessTest") {
    jhc::SingletonProcess::Instance()->markAndCheckStartup("test");
    REQUIRE(jhc::SingletonProcess::Instance()->isPrimary());
    REQUIRE(jhc::SingletonProcess::Instance()->isPrimary());

    jhc::SingletonProcess::Release();
    jhc::SingletonProcess::Instance()->markAndCheckStartup("test");
    REQUIRE(jhc::SingletonProcess::Instance()->isPrimary());
    REQUIRE(jhc::SingletonProcess::Instance()->isPrimary());
}

// Test: Windows virtual key convert.
TEST_CASE("WinVirtualKeyProcessTest") {
    REQUIRE(jhc::WinVirtualKey::ToInteger("VK_SHIFT") == 0x10);
    REQUIRE(jhc::WinVirtualKey::ToInteger("VK_EXECUTE") == 0x2B);
    REQUIRE(jhc::WinVirtualKey::ToInteger("VK_RETURN") == 0x0D);

    REQUIRE(jhc::WinVirtualKey::ToString(0x0D) == "VK_RETURN");
    REQUIRE(jhc::WinVirtualKey::ToString(0x2B) == "VK_EXECUTE");
}

TEST_CASE("TimerTest1", "Test start and stop") {
    {
        jhc::Timer t;
    }
}

TEST_CASE("TimerTest2", "Tests with two argument add") {
    jhc::Timer t;

    SECTION("Test uint64_t timeout argument") {
        int i = 0;
        t.add(100000, [&](std::size_t) { i = 42; });
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
        REQUIRE(i == 42);
    }

    SECTION("Test duration timeout argument") {
        int i = 0;
        t.add(std::chrono::milliseconds(100), [&](std::size_t) { i = 43; });
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
        REQUIRE(i == 43);
    }

    SECTION("Test time_point timeout argument") {
        int i = 0;
        t.add(std::chrono::steady_clock::now() + std::chrono::milliseconds(100), [&](std::size_t) { i = 44; });
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
        REQUIRE(i == 44);
    }
}

TEST_CASE("TimerTest3", "Tests with three argument add") {
    jhc::Timer t;

    SECTION("Test uint64_t timeout argument") {
        size_t count = 0;
        auto id = t.add(
            50000,  // 50ms
            [&](std::size_t) {
                ++count;
            },
            50000  // 50ms
        );
        std::this_thread::sleep_for(std::chrono::milliseconds(170));
        t.remove(id);
        REQUIRE(count == 3);
    }

    SECTION("Test duration timeout argument") {
        size_t count = 0;
        auto id =
            t.add(
                std::chrono::milliseconds(50),  // 50ms
                [&](std::size_t) { ++count; },
                std::chrono::microseconds(50000)  // 50ms
            );
        std::this_thread::sleep_for(std::chrono::milliseconds(235));
        t.remove(id);
        REQUIRE(count == 4);
    }
}

TEST_CASE("TimerTest4", "Test delete timer in callback") {
    jhc::Timer t;

    SECTION("Delete one timer") {
        size_t count = 0;
        t.add(
            std::chrono::milliseconds(10), [&](std::size_t id) {
                ++count;
                t.remove(id);
            },
            std::chrono::milliseconds(10));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        REQUIRE(count == 1);
    }

    SECTION("Ensure that the correct timer is freed and reused") {
        auto id1 = t.add(std::chrono::milliseconds(50), [](std::size_t) {});
        auto id2 = t.add(std::chrono::milliseconds(20), [&](std::size_t id) { t.remove(id); });
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        auto id3 = t.add(std::chrono::microseconds(100), [](std::size_t) {});
        auto id4 = t.add(std::chrono::microseconds(100), [](std::size_t) {});
        REQUIRE(id3 == id2);
        REQUIRE(id4 != id1);
        REQUIRE(id4 != id2);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    SECTION("Ensure that the correct timer is freed and reused - different ordering") {
        auto id1 = t.add(std::chrono::milliseconds(10), [&](std::size_t id) { t.remove(id); });
        auto id2 = t.add(std::chrono::milliseconds(40), [](std::size_t) {});
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        auto id3 = t.add(std::chrono::microseconds(100), [](std::size_t) {});
        auto id4 = t.add(std::chrono::microseconds(100), [](std::size_t) {});
        REQUIRE(id3 == id1);
        REQUIRE(id4 != id1);
        REQUIRE(id4 != id2);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

TEST_CASE("TimerTest5", "Test two identical timeouts") {
    int i = 0;
    int j = 0;
    jhc::Timer t;
    std::chrono::time_point<std::chrono::steady_clock> ts = std::chrono::steady_clock::now() + std::chrono::milliseconds(40);
    t.add(ts, [&](std::size_t) { i = 42; });
    t.add(ts, [&](std::size_t) { j = 43; });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(i == 42);
    REQUIRE(j == 43);
}

TEST_CASE("TimerTest6", "Test timeouts from the past.") {
    jhc::Timer t;

    SECTION("Test negative timeouts") {
        int i = 0;
        int j = 0;
        std::chrono::time_point<std::chrono::steady_clock> ts1 = std::chrono::steady_clock::now() - std::chrono::milliseconds(10);
        std::chrono::time_point<std::chrono::steady_clock> ts2 = std::chrono::steady_clock::now() - std::chrono::milliseconds(20);
        t.add(ts1, [&](std::size_t) { i = 42; });
        t.add(ts2, [&](std::size_t) { j = 43; });
        std::this_thread::sleep_for(std::chrono::microseconds(20));
        REQUIRE(i == 42);
        REQUIRE(j == 43);
    }

    SECTION("Test time overflow when blocking timer thread.") {
        int i = 0;
        std::chrono::time_point<std::chrono::steady_clock> ts1 = std::chrono::steady_clock::now() + std::chrono::milliseconds(10);
        std::chrono::time_point<std::chrono::steady_clock> ts2 = std::chrono::steady_clock::now() + std::chrono::milliseconds(20);
        t.add(ts1, [&](std::size_t) { std::this_thread::sleep_for(std::chrono::milliseconds(20)); });
        t.add(ts2, [&](std::size_t) { i = 42; });
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        REQUIRE(i == 42);
    }
}

TEST_CASE("TimerTest7", "Test order of multiple timeouts") {
    int i = 0;
    jhc::Timer t;
    t.add(10000, [&](std::size_t) { i = 42; });
    t.add(20000, [&](std::size_t) { i = 43; });
    t.add(30000, [&](std::size_t) { i = 44; });
    t.add(40000, [&](std::size_t) { i = 45; });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(i == 45);
}

TEST_CASE("TimerTest8", "Test with multiple timers") {
    int i = 0;
    jhc::Timer t1;
    jhc::Timer t2;

    SECTION("Update the same value at different times with different timers") {
        t1.add(std::chrono::milliseconds(20), [&](std::size_t) { i = 42; });
        t1.add(std::chrono::milliseconds(50), [&](std::size_t) { i = 43; });
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        REQUIRE(i == 42);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        REQUIRE(i == 43);
    }

    SECTION("Remove one timer without affecting the other") {
        auto id1 = t1.add(std::chrono::milliseconds(30), [&](std::size_t) { i = 42; });
        t1.add(std::chrono::milliseconds(50), [&](std::size_t) { i = 43; });
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        t1.remove(id1);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        REQUIRE(i == 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        REQUIRE(i == 43);
    }
}

TEST_CASE("TimerTest9", "Test remove timer id") {
    jhc::Timer t;

    SECTION("Remove out of range timer_id") {
        auto id = t.add(std::chrono::milliseconds(20), [](std::size_t) {});
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        auto res = t.remove(id + 1);
        REQUIRE(res == false);
    }
}

int main(int argc, char* argv[]) {
    return Catch::Session().run(argc, argv);
}