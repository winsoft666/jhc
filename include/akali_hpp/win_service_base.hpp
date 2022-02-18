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

#ifndef AKALI_WIN_SERVICE_BASE_HPP_
#define AKALI_WIN_SERVICE_BASE_HPP_
#pragma once

#include "akali_hpp/arch.hpp"

#ifdef AKALI_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif
#include <assert.h>
#include <strsafe.h>
#include "akali_hpp/stringencode.hpp"

namespace akali_hpp {
class WinServiceBase {
   public:
    // Register the executable for a service with the Service Control Manager
    // (SCM). After you call Run(ServiceBase), the SCM issues a Start command,
    // which results in a call to the OnStart method in the service. This
    // method blocks until the service has stopped.
    static BOOL Run(WinServiceBase& service) {
        s_service = &service;

        SERVICE_TABLE_ENTRY serviceTable[] = {{service.m_name, ServiceMain}, {NULL, NULL}};

        // Connects the main thread of a service process to the service control
        // manager, which causes the thread to be the service control dispatcher
        // thread for the calling process. This call returns when the service has
        // stopped. The process should simply terminate when the call returns.
        return StartServiceCtrlDispatcher(serviceTable);
    }

    // Service object constructor. The optional parameters (fCanStop,
    // fCanShutdown and fCanPauseContinue) allow you to specify whether the
    // service can be stopped, paused and continued, or be notified when
    // system shutdown occurs.
    WinServiceBase(LPCWSTR pszServiceName,
                   BOOL fCanStop = TRUE,
                   BOOL fCanShutdown = TRUE,
                   BOOL fCanPauseContinue = FALSE) {
        memset(m_name, 0, MAX_PATH * sizeof(WCHAR));
        if (pszServiceName)
            StringCchCopyW(m_name, MAX_PATH, pszServiceName);

        m_statusHandle = NULL;

        // The service runs in its own process.
        m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

        // The service is starting.
        m_status.dwCurrentState = SERVICE_START_PENDING;

        // The accepted commands of the service.
        DWORD dwControlsAccepted = 0;
        if (fCanStop)
            dwControlsAccepted |= SERVICE_ACCEPT_STOP;
        if (fCanShutdown)
            dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
        if (fCanPauseContinue)
            dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
        m_status.dwControlsAccepted = dwControlsAccepted;

        m_status.dwWin32ExitCode = NO_ERROR;
        m_status.dwServiceSpecificExitCode = 0;
        m_status.dwCheckPoint = 0;
        m_status.dwWaitHint = 0;
    }

    // Service object destructor.
    virtual ~WinServiceBase() {}

    // Stop the service.
    void Stop() {
        DWORD dwOriginalState = m_status.dwCurrentState;
        try {
            // Tell SCM that the service is stopping.
            SetServiceStatus(SERVICE_STOP_PENDING);

            // Perform service-specific stop operations.
            OnStop();

            // Tell SCM that the service is stopped.
            SetServiceStatus(SERVICE_STOPPED);
        } catch (DWORD dwError) {
            // Log the error.
            WriteErrorLogEntry(L"Service Stop", dwError);

            // Set the orginal service status.
            SetServiceStatus(dwOriginalState);
        } catch (...) {
            // Log the error.
            WriteEventLogEntry(L"Service failed to stop.", EVENTLOG_ERROR_TYPE);

            // Set the orginal service status.
            SetServiceStatus(dwOriginalState);
        }
    }

    // Log a message to the Application event log.
    // EVENTLOG_ERROR_TYPE
    // EVENTLOG_WARNING_TYPE
    // EVENTLOG_INFORMATION_TYPE
    //
    static void EventLogTraceW(LPCWSTR pszServiceName, WORD wType, LPCWSTR lpFormat, ...) {
        if (!lpFormat)
            return;

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
            if (pMsgBuffer) {
                HANDLE hEventSource = NULL;
                LPCWSTR lpszStrings[2] = {NULL, NULL};

                hEventSource = RegisterEventSource(NULL, pszServiceName);
                if (hEventSource) {
                    lpszStrings[0] = pszServiceName;
                    lpszStrings[1] = pMsgBuffer;

                    ReportEvent(hEventSource,  // Event log handle
                                wType,         // Event type
                                0,             // Event category
                                0,             // Event identifier
                                NULL,          // No security identifier
                                2,             // Size of lpszStrings array
                                0,             // No binary data
                                lpszStrings,   // Array of strings
                                NULL           // No binary data
                    );

                    DeregisterEventSource(hEventSource);
                }
            }
        }

        if (pMsgBuffer) {
            free(pMsgBuffer);
            pMsgBuffer = NULL;
        }
    }

    static void EventLogTraceA(LPCSTR pszServiceName, WORD wType, LPCSTR lpFormat, ...) {
        if (!lpFormat)
            return;

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
            if (pMsgBuffer) {
                HANDLE hEventSource = NULL;
                LPCSTR lpszStrings[2] = {NULL, NULL};

                hEventSource = RegisterEventSourceA(NULL, pszServiceName);
                if (hEventSource) {
                    lpszStrings[0] = pszServiceName;
                    lpszStrings[1] = pMsgBuffer;

                    ReportEventA(hEventSource,  // Event log handle
                                 wType,         // Event type
                                 0,             // Event category
                                 0,             // Event identifier
                                 NULL,          // No security identifier
                                 2,             // Size of lpszStrings array
                                 0,             // No binary data
                                 lpszStrings,   // Array of strings
                                 NULL           // No binary data
                    );

                    DeregisterEventSource(hEventSource);
                }
            }
        }

        if (pMsgBuffer) {
            free(pMsgBuffer);
            pMsgBuffer = NULL;
        }
    }

   protected:
    // When implemented in a derived class, executes when a Start command is
    // sent to the service by the SCM or when the operating system starts
    // (for a service that starts automatically). Specifies actions to take
    // when the service starts.
    virtual void OnStart(DWORD dwArgc, PWSTR* pszArgv) {}

    // When implemented in a derived class, executes when a Stop command is
    // sent to the service by the SCM. Specifies actions to take when a
    // service stops running.
    virtual void OnStop() {}

    // When implemented in a derived class, executes when a Pause command is
    // sent to the service by the SCM. Specifies actions to take when a
    // service pauses.
    virtual void OnPause() {}

    // When implemented in a derived class, OnContinue runs when a Continue
    // command is sent to the service by the SCM. Specifies actions to take
    // when a service resumes normal functioning after being paused.
    virtual void OnContinue() {}

    // When implemented in a derived class, executes when the system is
    // shutting down. Specifies what should occur immediately prior to the
    // system shutting down.
    virtual void OnShutdown() {}

    // Set the service status and report the status to the SCM.
    void SetServiceStatus(DWORD dwCurrentState,
                          DWORD dwWin32ExitCode = NO_ERROR,
                          DWORD dwWaitHint = 0) {
        static DWORD dwCheckPoint = 1;

        // Fill in the SERVICE_STATUS structure of the service.

        m_status.dwCurrentState = dwCurrentState;
        m_status.dwWin32ExitCode = dwWin32ExitCode;
        m_status.dwWaitHint = dwWaitHint;

        m_status.dwCheckPoint =
            ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
                ? 0
                : dwCheckPoint++;

        // Report the status of the service to the SCM.
        ::SetServiceStatus(m_statusHandle, &m_status);
    }

    // Log a message to the Application event log.
    void WriteEventLogEntry(LPCWSTR pszMessage, WORD wType) {
        HANDLE hEventSource = NULL;
        LPCWSTR lpszStrings[2] = {NULL, NULL};

        hEventSource = RegisterEventSource(NULL, m_name);
        if (hEventSource) {
            lpszStrings[0] = m_name;
            lpszStrings[1] = pszMessage;

            ReportEvent(hEventSource,  // Event log handle
                        wType,         // Event type
                        0,             // Event category
                        0,             // Event identifier
                        NULL,          // No security identifier
                        2,             // Size of lpszStrings array
                        0,             // No binary data
                        lpszStrings,   // Array of strings
                        NULL           // No binary data
            );

            DeregisterEventSource(hEventSource);
        }
    }

    void WriteEventLogEntryA(LPCSTR pszMessage, WORD wType) {
        std::wstring strMessageW = StringEncode::AnsiToUnicode(pszMessage);
        WriteEventLogEntry((LPCWSTR)strMessageW.c_str(), wType);
    }

    // Log an error message to the Application event log.
    void WriteErrorLogEntry(LPCWSTR pszFunction, DWORD dwError = GetLastError()) {
        wchar_t szMessage[260];
        StringCchPrintf(szMessage, ARRAYSIZE(szMessage), L"%s failed w/err 0x%08lx", pszFunction,
                        dwError);
        WriteEventLogEntry(szMessage, EVENTLOG_ERROR_TYPE);
    }

   private:
    // Entry point for the service. It registers the handler function for the
    // service and starts the service.
    static void WINAPI ServiceMain(DWORD dwArgc, LPWSTR* lpszArgv) {
        assert(s_service != NULL);

        // Register the handler function for the service
        s_service->m_statusHandle = RegisterServiceCtrlHandler(s_service->m_name, ServiceCtrlHandler);
        if (s_service->m_statusHandle == NULL) {
            throw GetLastError();
        }

        // Start the service.
        s_service->Start(dwArgc, lpszArgv);
    }

    // The function is called by the SCM whenever a control code is sent to
    // the service.
    static void WINAPI ServiceCtrlHandler(DWORD dwCtrl) {
        switch (dwCtrl) {
            case SERVICE_CONTROL_STOP:
                s_service->Stop();
                break;
            case SERVICE_CONTROL_PAUSE:
                s_service->Pause();
                break;
            case SERVICE_CONTROL_CONTINUE:
                s_service->Continue();
                break;
            case SERVICE_CONTROL_SHUTDOWN:
                s_service->Shutdown();
                break;
            case SERVICE_CONTROL_INTERROGATE:
                break;
            default:
                break;
        }
    }

    // Start the service.
    void Start(DWORD dwArgc, PWSTR* pszArgv) {
        try {
            // Tell SCM that the service is starting.
            SetServiceStatus(SERVICE_START_PENDING);

            // Perform service-specific initialization.
            OnStart(dwArgc, pszArgv);

            // Tell SCM that the service is started.
            SetServiceStatus(SERVICE_RUNNING);
        } catch (DWORD dwError) {
            // Log the error.
            WriteErrorLogEntry(L"Service Start", dwError);

            // Set the service status to be stopped.
            SetServiceStatus(SERVICE_STOPPED, dwError);
        } catch (...) {
            // Log the error.
            WriteEventLogEntry(L"Service failed to start.", EVENTLOG_ERROR_TYPE);

            // Set the service status to be stopped.
            SetServiceStatus(SERVICE_STOPPED);
        }
    }

    // Pause the service.
    void Pause() {
        try {
            // Tell SCM that the service is pausing.
            SetServiceStatus(SERVICE_PAUSE_PENDING);

            // Perform service-specific pause operations.
            OnPause();

            // Tell SCM that the service is paused.
            SetServiceStatus(SERVICE_PAUSED);
        } catch (DWORD dwError) {
            // Log the error.
            WriteErrorLogEntry(L"Service Pause", dwError);

            // Tell SCM that the service is still running.
            SetServiceStatus(SERVICE_RUNNING);
        } catch (...) {
            // Log the error.
            WriteEventLogEntry(L"Service failed to pause.", EVENTLOG_ERROR_TYPE);

            // Tell SCM that the service is still running.
            SetServiceStatus(SERVICE_RUNNING);
        }
    }

    // Resume the service after being paused.
    void Continue() {
        try {
            // Tell SCM that the service is resuming.
            SetServiceStatus(SERVICE_CONTINUE_PENDING);

            // Perform service-specific continue operations.
            OnContinue();

            // Tell SCM that the service is running.
            SetServiceStatus(SERVICE_RUNNING);
        } catch (DWORD dwError) {
            // Log the error.
            WriteErrorLogEntry(L"Service Continue", dwError);

            // Tell SCM that the service is still paused.
            SetServiceStatus(SERVICE_PAUSED);
        } catch (...) {
            // Log the error.
            WriteEventLogEntry(L"Service failed to resume.", EVENTLOG_ERROR_TYPE);

            // Tell SCM that the service is still paused.
            SetServiceStatus(SERVICE_PAUSED);
        }
    }

    // Execute when the system is shutting down.
    void Shutdown() {
        try {
            // Perform service-specific shutdown operations.
            OnShutdown();

            // Tell SCM that the service is stopped.
            SetServiceStatus(SERVICE_STOPPED);
        } catch (DWORD dwError) {
            // Log the error.
            WriteErrorLogEntry(L"Service Shutdown", dwError);
        } catch (...) {
            // Log the error.
            WriteEventLogEntry(L"Service failed to shut down.", EVENTLOG_ERROR_TYPE);
        }
    }

    // The singleton service instance.
    static WinServiceBase* s_service;

    // The name of the service
    WCHAR m_name[MAX_PATH];

    // The status of the service
    SERVICE_STATUS m_status;

    // The service status handle
    SERVICE_STATUS_HANDLE m_statusHandle;
};

WinServiceBase* WinServiceBase::s_service = NULL;

}  // namespace akali_hpp
#endif

#endif  // ! AKALI_WIN_SERVICE_BASE_H_
