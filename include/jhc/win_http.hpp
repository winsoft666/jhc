/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022 JiangXueqiao <winsoft666@outlook.com>.
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

#ifndef JHC_WIN_HTTP_HPP__
#define JHC_WIN_HTTP_HPP__

#include "jhc/arch.hpp"
#ifdef JHC_WIN
#include <string>
#include <vector>
#include <unordered_map>
#include "jhc/macros.hpp"

namespace jhc {
class WinHttp {
   public:
    JHC_DISALLOW_COPY_MOVE(WinHttp);

    WinHttp() = default;

    virtual ~WinHttp();

    // AccessType defined in winhttp.h
    // #define WINHTTP_ACCESS_TYPE_DEFAULT_PROXY 0
    // #define WINHTTP_ACCESS_TYPE_NO_PROXY 1
    // #define WINHTTP_ACCESS_TYPE_NAMED_PROXY 3
    // #define WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY 4
    bool openSession(
        const std::wstring& userAgent = L"",
        int dwAccessType = 1,
        const std::wstring& proxyName = L"",
        const std::wstring& proxyBypass = L"",
        int dwFlags = 0);

    bool openConnect(const std::wstring& url);

    bool openRequest(bool bPost = false);

    bool sendRequest(void* lpBuffer = NULL, unsigned int dwSize = 0);

    bool receiveResponse();

    std::vector<unsigned char> getResponseBody();

    unsigned int getResponseStatusCode();

    std::wstring getResponseStatusText();

    std::wstring getResponseRawHeaders();

    std::unordered_map<std::wstring, std::wstring> getResponseHeaders();

    bool setRequestHeader(const std::wstring& name, const std::wstring& value);

    bool setTimeout(
        int nResolveTimeout = 3000,
        int nConnectTimeout = 3000,
        int nSendTimeout = 3000,
        int nReceiveTimeout = 3000);

    bool setOption(int Option, int value);

    bool setAllowRedirect(bool bAllow);

   protected:
    void* m_hSession = NULL;
    void* m_hConnect = NULL;
    void* m_hRequest = NULL;

    std::wstring m_strHost;
    std::wstring m_strPath;
    std::wstring m_strExt;
    int m_nScheme = 0;  // INTERNET_SCHEME
    unsigned int m_nPort = 0;
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/win_http.cc"
#endif

#endif  // JHC_WIN
#endif
