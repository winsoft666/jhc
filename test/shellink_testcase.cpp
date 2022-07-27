#include "catch.hpp"
#include "jhc/win_shellink.hpp"
#include <Windows.h>
#include <ShlObj.h>
#include <strsafe.h>

// The number of in-use objects.
long objectCounter;

#ifdef JHC_WIN
TEST_CASE("Shellink1", "load Postman.lnk") {
    jhc::WinShellink wsl;
    jhc::WinShellink::ShellinkErr err = wsl.load(LR"(.\win_lnks\Postman.lnk)");
    CHECK(err == jhc::WinShellink::ShellinkErr::SHLLINK_ERR_NONE);

    std::wstring displayName = wsl.getDescription();
    CHECK(displayName == L"Postman");

    std::wstring targetPath = wsl.getTargetPath();
    CHECK(targetPath == LR"(C:\Users\jsme\AppData\Local\Postman\Postman.exe)");

    std::wstring iconPath = wsl.getIconPath();
    CHECK(iconPath == LR"(%USERPROFILE%\AppData\Local\Postman\Postman.exe)");

    bool isRunAsAdmin = wsl.isRunAsAdmin();
    CHECK(isRunAsAdmin == false);
}

TEST_CASE("Shellink2", "load 'Shows Desktop.lnk'") {
    jhc::WinShellink wsl;
    jhc::WinShellink::ShellinkErr err = wsl.load(LR"(.\win_lnks\Shows Desktop.lnk)");
    CHECK(err == jhc::WinShellink::ShellinkErr::SHLLINK_ERR_NONE);

    std::wstring displayName = wsl.getDescription();
    if (jhc::WinShellink::IsResourceString(displayName)) {
        std::wstring ret;
        if (jhc::WinShellink::LoadStringFromRes(displayName, ret))
            displayName = ret;
    }
    CHECK(displayName == LR"(œ‘ æ◊¿√Ê)");

    std::wstring targetPath = wsl.getTargetPath();
    CHECK(targetPath == LR"(::{3080F90D-D7AD-11D9-BD98-0000947B0257})");

    std::wstring iconPath = wsl.getIconPath();
    CHECK(iconPath == LR"(%windir%\system32\imageres.dll)");
}

TEST_CASE("Shellink3", "load VMCreate.lnk") {
    jhc::WinShellink wsl;
    jhc::WinShellink::ShellinkErr err = wsl.load(LR"(.\win_lnks\VMCreate.lnk)");
    CHECK(err == jhc::WinShellink::ShellinkErr::SHLLINK_ERR_NONE);

    std::wstring displayName = wsl.getDescription();
    if (jhc::WinShellink::IsResourceString(displayName)) {
        std::wstring ret;
        if (jhc::WinShellink::LoadStringFromRes(displayName, ret))
            displayName = ret;
    }
    CHECK(displayName == LR"(Hyper-V Quick Create enables rapid creation of virtual machines from a gallery of curated virtual machine images.)");

    std::wstring targetPath = wsl.getTargetPath();
    CHECK(targetPath == LR"(%ProgramFiles%\Hyper-V\VMCreate.exe)");

    std::wstring iconPath = wsl.getIconPath();
    CHECK(iconPath == LR"()");
}

TEST_CASE("Shellink4", "load computer.lnk") {
    jhc::WinShellink wsl;
    jhc::WinShellink::ShellinkErr err = wsl.load(LR"(.\win_lnks\computer.lnk)");
    CHECK(err == jhc::WinShellink::ShellinkErr::SHLLINK_ERR_NONE);

    std::wstring displayName = wsl.getDescription();
    CHECK(displayName == LR"(@%windir%\explorer.exe,-304)");

    std::wstring targetPath = wsl.getTargetPath();
    CHECK(targetPath == LR"(::{20D04FE0-3AEA-1069-A2D8-08002B30309D})");

    std::wstring iconPath = wsl.getIconPath();
    CHECK(iconPath == LR"()");
}

TEST_CASE("Shellink5", "load 'Math Input Panel.lnk'") {
#ifdef JHC_WIN64
    jhc::WinShellink wsl;
    jhc::WinShellink::ShellinkErr err = wsl.load(LR"(.\win_lnks\Math Input Panel.lnk)");
    CHECK(err == jhc::WinShellink::ShellinkErr::SHLLINK_ERR_NONE);

    std::wstring displayName = wsl.getDescription();
    if (jhc::WinShellink::IsResourceString(displayName)) {
        std::wstring ret;
        if (jhc::WinShellink::LoadStringFromRes(displayName, ret))
            displayName = ret;
    }
    CHECK(displayName == LR"(Math Input Panel)");

    std::wstring targetPath = wsl.getTargetPath();
    CHECK(targetPath == LR"(%CommonProgramFiles%\Microsoft Shared\Ink\mip.exe)");

    std::wstring iconPath = wsl.getIconPath();
    CHECK(iconPath == LR"(%CommonProgramFiles%\Microsoft Shared\Ink\mip.exe)");
#endif
}
#endif