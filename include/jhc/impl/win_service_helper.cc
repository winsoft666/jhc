#ifdef JHC_NOT_HEADER_ONLY
#include "../win_service_helper.hpp"
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
#endif
#include <stdio.h>
#include <strsafe.h>
#include "jhc/trace.hpp"

namespace jhc {
void WinServiceHelper::Install(const wchar_t* pszServiceName,
                               const wchar_t* pszDisplayName,
                               const wchar_t* pszDecription,
                               unsigned int dwStartType,
                               const wchar_t* pszDependencies,
                               const wchar_t* pszAccount,
                               const wchar_t* pszPassword) {
    wchar_t szPath[MAX_PATH] = {0};
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;

    if (GetModuleFileNameW(NULL, szPath, ARRAYSIZE(szPath)) == 0) {
        Trace::MsgW(L"GetModuleFileName failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
    if (schSCManager == NULL) {
        Trace::MsgW(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Install the service into SCM by calling CreateService
    schService =
        CreateServiceW(schSCManager,               // SCManager database
                       pszServiceName,             // Name of service
                       pszDisplayName,             // Name to display
                       SERVICE_ALL_ACCESS,         //SERVICE_QUERY_STATUS,           // Desired access
                       SERVICE_WIN32_OWN_PROCESS,  // Service type
                       dwStartType,                // Service start type
                       SERVICE_ERROR_NORMAL,       // Error control type
                       szPath,                     // Service's binary
                       NULL,                       // No load ordering group
                       NULL,                       // No tag identifier
                       NULL,                       //pszDependencies,                // Dependencies
                       NULL,                       //pszAccount,                     // Service running account
                       NULL                        //pszPassword                     // Password of the account
        );
    if (schService == NULL) {
        Trace::MsgW(L"CreateService failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }
    else {
        // Set service description
        if (pszDecription) {
            wchar_t szDesc[MAX_PATH] = {0};
            StringCchCopyW(szDesc, MAX_PATH, pszDecription);

            SERVICE_DESCRIPTION ServiceDesc;
            ServiceDesc.lpDescription = szDesc;
            ::ChangeServiceConfig2W(schService, SERVICE_CONFIG_DESCRIPTION, &ServiceDesc);
        }
    }

    Trace::MsgW(L"%s is installed.\n", pszServiceName);

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (schSCManager) {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService) {
        CloseServiceHandle(schService);
        schService = NULL;
    }
}

void WinServiceHelper::Uninstall(const wchar_t* pszServiceName) {
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS ssSvcStatus = {};

    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL) {
        Trace::MsgW(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Open the service with delete, stop, and query status permissions
    schService = OpenServiceW(schSCManager, pszServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
    if (schService == NULL) {
        Trace::MsgW(L"OpenService failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Try to stop the service
    if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus)) {
        Trace::MsgW(L"Stopping %s.", pszServiceName);
        Sleep(100);

        while (QueryServiceStatus(schService, &ssSvcStatus)) {
            if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING) {
                Trace::MsgW(L".");
                Sleep(1000);
            }
            else
                break;
        }

        if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED) {
            Trace::MsgW(L"\n%s is stopped.\n", pszServiceName);
        }
        else {
            Trace::MsgW(L"\n%s failed to stop.\n", pszServiceName);
        }
    }

    // Now remove the service by calling DeleteService.
    if (!DeleteService(schService)) {
        Trace::MsgW(L"DeleteService failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    Trace::MsgW(L"%s is removed.\n", pszServiceName);

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (schSCManager) {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService) {
        CloseServiceHandle(schService);
        schService = NULL;
    }
}

void WinServiceHelper::Start(const wchar_t* pszServiceName) {
    if (NULL == pszServiceName)
        return;

    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
    if (schSCManager == NULL)
        return;

    SC_HANDLE schService = OpenServiceW(schSCManager, pszServiceName, SERVICE_ALL_ACCESS);
    if (schService == NULL)
        return;

    SERVICE_STATUS status;
    QueryServiceStatus(schService, &status);

    if (status.dwCurrentState == SERVICE_STOPPED)
        ::StartServiceW(schService, 0, NULL);

    CloseServiceHandle(schSCManager);
    CloseServiceHandle(schService);
}

void WinServiceHelper::Stop(const wchar_t* pszServiceName) {
    if (NULL == pszServiceName)
        return;

    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
    if (schSCManager == NULL)
        return;

    SC_HANDLE schService = OpenServiceW(schSCManager, pszServiceName, SERVICE_ALL_ACCESS);
    if (schService == NULL)
        return;

    SERVICE_STATUS status;
    QueryServiceStatus(schService, &status);

    if (status.dwCurrentState == SERVICE_RUNNING)
        ControlService(schService, SERVICE_CONTROL_STOP, &status);

    CloseServiceHandle(schSCManager);
    CloseServiceHandle(schService);
}
}  // namespace jhc
#endif