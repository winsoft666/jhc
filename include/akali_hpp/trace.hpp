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
#include "akali_hpp/string_helper.hpp"

namespace akali_hpp {
class Trace {
   public:
    static void MsgW(const wchar_t* lpFormat, ...) {
        std::wstring output;
        va_list args;
        va_start(args, lpFormat);
        const bool ret = StringHelper::StringPrintfV(lpFormat, args, output);
        va_end(args);

        if (ret) {
#ifdef AKALI_WIN
            OutputDebugStringW(output.c_str());
#else
            printf("%ls", output.c_str());
#endif
        }
    }

    static void MsgA(const char* lpFormat, ...) {
        std::string output;
        va_list args;
        va_start(args, lpFormat);
        const bool ret = StringHelper::StringPrintfV(lpFormat, args, output);
        va_end(args);

        if (ret) {
#ifdef AKALI_WIN
            OutputDebugStringA(output.c_str());
#else
            printf("%s", output.c_str());
#endif
        }
    }
};
}  // namespace akali_hpp

#endif  // !AKALI_TRACE_H__