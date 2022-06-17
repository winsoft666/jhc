#include "catch.hpp"
#include "jhc/win_shellink.hpp"
#include <Windows.h>
#include <ShlObj.h>
#include <strsafe.h>
#include "EnumIDList.hpp"
#include "PIDL.h"

// The number of in-use objects.
long objectCounter;

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
    CHECK(targetPath == LR"()"); // TODO

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

TEST_CASE("Shellink4", "load computer.lnk") {
    jhc::WinShellink wsl;
    jhc::WinShellink::ShellinkErr err = wsl.load(LR"(.\win_lnks\computer.lnk)");
    CHECK(err == jhc::WinShellink::ShellinkErr::SHLLINK_ERR_NONE);

    std::wstring displayName = wsl.getDisplayName();
    CHECK(displayName == LR"(@%windir%\explorer.exe,-304)");

    std::wstring targetPath = wsl.getTargetPath();
    CHECK(targetPath == LR"()"); // TODO

    std::wstring iconPath = wsl.getIconPath();
    CHECK(iconPath == LR"()");

    jhc::WinShellink::LinkTargetIDList idlist = wsl.linkTargetIdList();
    ITEMIDLIST* pIDL = (ITEMIDLIST*)(&idlist.IDListData[0]);



    UINT count = PIDL::ItemCount(pIDL);
    std::vector<IShellFolder*> out;
    HRESULT hr = PIDL::GetShellFoldersFor(pIDL, &out);
    wchar_t szPath[MAX_PATH] = {0};
    PIDL::GetFullPath(pIDL, NULL, szPath, MAX_PATH);
}
#endif