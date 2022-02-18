#include <iostream>
#include "akali_hpp/win_service_base.hpp"
#include "akali_hpp/win_env_variable.hpp"
#include "akali_hpp/string_helper.hpp"
#include "akali_hpp/os_ver.hpp"

using namespace std;

int main()
{
    std::string s = "--abc123456a1bc-*---";
    cout << akali_hpp::StringHelper::IsStartsWith(s, "--b") << endl;
    cout << akali_hpp::StringHelper::ContainTimes(s, "") << endl;
    akali_hpp::WinVerInfo wvi = akali_hpp::OSVersion::GetWinVer();
    std::vector<std::string> v = akali_hpp::StringHelper::Split(s, "*");
    cout << akali_hpp::StringHelper::Join(v, "+") << endl;

    return 0;
}
