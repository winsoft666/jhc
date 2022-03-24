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

#ifndef JHC_WIN_SCHEDULE_TASK_HPP__
#define JHC_WIN_SCHEDULE_TASK_HPP__

#include "jhc/arch.hpp"

#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif
#include <tchar.h>
#include <Atlbase.h>
#include <comdef.h>
#include <taskschd.h>
#include <strsafe.h>
#include "jhc/macros.hpp"
#include "jhc/string_encode.hpp"

#pragma comment(lib, "taskschd.lib")

namespace jhc {
class ScheduleTask {
   public:
    JHC_DISALLOW_COPY_AND_ASSIGN(ScheduleTask);
    JHC_DISALLOW_MOVE_AND_ASSIGN(ScheduleTask);

    ScheduleTask() {
        m_lpITS = NULL;
        m_lpRootFolder = NULL;
        HRESULT hr = ::CoInitialize(NULL);
        assert(SUCCEEDED(hr));

        hr = ::CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService,
                                (LPVOID*)(&m_lpITS));
        if (SUCCEEDED(hr)) {
            hr = m_lpITS->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
            assert(SUCCEEDED(hr));

            hr = m_lpITS->GetFolder(_bstr_t(TEXT("\\")), &m_lpRootFolder);
            assert(SUCCEEDED(hr));
        }
    }

    virtual ~ScheduleTask() {
        if (m_lpITS) {
            m_lpITS->Release();
        }
        if (m_lpRootFolder) {
            m_lpRootFolder->Release();
        }
        ::CoUninitialize();
    }

    bool deleteTask(LPCTSTR pszTaskName) {
        if (NULL == m_lpRootFolder) {
            return false;
        }
        CComVariant variantTaskName(NULL);
        variantTaskName = pszTaskName;
        HRESULT hr = m_lpRootFolder->DeleteTask(variantTaskName.bstrVal, 0);
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool deleteFolder(LPCTSTR pszFolderName) {
        if (NULL == m_lpRootFolder) {
            return false;
        }
        CComVariant variantFolderName(NULL);
        variantFolderName = pszFolderName;
        HRESULT hr = m_lpRootFolder->DeleteFolder(variantFolderName.bstrVal, 0);
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    // ITrigger type is TASK_TRIGGER_LOGON
    // Action number is 1
    //
    bool createLoginTriggerTask(LPCTSTR pszTaskName,
                                LPCTSTR pszProgramPath,
                                LPCTSTR pszParameters,
                                LPCTSTR pszDescription,
                                LPCTSTR pszAuthor) {
        if (NULL == m_lpRootFolder) {
            return false;
        }

        delete (pszTaskName);

        ITaskDefinition* pTaskDefinition = NULL;
        HRESULT hr = m_lpITS->NewTask(0, &pTaskDefinition);
        if (FAILED(hr)) {
            return false;
        }

        IRegistrationInfo* pRegInfo = NULL;
        CComVariant variantAuthor(NULL);
        variantAuthor = pszAuthor;
        CComVariant variantDescription(NULL);
        variantDescription = pszDescription;
        hr = pTaskDefinition->get_RegistrationInfo(&pRegInfo);
        if (FAILED(hr)) {
            return false;
        }

        hr = pRegInfo->put_Author(variantAuthor.bstrVal);
        hr = pRegInfo->put_Description(variantDescription.bstrVal);
        pRegInfo->Release();

        IPrincipal* pPrincipal = NULL;
        hr = pTaskDefinition->get_Principal(&pPrincipal);
        if (FAILED(hr)) {
            return false;
        }

        hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
        hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
        pPrincipal->Release();

        ITaskSettings* pSettting = NULL;
        hr = pTaskDefinition->get_Settings(&pSettting);
        if (FAILED(hr)) {
            return false;
        }

        hr = pSettting->put_StopIfGoingOnBatteries(VARIANT_FALSE);
        hr = pSettting->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
        hr = pSettting->put_AllowDemandStart(VARIANT_TRUE);
        hr = pSettting->put_StartWhenAvailable(VARIANT_FALSE);
        hr = pSettting->put_MultipleInstances(TASK_INSTANCES_PARALLEL);
        pSettting->Release();

        IActionCollection* pActionCollect = NULL;
        hr = pTaskDefinition->get_Actions(&pActionCollect);
        if (FAILED(hr)) {
            return false;
        }
        IAction* pAction = NULL;

        hr = pActionCollect->Create(TASK_ACTION_EXEC, &pAction);
        pActionCollect->Release();

        CComVariant variantProgramPath(NULL);
        CComVariant variantParameters(NULL);
        IExecAction* pExecAction = NULL;
        hr = pAction->QueryInterface(IID_IExecAction, (PVOID*)(&pExecAction));
        if (FAILED(hr)) {
            pAction->Release();
            return false;
        }
        pAction->Release();

        variantProgramPath = pszProgramPath;
        variantParameters = pszParameters;
        pExecAction->put_Path(variantProgramPath.bstrVal);
        pExecAction->put_Arguments(variantParameters.bstrVal);
        pExecAction->Release();

        ITriggerCollection* pTriggers = NULL;
        hr = pTaskDefinition->get_Triggers(&pTriggers);
        if (FAILED(hr)) {
            return false;
        }

        ITrigger* pTrigger = NULL;
        hr = pTriggers->Create(TASK_TRIGGER_LOGON, &pTrigger);
        if (FAILED(hr)) {
            return false;
        }

        IRegisteredTask* pRegisteredTask = NULL;
        CComVariant variantTaskName(NULL);
        variantTaskName = pszTaskName;
        hr = m_lpRootFolder->RegisterTaskDefinition(
            variantTaskName.bstrVal, pTaskDefinition, TASK_CREATE_OR_UPDATE, _variant_t(), _variant_t(),
            TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(""), &pRegisteredTask);
        if (FAILED(hr)) {
            pTaskDefinition->Release();
            return false;
        }
        pTaskDefinition->Release();
        pRegisteredTask->Release();

        return true;
    }

    bool isExist(LPCTSTR pszTaskName) {
        if (NULL == m_lpRootFolder) {
            return false;
        }
        HRESULT hr = S_OK;
        CComVariant variantTaskName(NULL);
        CComVariant variantEnable(NULL);
        variantTaskName = pszTaskName;
        IRegisteredTask* pRegisteredTask = NULL;

        hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
        if (FAILED(hr) || (NULL == pRegisteredTask)) {
            return false;
        }
        pRegisteredTask->Release();

        return true;
    }

    bool isTaskValid(LPCTSTR pszTaskName) {
        if (NULL == m_lpRootFolder) {
            return false;
        }
        HRESULT hr = S_OK;
        CComVariant variantTaskName(NULL);
        CComVariant variantEnable(NULL);
        variantTaskName = pszTaskName;
        IRegisteredTask* pRegisteredTask = NULL;

        hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
        if (FAILED(hr) || (NULL == pRegisteredTask)) {
            return false;
        }

        TASK_STATE taskState;
        hr = pRegisteredTask->get_State(&taskState);
        if (FAILED(hr)) {
            pRegisteredTask->Release();
            return false;
        }
        pRegisteredTask->Release();

        if (TASK_STATE_DISABLED == taskState) {
            return false;
        }

        return true;
    }

    bool run(LPCTSTR pszTaskName, LPCTSTR pszParam) {
        if (NULL == m_lpRootFolder) {
            return false;
        }
        HRESULT hr = S_OK;
        CComVariant variantTaskName(NULL);
        CComVariant variantParameters(NULL);
        variantTaskName = pszTaskName;
        variantParameters = pszParam;

        IRegisteredTask* pRegisteredTask = NULL;
        hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
        if (FAILED(hr) || (NULL == pRegisteredTask)) {
            return false;
        }

        hr = pRegisteredTask->Run(variantParameters, NULL);
        if (FAILED(hr)) {
            pRegisteredTask->Release();
            return false;
        }
        pRegisteredTask->Release();

        return true;
    }

    bool isEnable(LPCTSTR pszTaskName) {
        if (NULL == m_lpRootFolder) {
            return false;
        }
        HRESULT hr = S_OK;
        CComVariant variantTaskName(NULL);
        CComVariant variantEnable(NULL);
        variantTaskName = pszTaskName;
        IRegisteredTask* pRegisteredTask = NULL;

        hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
        if (FAILED(hr) || (NULL == pRegisteredTask)) {
            return false;
        }

        pRegisteredTask->get_Enabled(&variantEnable.boolVal);
        pRegisteredTask->Release();
        if (ATL_VARIANT_FALSE == variantEnable.boolVal) {
            return false;
        }

        return true;
    }

    bool setEnable(LPCTSTR pszTaskName, bool bEnable) {
        if (NULL == m_lpRootFolder) {
            return false;
        }
        HRESULT hr = S_OK;
        CComVariant variantTaskName(NULL);
        CComVariant variantEnable(NULL);
        variantTaskName = pszTaskName;
        variantEnable = bEnable;
        IRegisteredTask* pRegisteredTask = NULL;

        hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
        if (FAILED(hr) || (NULL == pRegisteredTask)) {
            return false;
        }

        pRegisteredTask->put_Enabled(variantEnable.boolVal);
        pRegisteredTask->Release();

        return true;
    }

    bool getProgramPath(LPCTSTR pszTaskName, long lActionIndex, LPTSTR pszProgramPath) {
        if (NULL == m_lpRootFolder)
            return false;

        HRESULT hr = S_OK;
        CComVariant variantTaskName(NULL);
        variantTaskName = pszTaskName;
        IRegisteredTask* pRegisteredTask = NULL;
        ITaskDefinition* pTaskDefinition = NULL;
        IActionCollection* pActionColl = NULL;
        IAction* pAction = NULL;
        IExecAction* pExecAction = NULL;

        bool bRet = false;
        do {
            hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
            if (FAILED(hr) || (NULL == pRegisteredTask))
                break;

            hr = pRegisteredTask->get_Definition(&pTaskDefinition);
            if (FAILED(hr) || !pTaskDefinition)
                break;

            hr = pTaskDefinition->get_Actions(&pActionColl);
            if (FAILED(hr) || !pActionColl)
                break;

            hr = pActionColl->get_Item(lActionIndex, &pAction);
            if (FAILED(hr) || !pAction)
                break;

            hr = pAction->QueryInterface(IID_IExecAction, (PVOID*)(&pExecAction));
            if (FAILED(hr) || !pExecAction)
                break;

            CComVariant variantTaskStr(NULL);
            hr = pExecAction->get_Path(&variantTaskStr.bstrVal);
            if (FAILED(hr))
                break;

            StringCchCopy(pszProgramPath, MAX_PATH, UnicodeToTCHAR(variantTaskStr.bstrVal).c_str());
            bRet = true;
        } while (false);

        if (pExecAction)
            pExecAction->Release();

        if (pAction)
            pAction->Release();

        if (pActionColl)
            pActionColl->Release();

        if (pTaskDefinition)
            pTaskDefinition->Release();

        if (pRegisteredTask)
            pRegisteredTask->Release();

        return bRet;
    }

    bool getParameters(LPCTSTR pszTaskName, long lActionIndex, LPTSTR pszParameters) {
        if (NULL == m_lpRootFolder)
            return false;

        HRESULT hr = S_OK;
        CComVariant variantTaskName(NULL);
        variantTaskName = pszTaskName;
        IRegisteredTask* pRegisteredTask = NULL;
        ITaskDefinition* pTaskDefinition = NULL;
        IActionCollection* pActionColl = NULL;
        IAction* pAction = NULL;
        IExecAction* pExecAction = NULL;

        bool bRet = false;
        do {
            hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
            if (FAILED(hr) || (NULL == pRegisteredTask))
                break;

            hr = pRegisteredTask->get_Definition(&pTaskDefinition);
            if (FAILED(hr) || !pTaskDefinition)
                break;

            hr = pTaskDefinition->get_Actions(&pActionColl);
            if (FAILED(hr) || !pActionColl)
                break;

            hr = pActionColl->get_Item(lActionIndex, &pAction);
            if (FAILED(hr) || !pAction)
                break;

            hr = pAction->QueryInterface(IID_IExecAction, (PVOID*)(&pExecAction));
            if (FAILED(hr) || !pExecAction)
                break;

            CComVariant variantTaskStr(NULL);
            hr = pExecAction->get_Arguments(&variantTaskStr.bstrVal);
            if (FAILED(hr))
                break;
            StringCchCopy(pszParameters, MAX_PATH, UnicodeToTCHAR(variantTaskStr.bstrVal).c_str());
            bRet = true;
        } while (false);

        if (pExecAction)
            pExecAction->Release();

        if (pAction)
            pAction->Release();

        if (pActionColl)
            pActionColl->Release();

        if (pTaskDefinition)
            pTaskDefinition->Release();

        if (pRegisteredTask)
            pRegisteredTask->Release();

        return bRet;
    }

   protected:
    ITaskService* m_lpITS;
    ITaskFolder* m_lpRootFolder;
};
}  // namespace jhc
#endif
#endif  // !JHC_WIN_SCHEDULE_TASK_HPP__
