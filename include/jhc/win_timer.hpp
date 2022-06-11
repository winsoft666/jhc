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

#ifndef JHC_WIN_TIMER_HPP_
#define JHC_WIN_TIMER_HPP_

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
class WinTimerBase {
   public:
    WinTimerBase();

    virtual ~WinTimerBase();

    static void CALLBACK TimerProc(void* param, BOOLEAN timerCalled);

    // About dwFlags, see:
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682485(v=vs.85).aspx
    //
    bool start(unsigned int ulInterval,  // ulInterval in ms
               bool bImmediately,
               bool bOnce,
               unsigned long dwFlags = WT_EXECUTELONGFUNCTION);

    void stop(bool bWait);

    virtual void onTimedEvent() {}

   private:
    class Private;
    Private* p_ = nullptr;
};

template <class T>
class TWinTimer : public WinTimerBase {
   public:
    typedef void (T::*POnTimer)(void);

    TWinTimer() {
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

class WinTimer : public WinTimerBase {
   public:
    typedef std::function<void()> FN_CB;
    WinTimer() {}

    WinTimer(FN_CB cb) { setTimedEvent(cb); }

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
