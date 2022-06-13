#include "jhc/config.hpp"
#ifdef JHC_NOT_HEADER_ONLY
#include "../trace.hpp"
#endif

#include "jhc/arch.hpp"
#include <memory>
#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif
#include <strsafe.h>
#else
#include <string>
#include <stdarg.h>
#endif
#include "jhc/string_helper.hpp"

namespace jhc {
JHC_INLINE void Trace::MsgW(const wchar_t* lpFormat, ...) {
    std::wstring output;
    va_list args;
    va_start(args, lpFormat);
    const bool ret = StringHelper::StringPrintfV(lpFormat, args, output);
    va_end(args);

    if (ret) {
#ifdef JHC_WIN
        OutputDebugStringW(output.c_str());
#else
        printf("%ls", output.c_str());
#endif
    }
}

JHC_INLINE void Trace::MsgA(const char* lpFormat, ...) {
    std::string output;
    va_list args;
    va_start(args, lpFormat);
    const bool ret = StringHelper::StringPrintfV(lpFormat, args, output);
    va_end(args);

    if (ret) {
#ifdef JHC_WIN
        OutputDebugStringA(output.c_str());
#else
        printf("%s", output.c_str());
#endif
    }
}
}  // namespace jhc