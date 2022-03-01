#include "akali_hpp.h"
#include <iostream>

using namespace std;

int main()
{
    const std::string s = "--abc123456A1bc-*---";
    const std::string s2 = "--abc123456A1Bc-*---";
    const std::wstring sw = L"--abc123456A1bc-*---";
    const std::wstring sw2 = L"--abc123456A1bC-*---";
    const std::wstring sw3 = L"--abc123456A1bC--";

    cout << akali_hpp::StringHelper::ToLower(s) << endl;
    cout << akali_hpp::StringHelper::ToUpper(s) << endl;

    wcout << akali_hpp::StringHelper::ToLower(sw) << endl;
    wcout << akali_hpp::StringHelper::ToUpper(sw) << endl;

    cout << akali_hpp::StringHelper::IsEqual(s, s2, false) << endl;
    cout << akali_hpp::StringHelper::IsEqual(s, s2, true) << endl;

    cout << akali_hpp::StringHelper::IsEqual(sw, sw2, false) << endl;
    cout << akali_hpp::StringHelper::IsEqual(sw, sw2, true) << endl;

    cout << akali_hpp::StringHelper::IsEqual(sw, sw3, true) << endl;

    cout << akali_hpp::StringHelper::IsStartsWith(s, "--b") << endl;
    cout << akali_hpp::StringHelper::ContainTimes(s, "") << endl;
    akali_hpp::WinVerInfo wvi = akali_hpp::OSVersion::GetWinVer();
    std::vector<std::string> v = akali_hpp::StringHelper::Split(s, "*");
    cout << akali_hpp::StringHelper::Join(v, "+") << endl;

    const std::string base64Encoded = akali_hpp::Base64::Encode("https://www.google.com/?q=test", true);
    cout << base64Encoded << endl;
    cout << akali_hpp::Base64::Decode(base64Encoded) << endl;

    const std::string hexEncoded = akali_hpp::HexEncode::Encode("123456789");
    cout << hexEncoded << endl;
    cout << akali_hpp::HexEncode::Decode(hexEncoded) << endl;

    const std::string urlEncoded = akali_hpp::UrlEncode::Encode("https://www.google.com/?q=test&a=1");
    cout << urlEncoded << endl;
    cout << akali_hpp::UrlEncode::Decode(urlEncoded) << endl;

    cout << akali_hpp::ProcessUtil::GetCurrentProcessPath() << endl;

    if (akali_hpp::SpdlogWrapper::GlobalRegister("akali_hpp_tester")) {
        for (int i = 0; i < 10000; i++) {
            akali_hpp::SpdlogWrapper::Trace("this is a test log, id is {}", i);
        }
    }

    return 0;
}
