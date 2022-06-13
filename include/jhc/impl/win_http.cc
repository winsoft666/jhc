#include "jhc/config.hpp"
#ifdef JHC_NOT_HEADER_ONLY
#include "../win_http.hpp"
#endif

#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif  // !_INC_WINDOWS
#include <winhttp.h>
#pragma comment(lib, "winhttp")
#include "jhc/string_helper.hpp"

namespace jhc {
JHC_INLINE WinHttp::~WinHttp() {
    if (m_hRequest) {
        WinHttpCloseHandle(m_hRequest);
        m_hRequest = NULL;
    }

    if (m_hConnect) {
        WinHttpCloseHandle(m_hConnect);
        m_hConnect = NULL;
    }

    if (m_hSession) {
        WinHttpCloseHandle(m_hSession);
        m_hSession = NULL;
    }
}

JHC_INLINE bool WinHttp::openSession(
    const std::wstring& userAgent,
    int dwAccessType,
    const std::wstring& proxyName,
    const std::wstring& proxyBypass,
    int dwFlags) {
    if (dwAccessType == WINHTTP_ACCESS_TYPE_NO_PROXY) {
        m_hSession = WinHttpOpen(userAgent.c_str(), dwAccessType, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, dwFlags);
    }
    else {
        m_hSession = WinHttpOpen(userAgent.c_str(), dwAccessType, proxyName.c_str(), proxyBypass.c_str(), dwFlags);
    }

    return m_hSession;
}

JHC_INLINE bool WinHttp::openConnect(const std::wstring& url) {
    if (m_hSession) {
        if (WinHttpCheckPlatform()) {
            URL_COMPONENTS urlComp;
            ZeroMemory(&urlComp, sizeof(urlComp));
            urlComp.dwStructSize = sizeof(urlComp);

            WCHAR wszScheme[64] = {0};
            urlComp.lpszScheme = wszScheme;
            urlComp.dwSchemeLength = ARRAYSIZE(wszScheme);

            WCHAR wszHostName[1024] = {0};
            urlComp.lpszHostName = wszHostName;
            urlComp.dwHostNameLength = ARRAYSIZE(wszHostName);

            WCHAR wszUrlPath[1024] = {0};
            urlComp.lpszUrlPath = wszUrlPath;
            urlComp.dwUrlPathLength = ARRAYSIZE(wszUrlPath);

            WCHAR wszExtraInfo[1024] = {0};
            urlComp.lpszExtraInfo = wszExtraInfo;
            urlComp.dwExtraInfoLength = ARRAYSIZE(wszExtraInfo);

            if (WinHttpCrackUrl(url.c_str(), url.length(), ICU_ESCAPE, &urlComp)) {
                m_strHost = urlComp.lpszHostName;
                m_strPath = urlComp.lpszUrlPath;
                m_strExt = urlComp.lpszExtraInfo;
                m_nPort = urlComp.nPort;
                m_nScheme = urlComp.nScheme;
                m_hConnect = WinHttpConnect(m_hSession, m_strHost.c_str(), m_nPort, 0);
                return m_hConnect;
            }
        }
    }
    return false;
}

JHC_INLINE bool WinHttp::openRequest(bool bPost) {
    if (m_hConnect) {
        const wchar_t* pwszVerb = bPost ? L"POST" : L"GET";
        const DWORD dwFlags = (m_nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0);
        m_hRequest = WinHttpOpenRequest(m_hConnect,
                                        pwszVerb,
                                        m_strPath.c_str(),
                                        NULL,
                                        WINHTTP_NO_REFERER,
                                        WINHTTP_DEFAULT_ACCEPT_TYPES,
                                        dwFlags);

        return m_hRequest;
    }
    return true;
}

JHC_INLINE bool WinHttp::sendRequest(void* lpBuffer, unsigned int dwSize) {
    BOOL bResults = FALSE;
    if (m_hRequest) {
        if (lpBuffer != NULL && dwSize > 0)
            bResults = WinHttpSendRequest(m_hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, lpBuffer, dwSize, dwSize, 0);
        else
            bResults = WinHttpSendRequest(m_hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    }
    return !!bResults;
}

JHC_INLINE bool WinHttp::receiveResponse() {
    if (m_hRequest) {
        return WinHttpReceiveResponse(m_hRequest, NULL);
    }
    return false;
}

JHC_INLINE std::vector<unsigned char> WinHttp::getResponseBody() {
    DWORD dwSize = 0;
    std::vector<unsigned char> list;
    if (!m_hRequest)
        return list;

    do {
        // Check for available data.
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(m_hRequest, &dwSize)) {
            break;
        }

        // No more available data.
        if (!dwSize)
            break;

        // Allocate space for the buffer.
        BYTE* lpReceivedData = new BYTE[dwSize];
        if (!lpReceivedData) {
            break;
        }

        // Read the Data.
        ZeroMemory(lpReceivedData, dwSize);

        DWORD dwRead = 0;
        if (!WinHttpReadData(m_hRequest, lpReceivedData, dwSize, &dwRead)) {
            delete[] lpReceivedData;
            break;
        }

        for (size_t i = 0; i < dwSize; i++) {
            list.push_back(lpReceivedData[i]);
        }

        // Free the memory allocated to the buffer.
        delete[] lpReceivedData;
    } while (dwSize > 0);

    return list;
}

JHC_INLINE unsigned int WinHttp::getResponseStatusCode() {
    DWORD dwSize = sizeof(DWORD);
    DWORD dwStatusCode = 0;
    std::wstring strStatus;

    BOOL bResults = WinHttpQueryHeaders(
        m_hRequest,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX,
        &dwStatusCode,
        &dwSize,
        WINHTTP_NO_HEADER_INDEX);

    return dwStatusCode;
}

JHC_INLINE std::wstring WinHttp::getResponseStatusText() {
    DWORD dwSize = 0;
    WCHAR* lpOutBuffer = NULL;
    std::wstring strStatus;

    BOOL bResults = WinHttpQueryHeaders(m_hRequest, WINHTTP_QUERY_STATUS_TEXT, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwSize, WINHTTP_NO_HEADER_INDEX);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        lpOutBuffer = new WCHAR[dwSize / sizeof(WCHAR)];
        bResults = WinHttpQueryHeaders(m_hRequest, WINHTTP_QUERY_STATUS_TEXT, WINHTTP_HEADER_NAME_BY_INDEX, lpOutBuffer, &dwSize, WINHTTP_NO_HEADER_INDEX);
        if (bResults)
            strStatus.assign(lpOutBuffer, dwSize / sizeof(WCHAR));
        delete[] lpOutBuffer;
    }

    return strStatus;
}

JHC_INLINE std::wstring WinHttp::getResponseRawHeaders() {
    DWORD dwSize = 0;
    WCHAR* lpOutBuffer = NULL;
    std::wstring strResult;

    BOOL bResults = WinHttpQueryHeaders(m_hRequest,
                                        WINHTTP_QUERY_RAW_HEADERS_CRLF,
                                        WINHTTP_HEADER_NAME_BY_INDEX,
                                        NULL,
                                        &dwSize,
                                        WINHTTP_NO_HEADER_INDEX);

    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        lpOutBuffer = new WCHAR[dwSize / sizeof(WCHAR)];
        bResults = WinHttpQueryHeaders(m_hRequest,
                                       WINHTTP_QUERY_RAW_HEADERS_CRLF,
                                       WINHTTP_HEADER_NAME_BY_INDEX,
                                       lpOutBuffer,
                                       &dwSize,
                                       WINHTTP_NO_HEADER_INDEX);
        if (bResults)
            strResult.assign(lpOutBuffer, dwSize / sizeof(WCHAR));
        delete[] lpOutBuffer;
    }
    return strResult;
}

JHC_INLINE std::unordered_map<std::wstring, std::wstring> WinHttp::getResponseHeaders() {
    std::unordered_map<std::wstring, std::wstring> result;

    const std::wstring strHeaders = getResponseRawHeaders();
    std::vector<std::wstring> headers = StringHelper::Split(strHeaders, L"\r\n", false);
    for (const auto& item : headers) {
        std::vector<std::wstring> v = StringHelper::Split(item, L": ", false);
        if (v.size() == 2) {
            result[v[0]] = v[1];
        }
    }
    return result;
}

JHC_INLINE bool WinHttp::setRequestHeader(const std::wstring& name, const std::wstring& value) {
    if (name.empty() || value.empty())
        return false;

    BOOL bResults = FALSE;
    if (m_hRequest) {
        const std::wstring strHeader(name + L": " + value);
        bResults = WinHttpAddRequestHeaders(m_hRequest,
                                            strHeader.c_str(),
                                            (ULONG)-1L,
                                            WINHTTP_ADDREQ_FLAG_ADD_IF_NEW | WINHTTP_ADDREQ_FLAG_REPLACE);
    }
    return !!bResults;
}

JHC_INLINE bool WinHttp::setTimeout(
    int nResolveTimeout,
    int nConnectTimeout,
    int nSendTimeout,
    int nReceiveTimeout) {
    if (m_hSession)
        return WinHttpSetTimeouts(m_hSession, nResolveTimeout, nConnectTimeout, nSendTimeout, nReceiveTimeout);
    return false;
}

JHC_INLINE bool WinHttp::setOption(int Option, int value) {
    if (m_hRequest)
        return WinHttpSetOption(m_hRequest, Option, (LPVOID)&value, 4);
    return false;
}

JHC_INLINE bool WinHttp::setAllowRedirect(bool bAllow) {
    return setOption(WINHTTP_OPTION_REDIRECT_POLICY,
                     bAllow ? WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS : WINHTTP_OPTION_REDIRECT_POLICY_NEVER);
}
}  // namespace jhc
#endif