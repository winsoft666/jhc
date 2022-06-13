#include "jhc/config.hpp"
#ifdef JHC_NOT_HEADER_ONLY
#include "../win_timer.hpp"
#endif
#include <assert.h>

#ifdef JHC_WIN
namespace jhc {
class WinTimerBase::Private {
   public:
    HANDLE m_hTimer = NULL;
};

JHC_INLINE WinTimerBase::WinTimerBase() :
    p_(new WinTimerBase::Private()) {
}

JHC_INLINE WinTimerBase::~WinTimerBase() {
    delete p_;
    p_ = NULL;
}

JHC_INLINE void CALLBACK WinTimerBase::TimerProc(void* param, BOOLEAN timerCalled) {
    UNREFERENCED_PARAMETER(timerCalled);
    WinTimerBase* timer = static_cast<WinTimerBase*>(param);

    if (timer)
        timer->onTimedEvent();
}

// About dwFlags, see:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682485(v=vs.85).aspx
//
JHC_INLINE bool WinTimerBase::start(unsigned int ulInterval,  // ulInterval in ms
                                    bool bImmediately,
                                    bool bOnce,
                                    unsigned long dwFlags) {
    bool bRet = false;

    if (!p_->m_hTimer) {
        bRet = !!CreateTimerQueueTimer(&p_->m_hTimer, NULL, TimerProc, (PVOID)this,
                                       bImmediately ? 0 : ulInterval, bOnce ? 0 : ulInterval, dwFlags);
    }

    return bRet;
}

JHC_INLINE void WinTimerBase::stop(bool bWait) {
    if (p_->m_hTimer) {
        BOOL b = DeleteTimerQueueTimer(NULL, p_->m_hTimer, bWait ? INVALID_HANDLE_VALUE : NULL);
        assert(b);
        p_->m_hTimer = NULL;
    }
}
}  // namespace jhc
#endif