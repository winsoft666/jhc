#include "catch.hpp"
#include "jhc/win_shellink.hpp"

#ifdef JHC_WIN
TEST_CASE("Shellink1", "load Postman.lnk") {
    jhc::WinShellink wsl;
    jhc::WinShellink::ShellinkErr err = wsl.load(LR"(.\win_lnks\Postman.lnk)");
    CHECK(err == jhc::WinShellink::ShellinkErr::SHLLINK_ERR_NONE);

    std::wstring displayName = wsl.getDisplayName();
    CHECK(displayName == L"Postman");

    std::wstring targetPath = wsl.getTargetPath();
    CHECK(targetPath == LR"(C:\Users\jsme\AppData\Local\Postman\Postman.exe)");

    std::wstring iconPath = wsl.getIconPath();
    CHECK(iconPath == LR"(%USERPROFILE%\AppData\Local\Postman\Postman.exe)");
}

TEST_CASE("Shellink2", "load 'Shows Desktop.lnk'") {
    jhc::WinShellink wsl;
    jhc::WinShellink::ShellinkErr err = wsl.load(LR"(.\win_lnks\Shows Desktop.lnk)");
    CHECK(err == jhc::WinShellink::ShellinkErr::SHLLINK_ERR_NONE);

    std::wstring displayName = wsl.getDisplayName();
    CHECK(displayName == LR"(@%SystemRoot%\system32\shell32.dll,-10113)");

    std::wstring targetPath = wsl.getTargetPath();
    CHECK(targetPath == LR"()");

    std::wstring iconPath = wsl.getIconPath();
    CHECK(iconPath == LR"(%windir%\system32\imageres.dll)");
}

TEST_CASE("Shellink3", "load VMCreate.lnk") {
    jhc::WinShellink wsl;
    jhc::WinShellink::ShellinkErr err = wsl.load(LR"(.\win_lnks\VMCreate.lnk)");
    CHECK(err == jhc::WinShellink::ShellinkErr::SHLLINK_ERR_NONE);

    std::wstring displayName = wsl.getDisplayName();
    CHECK(displayName == LR"(VMCreate)");

    std::wstring targetPath = wsl.getTargetPath();
    CHECK(targetPath == LR"(%ProgramFiles%\Hyper-V\VMCreate.exe)");

    std::wstring iconPath = wsl.getIconPath();
    CHECK(iconPath == LR"()");
}
#endif