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
    cout << akali_hpp::StringHelper::ToLower(s);
    cout << akali_hpp::StringHelper::ToUpper(s);

    wcout << akali_hpp::StringHelper::ToLower(sw);
    wcout << akali_hpp::StringHelper::ToUpper(sw);

    cout << akali_hpp::StringHelper::IsEqual(s, s2, false);
    cout << akali_hpp::StringHelper::IsEqual(s, s2, true);

    cout << akali_hpp::StringHelper::IsEqual(sw, sw2, false);
    cout << akali_hpp::StringHelper::IsEqual(sw, sw2, true);

    cout << akali_hpp::StringHelper::IsEqual(sw, sw3, true);

    cout << akali_hpp::StringHelper::IsStartsWith(s, "--b") << endl;
    cout << akali_hpp::StringHelper::ContainTimes(s, "") << endl;
    akali_hpp::WinVerInfo wvi = akali_hpp::OSVersion::GetWinVer();
    std::vector<std::string> v = akali_hpp::StringHelper::Split(s, "*");
    cout << akali_hpp::StringHelper::Join(v, "+") << endl;

    

    return 0;
}
