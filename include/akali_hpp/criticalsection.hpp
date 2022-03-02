/*******************************************************************************
*    Copyright (C) <2022>  <winsoft666@outlook.com>.
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

#ifndef AKALI_CRITICAL_SECTION_HPP__
#define AKALI_CRITICAL_SECTION_HPP__
#pragma once

#include "akali_hpp/arch.hpp"

#ifdef AKALI_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif // !_INC_WINDOWS
#include "akali_hpp/macros.hpp"

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

class CritScope {
   public:
    explicit CritScope(const CriticalSection* pCS) :
        crit_(pCS) { crit_->Enter(); }
    ~CritScope() { crit_->Leave(); }

   private:
    const CriticalSection* const crit_;
    AKALI_DISALLOW_COPY_AND_ASSIGN(CritScope);
};
}  // namespace akali_hpp
#endif // !AKALI_WIN
#endif  // !AKALI_CRITICAL_SECTION_HPP__
