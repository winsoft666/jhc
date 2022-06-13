#include "jhc/config.hpp"
#ifdef JHC_NOT_HEADER_ONLY
#include "../win_criticalsection.hpp"
#endif

#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif  // !_INC_WINDOWS

namespace jhc {

class WinCriticalSection::Private {
   public:
    CRITICAL_SECTION crit_;
};

JHC_INLINE WinCriticalSection::WinCriticalSection() :
    p_(new WinCriticalSection::Private()) {
    InitializeCriticalSection(&p_->crit_);
}

JHC_INLINE WinCriticalSection::~WinCriticalSection() {
    DeleteCriticalSection(&p_->crit_);

    delete p_;
    p_ = nullptr;
}

JHC_INLINE void WinCriticalSection::enter() const {
    EnterCriticalSection(&p_->crit_);
}

JHC_INLINE void WinCriticalSection::leave() const {
    LeaveCriticalSection(&p_->crit_);
}

JHC_INLINE bool WinCriticalSection::tryEnter() const {
    return TryEnterCriticalSection(&p_->crit_) != FALSE;
}
}  // namespace jhc
#endif