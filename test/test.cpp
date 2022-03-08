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
#include <iostream>
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
    EXPECT_TRUE(akl::Md5::GetStringMd5((void*)str.c_str(), str.length()) == "f6835168c4823ac89c1bc97154a675a8");
}

void Base64Test() {
    EXPECT_TRUE(akl::Base64::Encode("hello world!") == "aGVsbG8gd29ybGQh");
    EXPECT_TRUE(akl::Base64::Decode("aGVsbG8gd29ybGQh") == "hello world!");
}

void IpAddressTest() {
    EXPECT_TRUE(akl::IPAddress::IPIsLoopback(akl::IPAddress("192.168.50.12")) == false);
    EXPECT_TRUE(akl::IPAddress::IPIsLoopback(akl::IPAddress("127.0.0.1")));
}

void StringHelperTest() {
    EXPECT_TRUE(akl::StringHelper::ToLower("1234567890abcdefABCDEF#@!%%") == "1234567890abcdefabcdef#@!%%");
    EXPECT_TRUE(akl::StringHelper::ToLower(L"1234567890abcdefABCDEF#@!%%") == L"1234567890abcdefabcdef#@!%%");
    EXPECT_TRUE(akl::StringHelper::ToUpper("1234567890abcdefABCDEF#@!%%") == "1234567890ABCDEFABCDEF#@!%%");
    EXPECT_TRUE(akl::StringHelper::ToUpper(L"1234567890abcdefABCDEF#@!%%") == L"1234567890ABCDEFABCDEF#@!%%");

    EXPECT_TRUE(akl::StringHelper::IsEqual("abcdefgxyz123#~/", "abcdefgxyz123#~/", false));
    EXPECT_TRUE(!akl::StringHelper::IsEqual("abcdefgxyz123#~/", "abcdefgxyZ123#~/", false));
    EXPECT_TRUE(akl::StringHelper::IsEqual("abcdefgxyz123#~/", "abcdefgxyZ123#~/", true));

    EXPECT_TRUE(akl::StringHelper::IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyz123#~/", false));
    EXPECT_TRUE(!akl::StringHelper::IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyZ123#~/", false));
    EXPECT_TRUE(akl::StringHelper::IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyZ123#~/", true));

    EXPECT_TRUE(akl::StringHelper::IsStartsWith("1234567890abcdef#@!%%", "1234567890"));
    EXPECT_TRUE(akl::StringHelper::IsStartsWith(L"1234567890abcdef#@!%%", L"1234567890"));
    EXPECT_TRUE(!akl::StringHelper::IsStartsWith("1234567890abcdef#@!%%", "abcdefg"));
    EXPECT_TRUE(!akl::StringHelper::IsStartsWith(L"1234567890abcdef#@!%%", L"abcdefg"));

    EXPECT_TRUE(akl::StringHelper::ContainTimes("123456712", "12") == 2);
    EXPECT_TRUE(akl::StringHelper::ContainTimes(L"123456712", L"12") == 2);
    EXPECT_TRUE(akl::StringHelper::ContainTimes("1234567121", "121") == 1);
    EXPECT_TRUE(akl::StringHelper::ContainTimes(L"1234567121", L"121") == 1);
    EXPECT_TRUE(akl::StringHelper::ContainTimes("123 4567 121", " ") == 2);
    EXPECT_TRUE(akl::StringHelper::ContainTimes(L"123 4567 121", L" ") == 2);

    const std::string s1 = akl::StringHelper::StringPrintf("%s's age is %d", "jack", 18);
    EXPECT_TRUE(s1 == "jack's age is 18");

    const std::wstring ws1 = akl::StringHelper::StringPrintf(L"%ls's age is %d", L"jack", 18);
    EXPECT_TRUE(ws1 == L"jack's age is 18");

    std::string bigStrA(2048, 'a');
    bigStrA += "[end]";
    const std::string s2 = akl::StringHelper::StringPrintf("%s length is %d", bigStrA.c_str(), bigStrA.length());
    EXPECT_TRUE(s2 == bigStrA + " length is " + std::to_string(bigStrA.length()));

    std::wstring bigStrW(2048, L'a');
    bigStrW += L"[end]";
    const std::wstring ws2 = akl::StringHelper::StringPrintf(L"%ls length is %d", bigStrW.c_str(), bigStrW.length());
    EXPECT_TRUE(ws2 == bigStrW + L" length is " + std::to_wstring(bigStrW.length()));

    std::string bigStrA2(1024, 'c');
    const std::string s3 = akl::StringHelper::StringPrintf("%s", bigStrA2.c_str());
    EXPECT_TRUE(s3 == bigStrA2);

    std::wstring bigStrW2(1024, L'c');
    const std::wstring sw3 = akl::StringHelper::StringPrintf(L"%ls", bigStrW2.c_str());
    EXPECT_TRUE(sw3 == bigStrW2);
}

void StringEncodeTest() {
    const std::string u8str = u8"中国china";
    const std::wstring wstr = L"中国china";
    EXPECT_TRUE(akl::StringEncode::Utf8ToUnicode(u8str) == wstr);
    EXPECT_TRUE(akl::StringEncode::UnicodeToUtf8(wstr) == u8str);
}

void PathTest() {
#ifdef AKALI_WIN
    printf("Windows Folder: %" PATH_FS "\n", akl::PathUtil::GetWindowsFolder().c_str());
    printf("System Folder: %" PATH_FS "\n", akl::PathUtil::GetSystemFolder().c_str());
    printf("Temp Folder: %" PATH_FS "\n", akl::PathUtil::GetTempFolder().c_str());
    printf("LocalAppData Folder: %" PATH_FS "\n", akl::PathUtil::GetLocalAppDataFolder().c_str());
#endif
}

void TraceTest() {
    std::string bigStrA(1080, 's');
    bigStrA += "[end]";

    std::wstring bigStrW(1080, L's');
    bigStrW += L"[end]";

    akl::Trace::MsgA("[%" PRId64 "] this big message output by akl::Trace::MsgA: %s\n", time(nullptr), bigStrA.c_str());
    akl::Trace::MsgW(L"[%" PRId64 "] this big message output by akl::Trace::MsgW: %ls\n", time(nullptr), bigStrW.c_str());
}

void CmdLineParserTest() {
    std::wstring wparam = L"\"C:\\Program Files (x86)\\Google\\Chrome.exe\" -k1=v1 -k2:v2 /k3=v3 /k4:v4 /k5 -k6=v6= /k7=\"v7 /v=-'\"";
    akl::CmdLineParser clp(wparam);
    printf("Key-Value list:\n");
    for (akl::CmdLineParser::ITERPOS it = clp.begin(); it != clp.end(); ++it)
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

    akl::json j;
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

    akl::json j2 = {
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

    akl::json j3;
    j3["pi"] = 3.141;
    j3["happy"] = true;
    j3["name"] = "Niels";
    j3["nothing"] = nullptr;

    akl::json j3_answer;
    j3_answer["everything"] = 42;

    j3["answer"] = j3_answer;

    akl::json j3_list = akl::json::array();
    j3_list.push_back(1);
    j3_list.push_back(0);
    j3_list.push_back(2);

    j3["list"] = j3_list;

    akl::json j3_object;
    j3_object["currency"] = "USD";
    j3_object["value"] = 42.99;

    j3["object"] = j3_object;

    EXPECT_TRUE(j3.dump() == expectJSON);
}

void ParseJsonMethod1Test() {
    // create json object
    akl::json j;
    j["pi"] = 3.141;
    j["happy"] = true;
    j["name"] = "Niels";
    j["nothing"] = nullptr;
    j["answer"]["everything"] = 42;
    j["list"] = {1, 0, 2};
    j["object"] = {{"currency", "USD"}, {"value", 42.99}};

    const std::string strJson = j.dump();

    // parse json string
    auto jsonObj = akl::json::parse(strJson);
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

void FileSystemTest1() {
#ifdef AKALI_WIN
    akl::filesystem::path path1(u8"C:/test/abc/__filesystem_test1__.dat");
    EXPECT_TRUE(path1.wstring() == L"C:\\test\\abc\\__filesystem_test1__.dat");
    EXPECT_TRUE(path1.generic_wstring() == L"C:/test/abc/__filesystem_test1__.dat");

    EXPECT_TRUE(path1.string() == u8"C:\\test\\abc\\__filesystem_test1__.dat");
    EXPECT_TRUE(path1.generic_string() == u8"C:/test/abc/__filesystem_test1__.dat");

    EXPECT_TRUE(path1.has_extension());
    EXPECT_TRUE(path1.has_filename());
    EXPECT_TRUE(path1.has_parent_path());
    EXPECT_TRUE(path1.has_relative_path());
    EXPECT_TRUE(path1.has_root_directory());
    EXPECT_TRUE(path1.has_root_name());
    EXPECT_TRUE(path1.has_root_path());
    EXPECT_TRUE(path1.has_stem());

    EXPECT_TRUE(path1.extension().u8string() == u8".dat");
    EXPECT_TRUE(path1.filename() == u8"__filesystem_test1__.dat");
    EXPECT_TRUE(path1.stem() == u8"__filesystem_test1__");

    EXPECT_TRUE(path1.parent_path().wstring() == L"C:\\test\\abc");
    EXPECT_TRUE(path1.parent_path().generic_wstring() == L"C:/test/abc");

    EXPECT_TRUE(path1.relative_path().wstring() == L"test\\abc\\__filesystem_test1__.dat");
    EXPECT_TRUE(path1.root_name() == "C:");
    EXPECT_TRUE(path1.root_directory() == "\\");
    EXPECT_TRUE(path1.root_path() == "C:\\");

    path1.replace_filename(L"abctest.txt");
    EXPECT_TRUE(path1.string() == u8"C:\\test\\abc\\abctest.txt");

    path1.replace_extension(".txt");
    EXPECT_TRUE(path1.string() == u8"C:\\test\\abc\\abctest.txt");

    path1.replace_filename(u8"__filesystem_test1__.dat");
    EXPECT_TRUE(path1.generic_string() == u8"C:/test/abc/__filesystem_test1__.dat");

    path1 = L"C:\\test\\..\\123\\.\\__filesystem_test1__.dat";
    akl::filesystem::path path2 = akl::filesystem::absolute(path1);
    EXPECT_TRUE(path2.wstring() == L"C:\\123\\__filesystem_test1__.dat");
#else
    akl::filesystem::path path1(u8"/test/abc/__filesystem_test1__.dat");
    EXPECT_TRUE(path1.wstring() == L"/test/abc/__filesystem_test1__.dat");
    EXPECT_TRUE(path1.generic_wstring() == L"/test/abc/__filesystem_test1__.dat");

    EXPECT_TRUE(path1.string() == u8"/test/abc/__filesystem_test1__.dat");
    EXPECT_TRUE(path1.generic_string() == u8"/test/abc/__filesystem_test1__.dat");

    EXPECT_TRUE(path1.has_extension());
    EXPECT_TRUE(path1.has_filename());
    EXPECT_TRUE(path1.has_parent_path());
    EXPECT_TRUE(path1.has_relative_path());
    EXPECT_TRUE(path1.has_root_directory());
    EXPECT_TRUE(path1.has_root_name() == false);
    EXPECT_TRUE(path1.has_root_path());
    EXPECT_TRUE(path1.has_stem());

    EXPECT_TRUE(path1.extension().u8string() == u8".dat");
    EXPECT_TRUE(path1.filename() == u8"__filesystem_test1__.dat");
    EXPECT_TRUE(path1.stem() == u8"__filesystem_test1__");

    EXPECT_TRUE(path1.parent_path().wstring() == L"/test/abc");
    EXPECT_TRUE(path1.parent_path().generic_wstring() == L"/test/abc");

    EXPECT_TRUE(path1.relative_path().wstring() == L"test/abc/__filesystem_test1__.dat");
    EXPECT_TRUE(path1.root_name() == "");
    EXPECT_TRUE(path1.root_directory() == "/");
    EXPECT_TRUE(path1.root_path() == "/");

    path1.replace_filename(L"abctest.txt");
    EXPECT_TRUE(path1.string() == u8"/test/abc/abctest.txt");

    path1.replace_extension(".txt");
    EXPECT_TRUE(path1.string() == u8"/test/abc/abctest.txt");

    path1.replace_filename(u8"__filesystem_test1__.dat");
    EXPECT_TRUE(path1.generic_string() == u8"/test/abc/__filesystem_test1__.dat");

    path1 = L".";
    akl::filesystem::path path2 = akl::filesystem::absolute(path1);
    std::wcout << path2.wstring() << std::endl;
    EXPECT_TRUE(path2.wstring().length() > 4);
#endif
}

void FileSystemTest2() {
    akl::filesystem::path path2(L"C:\\test\\abc\\__filesystem_test_2__.dat");
    if (akl::filesystem::exists(path2))
        EXPECT_TRUE(akl::filesystem::remove(path2));
}

void FileTest1() {
    constexpr int64_t bytes4gb = 4LL * 1024LL * 1024LL * 1024LL;
    const std::string str1K(1024, 'a');

    akl::filesystem::path path1(u8"__file_test_文件测试1__.dat");
    akl::filesystem::path openMode1(u8"ab+");

    akl::File file1(path1);
    EXPECT_TRUE(file1.path() == path1);
    if (akl::filesystem::exists(file1.path()))
        EXPECT_TRUE(akl::filesystem::remove(file1.path()));

    EXPECT_TRUE(akl::filesystem::exists(file1.path()) == false);
    EXPECT_TRUE(file1.isOpen() == false);
    EXPECT_TRUE(file1.exist() == false);
    EXPECT_TRUE(file1.open(openMode1));
    // write 4GB file
    for (size_t i = 0; i < 4 * 1024 * 1024; i++) {
        file1.writeFrom((void*)str1K.c_str(), str1K.size());
    }

    EXPECT_TRUE(file1.flush());
    EXPECT_TRUE(file1.fileSize() == bytes4gb);

    std::string strRead(1024, '\0');
    EXPECT_TRUE(file1.readFrom((void*)&strRead[0], 1024, 0) == 1024);
    EXPECT_TRUE(strRead == str1K);

    std::string strRead2(1024, '\0');
    EXPECT_TRUE(file1.readFrom((void*)&strRead2[0], 1024, 3LL * 1024LL * 1024LL * 1024LL) == 1024);
    EXPECT_TRUE(strRead2 == str1K);

    EXPECT_TRUE(file1.fileSize() == bytes4gb);
    EXPECT_TRUE(file1.close());
    EXPECT_TRUE(akl::filesystem::remove(file1.path()));
}

void ProcessTest() {
#ifdef AKALI_WIN
    akl::Process proc(
        L"cmd.exe",
        akl::Process::string_type(),
        [](const char* bytes, size_t n) {
            const std::string str(bytes, n);
            printf("%s", str.c_str());
        },
        [](const char* bytes, size_t n) {
            const std::string str(bytes, n);
            printf("%s", str.c_str());
        },
        true);

    EXPECT_TRUE(proc.successed());

    int exitStatus = 0;
    EXPECT_TRUE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_TRUE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_TRUE(proc.write(std::string("ping 127.0.0.1\n")));
    EXPECT_TRUE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    proc.killProcessTree();
    EXPECT_TRUE(proc.getExitStatus() > 0);
#else
    akl::Process proc(
        "bash",
        akl::Process::string_type(),
        [](const char* bytes, size_t n) {
            const std::string str(bytes, n);
            printf("%s", str.c_str());
        },
        [](const char* bytes, size_t n) {
            const std::string str(bytes, n);
            printf("%s", str.c_str());
        },
        true);

    EXPECT_TRUE(proc.successed());

    int exitStatus = 0;
    EXPECT_TRUE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_TRUE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_TRUE(proc.write(std::string("ping -c 6 127.0.0.1\n")));
    std::this_thread::sleep_for(std::chrono::seconds(3));
    proc.killProcessTree(true);
    EXPECT_TRUE(proc.getExitStatus() > 0);
#endif
}

int main() {
    printf("Current timestamp(by microseconds): %" PRId64 "\n", akl::TimeUtil::GetCurrentTimestampByMicroSec());

    const std::string strOSVer = akl::OSVersion::GetOSVersion();
    printf("Current OS Version: %s\n", strOSVer.c_str());

    const std::string strCurExePath = akl::ProcessUtil::GetCurrentProcessPath();
    printf("Current Path: %s\n", strCurExePath.c_str());

    ProcessTest();
    PathTest();
    Md5Test();
    Base64Test();
    IpAddressTest();
    StringHelperTest();
    StringEncodeTest();
    CmdLineParserTest();
    TraceTest();
    CreateJsonMethod1Test();
    CreateJsonMethod2Test();
    CreateJsonMethod3Test();
    ParseJsonMethod1Test();

    FileSystemTest1();
    FileSystemTest2();
    FileTest1();

    return 0;
}
