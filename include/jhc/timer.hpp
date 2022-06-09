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

#ifndef JHC_TIMER_HPP_
#define JHC_TIMER_HPP_

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
#include <functional>

namespace jhc {
class TimerBase {
   public:
    TimerBase() {
        m_hTimer = NULL;
        m_pTimer = NULL;
    }

    virtual ~TimerBase() = default;

    static void CALLBACK TimerProc(void* param, BOOLEAN timerCalled) {
        UNREFERENCED_PARAMETER(timerCalled);
        TimerBase* timer = static_cast<TimerBase*>(param);

        if (timer)
            timer->onTimedEvent();
    }

    // About dwFlags, see:
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682485(v=vs.85).aspx
    //
    bool start(DWORD ulInterval,  // ulInterval in ms
               BOOL bImmediately,
               BOOL bOnce,
               ULONG dwFlags = WT_EXECUTELONGFUNCTION) {
        bool bRet = false;

        if (!m_hTimer) {
            bRet = !!CreateTimerQueueTimer(&m_hTimer, NULL, TimerProc, (PVOID)this,
                                           bImmediately ? 0 : ulInterval, bOnce ? 0 : ulInterval, dwFlags);
        }

        return bRet;
    }

    void stop(bool bWait) {
        if (m_hTimer) {
            BOOL b = DeleteTimerQueueTimer(NULL, m_hTimer, bWait ? INVALID_HANDLE_VALUE : NULL);
            assert(b);
            m_hTimer = NULL;
        }
    }

    virtual void onTimedEvent() {}

   private:
    HANDLE m_hTimer;
    PTP_TIMER m_pTimer;
};

template <class T>
class TTimer : public TimerBase {
   public:
    typedef void (T::*POnTimer)(void);

    TTimer() {
        m_pClass = NULL;
        m_pfnOnTimer = NULL;
    }

    void setTimedEvent(T* pClass, POnTimer pFunc) {
        m_pClass = pClass;
        m_pfnOnTimer = pFunc;
    }

   protected:
    void onTimedEvent() override {
        if (m_pfnOnTimer && m_pClass) {
            (m_pClass->*m_pfnOnTimer)();
        }
    }

   private:
    T* m_pClass;
    POnTimer m_pfnOnTimer;
};

class Timer : public TimerBase {
   public:
    typedef std::function<void()> FN_CB;
    Timer() {}

    Timer(FN_CB cb) { setTimedEvent(cb); }

    void setTimedEvent(FN_CB cb) { m_cb = cb; }

   protected:
    void onTimedEvent() override {
        if (m_cb) {
            m_cb();
        }
    }

   private:
    FN_CB m_cb;
};
}  // namespace jhc
#endif
#endif  // !JHC_WIN_TIMER_HPP_
