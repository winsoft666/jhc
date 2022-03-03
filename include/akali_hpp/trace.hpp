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
    static void MsgW(const wchar_t* lpFormat, ...) {
        if (!lpFormat)
            return;

#ifdef AKALI_WIN
        wchar_t* pMsgBuffer = NULL;
        size_t iMsgBufCount = 0;

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
        wchar_t* msgBuf = nullptr;
        size_t msgBufSize = 1024;
        va_list arglist;
        va_start(arglist, lpFormat);

        do {
            if (msgBuf) {
                free(msgBuf);
                msgBuf = nullptr;
            }
            msgBuf = (wchar_t*)malloc(msgBufSize * sizeof(wchar_t));
            memset(msgBuf, 0, msgBufSize * sizeof(wchar_t));

            const int err = vswprintf(msgBuf, msgBufSize, lpFormat, arglist);
            if (err >= 0 && err < msgBufSize)
                break;

            msgBufSize *= 2;
        } while (true);

        va_end(arglist);
        printf("%ls", msgBuf);

        if (msgBuf) {
            free(msgBuf);
            msgBuf = nullptr;
        }
#endif
    }

    static void MsgA(const char* lpFormat, ...) {
        if (!lpFormat)
            return;

#ifdef AKALI_WIN
        char* pMsgBuffer = NULL;
        size_t iMsgBufCount = 0;

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
        char* msgBuf = nullptr;
        size_t msgBufSize = 1024;
        va_list arglist;
        va_start(arglist, lpFormat);

        do {
            if (msgBuf) {
                free(msgBuf);
                msgBuf = nullptr;
            }

            msgBuf = (char*)malloc(msgBufSize * sizeof(char));
            memset(msgBuf, 0, msgBufSize * sizeof(char));

            const int err = vsnprintf(msgBuf, msgBufSize, lpFormat, arglist);
            if (err >= 0 && err < msgBufSize)
                break;

            msgBufSize *= 2;
        } while (true);

        va_end(arglist);
        printf("%s", msgBuf);

        if (msgBuf) {
            free(msgBuf);
            msgBuf = nullptr;
        }
#endif
    }
};
}  // namespace akali_hpp

#endif  // !AKALI_TRACE_H__