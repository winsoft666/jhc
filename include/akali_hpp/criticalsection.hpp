/*******************************************************************************
 * Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
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

#ifndef AKALI_CRITICAL_SECTION_HPP__
#define AKALI_CRITICAL_SECTION_HPP__
#pragma once

#include "akali_hpp/arch.hpp"

#ifdef AKALI_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "akali_hpp/constructormagic.hpp"

namespace akali_hpp {
class CriticalSection {
   public:
    CriticalSection() { InitializeCriticalSection(&crit_); }
    ~CriticalSection() { DeleteCriticalSection(&crit_); }
    void Enter() const { EnterCriticalSection(&crit_); }
    void Leave() const { LeaveCriticalSection(&crit_); }
    bool TryEnter() const { return TryEnterCriticalSection(&crit_) != FALSE; }

   private:
    CriticalSection(const CriticalSection& refCritSec);
    CriticalSection& operator=(const CriticalSection& refCritSec);
    mutable CRITICAL_SECTION crit_;
};

class AKALI_API CritScope {
   public:
    explicit CritScope(const CriticalSection* pCS) :
        crit_(pCS) { crit_->Enter(); }
    ~CritScope() { crit_->Leave(); }

   private:
    const CriticalSection* const crit_;
    AKALI_DISALLOW_COPY_AND_ASSIGN(CritScope);
};
}  // namespace akali_hpp
#endif
#endif  // !AKALI_CRITICAL_SECTION_HPP__
