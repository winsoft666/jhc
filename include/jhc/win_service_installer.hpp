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

#ifndef JHC_WIN_SERVICE_INSTALLER_HPP_
#define JHC_WIN_SERVICE_INSTALLER_HPP_
#pragma once
#include "jhc/arch.hpp"
#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif
#include <stdio.h>
#include <strsafe.h>
#include "trace.hpp"

namespace jhc {
class WinServiceInstaller {
   public:
    //
    //   FUNCTION: InstallService
    //
    //   PURPOSE: Install the current application as a service to the local
    //   service control manager database.
    //
    //   PARAMETERS:
    //   * pszServiceName - the name of the service to be installed
    //   * pszDisplayName - the display name of the service
    //   * dwStartType - the service start option. This parameter can be one of
    //     the following values: SERVICE_AUTO_START, SERVICE_BOOT_START,
    //     SERVICE_DEMAND_START, SERVICE_DISABLED, SERVICE_SYSTEM_START.
    //   * pszDependencies - a pointer to a double null-terminated array of null-
    //     separated names of services or load ordering groups that the system
    //     must start before this service.
    //   * pszAccount - the name of the account under which the service runs.
    //   * pszPassword - the password to the account name.
    //
    //   NOTE: If the function fails to install the service, it prints the error
    //   in the standard output stream for users to diagnose the problem.
    //
    static void InstallService(PWSTR pszServiceName,
                               PWSTR pszDisplayName,
                               PWSTR pszDecription,
                               DWORD dwStartType,
                               PWSTR pszDependencies,
                               PWSTR pszAccount,
                               PWSTR pszPassword) {
        wchar_t szPath[MAX_PATH];
        SC_HANDLE schSCManager = NULL;
        SC_HANDLE schService = NULL;

        if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)) == 0) {
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
            CreateService(schSCManager,               // SCManager database
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
                SERVICE_DESCRIPTION ServiceDesc;
                ServiceDesc.lpDescription = pszDecription;
                ::ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &ServiceDesc);
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

    //
    //   FUNCTION: UninstallService
    //
    //   PURPOSE: Stop and remove the service from the local service control
    //   manager database.
    //
    //   PARAMETERS:
    //   * pszServiceName - the name of the service to be removed.
    //
    //   NOTE: If the function fails to uninstall the service, it prints the
    //   error in the standard output stream for users to diagnose the problem.
    //
    static void UninstallService(PWSTR pszServiceName) {
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
        schService = OpenService(schSCManager, pszServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
        if (schService == NULL) {
            Trace::MsgW(L"OpenService failed w/err 0x%08lx\n", GetLastError());
            goto Cleanup;
        }

        // Try to stop the service
        if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus)) {
            Trace::MsgW(L"Stopping %s.", pszServiceName);
            Sleep(1000);

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

    static void StartService(PWSTR pszServiceName) {
        if (NULL == pszServiceName)
            return;

        SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
        if (schSCManager == NULL)
            return;

        SC_HANDLE schService = OpenService(schSCManager, pszServiceName, SERVICE_ALL_ACCESS);
        if (schService == NULL)
            return;

        SERVICE_STATUS status;
        QueryServiceStatus(schService, &status);

        if (status.dwCurrentState == SERVICE_STOPPED)
            ::StartService(schService, 0, NULL);

        CloseServiceHandle(schSCManager);
        CloseServiceHandle(schService);
    }

    static void StopService(PWSTR pszServiceName) {
        if (NULL == pszServiceName)
            return;

        SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
        if (schSCManager == NULL)
            return;

        SC_HANDLE schService = OpenService(schSCManager, pszServiceName, SERVICE_ALL_ACCESS);
        if (schService == NULL)
            return;

        SERVICE_STATUS status;
        QueryServiceStatus(schService, &status);

        if (status.dwCurrentState == SERVICE_RUNNING)
            ControlService(schService, SERVICE_CONTROL_STOP, &status);

        CloseServiceHandle(schSCManager);
        CloseServiceHandle(schService);
    }
};
}  // namespace jhc
#endif
#endif