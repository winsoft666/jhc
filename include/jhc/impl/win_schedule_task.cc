#include "jhc/config.hpp"
#ifdef JHC_NOT_HEADER_ONLY
#include "../win_schedule_task.hpp"
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
#include <tchar.h>
#include <Atlbase.h>
#include <comdef.h>
#include <taskschd.h>
#include <strsafe.h>
#include "jhc/macros.hpp"
#include "jhc/string_encode.hpp"

#pragma comment(lib, "taskschd.lib")

namespace jhc {
class ScheduleTask::Private {
   public:
    ITaskService* m_lpITS = NULL;
    ITaskFolder* m_lpRootFolder = NULL;
};

JHC_INLINE ScheduleTask::ScheduleTask() :
    p_(new ScheduleTask::Private()) {
    HRESULT hr = ::CoInitialize(NULL);
    assert(SUCCEEDED(hr));

    hr = ::CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService,
                            (LPVOID*)(&p_->m_lpITS));
    if (SUCCEEDED(hr)) {
        hr = p_->m_lpITS->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
        assert(SUCCEEDED(hr));

        hr = p_->m_lpITS->GetFolder(_bstr_t(TEXT("\\")), &p_->m_lpRootFolder);
        assert(SUCCEEDED(hr));
    }
}

JHC_INLINE ScheduleTask::~ScheduleTask() {
    if (p_->m_lpITS) {
        p_->m_lpITS->Release();
    }
    if (p_->m_lpRootFolder) {
        p_->m_lpRootFolder->Release();
    }
    ::CoUninitialize();

    delete p_;
    p_ = nullptr;
}

JHC_INLINE bool ScheduleTask::deleteTask(const wchar_t* pszTaskName) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    CComVariant variantTaskName(NULL);
    variantTaskName = pszTaskName;
    HRESULT hr = p_->m_lpRootFolder->DeleteTask(variantTaskName.bstrVal, 0);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}

JHC_INLINE bool ScheduleTask::deleteFolder(const wchar_t* pszFolderName) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    CComVariant variantFolderName(NULL);
    variantFolderName = pszFolderName;
    HRESULT hr = p_->m_lpRootFolder->DeleteFolder(variantFolderName.bstrVal, 0);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}

// ITrigger type is TASK_TRIGGER_LOGON
// Action number is 1
//
JHC_INLINE bool ScheduleTask::createLoginTriggerTask(const wchar_t* pszTaskName,
                                                     const wchar_t* pszProgramPath,
                                                     const wchar_t* pszParameters,
                                                     const wchar_t* pszDescription,
                                                     const wchar_t* pszAuthor) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }

    deleteTask(pszTaskName);

    ITaskDefinition* pTaskDefinition = NULL;
    HRESULT hr = p_->m_lpITS->NewTask(0, &pTaskDefinition);
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
    hr = p_->m_lpRootFolder->RegisterTaskDefinition(
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

JHC_INLINE bool ScheduleTask::isExist(const wchar_t* pszTaskName) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    HRESULT hr = S_OK;
    CComVariant variantTaskName(NULL);
    CComVariant variantEnable(NULL);
    variantTaskName = pszTaskName;
    IRegisteredTask* pRegisteredTask = NULL;

    hr = p_->m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
    if (FAILED(hr) || (NULL == pRegisteredTask)) {
        return false;
    }
    pRegisteredTask->Release();

    return true;
}

JHC_INLINE bool ScheduleTask::isTaskValid(const wchar_t* pszTaskName) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    HRESULT hr = S_OK;
    CComVariant variantTaskName(NULL);
    CComVariant variantEnable(NULL);
    variantTaskName = pszTaskName;
    IRegisteredTask* pRegisteredTask = NULL;

    hr = p_->m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
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

JHC_INLINE bool ScheduleTask::run(const wchar_t* pszTaskName, const wchar_t* pszParam) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    HRESULT hr = S_OK;
    CComVariant variantTaskName(NULL);
    CComVariant variantParameters(NULL);
    variantTaskName = pszTaskName;
    variantParameters = pszParam;

    IRegisteredTask* pRegisteredTask = NULL;
    hr = p_->m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
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

JHC_INLINE bool ScheduleTask::isEnable(const wchar_t* pszTaskName) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    HRESULT hr = S_OK;
    CComVariant variantTaskName(NULL);
    CComVariant variantEnable(NULL);
    variantTaskName = pszTaskName;
    IRegisteredTask* pRegisteredTask = NULL;

    hr = p_->m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
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

JHC_INLINE bool ScheduleTask::setEnable(const wchar_t* pszTaskName, bool bEnable) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    HRESULT hr = S_OK;
    CComVariant variantTaskName(NULL);
    CComVariant variantEnable(NULL);
    variantTaskName = pszTaskName;
    variantEnable = bEnable;
    IRegisteredTask* pRegisteredTask = NULL;

    hr = p_->m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
    if (FAILED(hr) || (NULL == pRegisteredTask)) {
        return false;
    }

    pRegisteredTask->put_Enabled(variantEnable.boolVal);
    pRegisteredTask->Release();

    return true;
}

JHC_INLINE bool ScheduleTask::getProgramPath(const wchar_t* pszTaskName, long lActionIndex, wchar_t* pszProgramPath) {
    if (NULL == p_->m_lpRootFolder)
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
        hr = p_->m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
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

        StringCchCopyW(pszProgramPath, MAX_PATH, variantTaskStr.bstrVal);
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

JHC_INLINE bool ScheduleTask::getParameters(const wchar_t* pszTaskName, long lActionIndex, wchar_t* pszParameters) {
    if (NULL == p_->m_lpRootFolder)
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
        hr = p_->m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
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

        StringCchCopyW(pszParameters, MAX_PATH, variantTaskStr.bstrVal);
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
}  // namespace jhc
#endif