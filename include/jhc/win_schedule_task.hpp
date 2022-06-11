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
#include "jhc/macros.hpp"

namespace jhc {
class ScheduleTask {
   public:
    JHC_DISALLOW_COPY_MOVE(ScheduleTask);

    ScheduleTask();

    virtual ~ScheduleTask();

    bool deleteTask(const wchar_t* pszTaskName);

    bool deleteFolder(const wchar_t* pszFolderName);

    // ITrigger type is TASK_TRIGGER_LOGON
    // Action number is 1
    //
    bool createLoginTriggerTask(const wchar_t* pszTaskName,
                                const wchar_t* pszProgramPath,
                                const wchar_t* pszParameters,
                                const wchar_t* pszDescription,
                                const wchar_t* pszAuthor);

    bool isExist(const wchar_t* pszTaskName);

    bool isTaskValid(const wchar_t* pszTaskName);

    bool run(const wchar_t* pszTaskName, const wchar_t* pszParam);

    bool isEnable(const wchar_t* pszTaskName);

    bool setEnable(const wchar_t* pszTaskName, bool bEnable);

    bool getProgramPath(const wchar_t* pszTaskName, long lActionIndex, wchar_t* pszProgramPath);

    bool getParameters(const wchar_t* pszTaskName, long lActionIndex, wchar_t* pszParameters);

   protected:
    class Private;
    Private* p_ = nullptr;
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/win_schedule_task.cc"
#endif
#endif
#endif  // !JHC_WIN_SCHEDULE_TASK_HPP__
