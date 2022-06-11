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

#ifndef JHC_WIN_CRITICAL_SECTION_HPP__
#define JHC_WIN_CRITICAL_SECTION_HPP__
//
// Windows CriticalSection wrapper.
//

#include "jhc/arch.hpp"
#ifdef JHC_WIN
#include "jhc/macros.hpp"

namespace jhc {
class WinCriticalSection {
   public:
    JHC_DISALLOW_COPY_MOVE(WinCriticalSection);

    WinCriticalSection();
    ~WinCriticalSection();
    void enter() const;
    void leave() const;
    bool tryEnter() const;

   private:
    class Private;
    Private* p_ = nullptr;
};

class ScopedWinCriticalSection {
   public:
    JHC_DISALLOW_COPY_MOVE(ScopedWinCriticalSection);

    explicit ScopedWinCriticalSection(const WinCriticalSection* pCS) :
        crit_(pCS) { crit_->enter(); }
    ~ScopedWinCriticalSection() { crit_->leave(); }

   private:
    const WinCriticalSection* const crit_;
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/win_criticalsection.cc"
#endif
#endif  // !JHC_WIN
#endif  // !JHC_WIN_CRITICAL_SECTION_HPP__
