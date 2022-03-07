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

#ifndef AKALI_WIN_MAIN_HPP_
#define AKALI_WIN_MAIN_HPP_
#pragma once
#include "akali_hpp/arch.hpp"

#ifdef AKALI_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif
#include <tchar.h>
#include <DbgHelp.h>
#include <strsafe.h>

namespace akl {
class WinExceptionCatchInternal {
   public:
    typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

    static TCHAR* lstrrchr(LPCTSTR string, int ch) {
        TCHAR* start = (TCHAR*)string;

        while (*string++)
            ;

        while (--string != start && *string != (TCHAR)ch)
            ;

        if (*string == (TCHAR)ch)
            return (TCHAR*)string;

        return NULL;
    }

    static void DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo) {
        if (!excpInfo) {
            static int iTimes = 0;

            if (iTimes++ > 1)
                return;

            __try {
                RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
            } __except (DumpMiniDump(hFile, GetExceptionInformation()), EXCEPTION_CONTINUE_EXECUTION) {
            }
        }
        else {
            MINIDUMP_EXCEPTION_INFORMATION eInfo;
            eInfo.ThreadId = GetCurrentThreadId();
            eInfo.ExceptionPointers = excpInfo;
            eInfo.ClientPointers = FALSE;

            MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal,
                              excpInfo ? &eInfo : NULL, NULL, NULL);
        }
    }

    static int __cdecl RecordExceptionInfo(PEXCEPTION_POINTERS pExceptPtrs, const TCHAR* szDumpNamePrefix) {
        static bool bFirstTime = true;

        if (!bFirstTime)
            return EXCEPTION_CONTINUE_SEARCH;

        bFirstTime = false;

        // create a unique string.
        //
        TCHAR szLocalTime[50] = {0};
        SYSTEMTIME st;
        GetLocalTime(&st);
        StringCchPrintf(szLocalTime, 50, TEXT("%04d%02d%02d.%02d.%02d.%02d.%04d"), st.wYear, st.wMonth,
                        st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        TCHAR szExeDir[MAX_PATH + 1] = {0};

        GetModuleFileName(NULL, szExeDir, MAX_PATH);

        if (TCHAR* p = lstrrchr(szExeDir, TEXT('\\'))) {
            *(p + 1) = 0;
        }

        DWORD dwCurrentPID = GetCurrentProcessId();

        TCHAR szDumpFileName[MAX_PATH + 1] = {0};
        _stprintf_s(szDumpFileName, MAX_PATH, TEXT("%s%s_%ld_%s.dmp"), szExeDir, szDumpNamePrefix,
                    dwCurrentPID, szLocalTime);

        HANDLE hMiniDumpFile = CreateFile(szDumpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                                          FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);

        if (hMiniDumpFile != INVALID_HANDLE_VALUE) {
            DumpMiniDump(hMiniDumpFile, pExceptPtrs);

            CloseHandle(hMiniDumpFile);
            hMiniDumpFile = NULL;
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }
};
}  // namespace akl

#define WINMAIN_BEGIN(szDumpNamePrefix)                                                                 \
    int __96A9695E_RUN_WINMAIN_FUNC(HINSTANCE hInstance, LPTSTR lpCmdLine);                             \
    LONG WINAPI __96A9695E_UnhandledExceptionHandler(_EXCEPTION_POINTERS* pExceptionInfo) {             \
        OutputDebugString(TEXT("Create a dump file sine an exception occurred in sub-thread.\n"));      \
        int iRet = akl::WinExceptionCatchInternal::RecordExceptionInfo(pExceptionInfo, szDumpNamePrefix);                        \
        return iRet;                                                                                    \
    }                                                                                                   \
    int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,              \
                           int nCmdShow) {                                                              \
        UNREFERENCED_PARAMETER(hPrevInstance);                                                          \
        UNREFERENCED_PARAMETER(nCmdShow);                                                               \
        ::SetUnhandledExceptionFilter(__96A9695E_UnhandledExceptionHandler);                            \
        int ret = 0;                                                                                    \
        __try {                                                                                         \
            ret = __96A9695E_RUN_WINMAIN_FUNC(hInstance, lpCmdLine);                                    \
        } __except (akl::WinExceptionCatchInternal::RecordExceptionInfo(GetExceptionInformation(), szDumpNamePrefix)) {          \
            OutputDebugString(TEXT("Create a dump file sine an exception occurred in main-thread.\n")); \
        }                                                                                               \
        return ret;                                                                                     \
    }                                                                                                   \
    int __96A9695E_RUN_WINMAIN_FUNC(HINSTANCE hInstance, LPTSTR lpCmdLine) {
#define WINMAIN_END }

#define WMAIN_BEGIN(szDumpName)                                                                     \
    int __96A9695E_RUN_MAIN_FUNC(int argc, wchar_t* argv[]);                                        \
    LONG WINAPI __96A9695E_UnhandledExceptionHandler(_EXCEPTION_POINTERS* pExceptionInfo) {         \
        OutputDebugString(TEXT("Create a dump file since an exception occurred in sub-thread.\n")); \
        int iRet = akl::WinExceptionCatchInternal::RecordExceptionInfo(pExceptionInfo, szDumpName);                          \
        return iRet;                                                                                \
    }                                                                                               \
    int wmain(int argc, wchar_t* argv[]) {                                                          \
        ::SetUnhandledExceptionFilter(__96A9695E_UnhandledExceptionHandler);                        \
        int ret = 0;                                                                                \
        __try {                                                                                     \
            ret = __96A9695E_RUN_MAIN_FUNC(argc, argv);                                             \
        } __except (akl::WinExceptionCatchInternal::RecordExceptionInfo(GetExceptionInformation(), szDumpName)) {            \
            OutputDebugString(                                                                      \
                TEXT("Create a dump file since an exception occurred in "                           \
                     "main-thread.\n"));                                                            \
        }                                                                                           \
        return ret;                                                                                 \
    }                                                                                               \
    int __96A9695E_RUN_MAIN_FUNC(int argc, wchar_t* argv[]) {
#define MAIN_BEGIN(szDumpName)                                                                      \
    int __96A9695E_RUN_MAIN_FUNC(int argc, char* argv[]);                                           \
    LONG WINAPI __96A9695E_UnhandledExceptionHandler(_EXCEPTION_POINTERS* pExceptionInfo) {         \
        OutputDebugString(TEXT("Create a dump file since an exception occurred in sub-thread.\n")); \
        int iRet = akl::WinExceptionCatchInternal::RecordExceptionInfo(pExceptionInfo, szDumpName);                          \
        return iRet;                                                                                \
    }                                                                                               \
    int main(int argc, char* argv[]) {                                                              \
        ::SetUnhandledExceptionFilter(__96A9695E_UnhandledExceptionHandler);                        \
        int ret = 0;                                                                                \
        __try {                                                                                     \
            ret = __96A9695E_RUN_MAIN_FUNC(argc, argv);                                             \
        } __except (akl::WinExceptionCatchInternal::RecordExceptionInfo(GetExceptionInformation(), szDumpName)) {            \
            OutputDebugString(                                                                      \
                TEXT("Create a dump file since an exception occurred in "                           \
                     "main-thread.\n"));                                                            \
        }                                                                                           \
        return ret;                                                                                 \
    }                                                                                               \
    int __96A9695E_RUN_MAIN_FUNC(int argc, char* argv[]) {
#define WMAIN_END }
#define MAIN_END }

#endif

#endif  // !AKALI_WIN_MAIN_HPP_
