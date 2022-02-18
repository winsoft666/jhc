/*******************************************************************************
 * Copyright (C) 2021 - 2026, winsoft666, <winsoft666@outlook.com>.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Expect bugs
 *
 * Please use and enjoy. Please let me know of any bugs/improvements
 * that you have found/implemented and I will fix/incorporate them into this
 * file.
 *******************************************************************************/

#ifndef AKALI_TRACE_HPP__
#define AKALI_TRACE_HPP__
#pragma once

#include "akali_hpp/arch.hpp"
#include <memory>
#ifdef AKALI_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif
#include <strsafe.h>
#else
#include <string>
#include <stdarg.h>
#endif

namespace akali_hpp {
class Trace {
   public:
    static void TraceMsgW(const wchar_t* lpFormat, ...) {
        if (!lpFormat)
            return;

#ifdef AKALI_WIN
        wchar_t* pMsgBuffer = NULL;
        unsigned int iMsgBufCount = 0;

        va_list arglist;
        va_start(arglist, lpFormat);
        HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;

        while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
            iMsgBufCount += 1024;

            if (pMsgBuffer) {
                free(pMsgBuffer);
                pMsgBuffer = NULL;
            }

            pMsgBuffer = (wchar_t*)malloc(iMsgBufCount * sizeof(wchar_t));

            if (!pMsgBuffer) {
                break;
            }

            hr = StringCchVPrintfW(pMsgBuffer, iMsgBufCount, lpFormat, arglist);
        }

        va_end(arglist);

        if (hr == S_OK) {
            if (pMsgBuffer)
                OutputDebugStringW(pMsgBuffer);
        }

        if (pMsgBuffer) {
            free(pMsgBuffer);
            pMsgBuffer = NULL;
        }
#else
        wchar_t msgBuf[1024] = {0};
        va_list arglist;
        va_start(arglist, lpFormat);
        int err = vswprintf(msgBuf, 1024, lpFormat, arglist);
        va_end(arglist);
        printf("%ls\n", msgBuf);
#endif
    }

    static void TraceMsgA(const char* lpFormat, ...) {
        if (!lpFormat)
            return;

#ifdef AKALI_WIN
        char* pMsgBuffer = NULL;
        unsigned int iMsgBufCount = 0;

        va_list arglist;
        va_start(arglist, lpFormat);
        HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;

        while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
            iMsgBufCount += 1024;

            if (pMsgBuffer) {
                free(pMsgBuffer);
                pMsgBuffer = NULL;
            }

            pMsgBuffer = (char*)malloc(iMsgBufCount * sizeof(char));

            if (!pMsgBuffer) {
                break;
            }

            hr = StringCchVPrintfA(pMsgBuffer, iMsgBufCount, lpFormat, arglist);
        }

        va_end(arglist);

        if (hr == S_OK) {
            if (pMsgBuffer)
                OutputDebugStringA(pMsgBuffer);
        }

        if (pMsgBuffer) {
            free(pMsgBuffer);
            pMsgBuffer = NULL;
        }
#else
        char msgBuf[1024] = {0};
        va_list arglist;
        va_start(arglist, lpFormat);
        int err = vsnprintf(msgBuf, 1024, lpFormat, arglist);
        va_end(arglist);
        printf("%s\n", msgBuf);
#endif
    }
};
}  // namespace akali_hpp

#endif  // !AKALI_TRACE_H__