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

#ifndef SCOPED_OBJECT_HPP__
#define SCOPED_OBJECT_HPP__

#include "jhc/arch.hpp"
#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif // !_INC_WINDOWS
#else
#endif  // JHC_WIN
#include "jhc/macros.hpp"

namespace jhc {
#ifdef JHC_WIN
// Simple HANDLE wrapper to close it automatically from the destructor.
class ScopedHandle {
   public:
    ScopedHandle(bool isNullInvalid = true) noexcept :
        kInvalidHandle_(isNullInvalid ? NULL : INVALID_HANDLE_VALUE) {
        handle_ = kInvalidHandle_;
    }

    ~ScopedHandle() noexcept { close(); }

    void close() noexcept {
        if (handle_ != kInvalidHandle_) {
            CloseHandle(handle_);
            handle_ = kInvalidHandle_;
        }
    }

    HANDLE detach() noexcept {
        HANDLE old_handle = handle_;
        handle_ = kInvalidHandle_;
        return old_handle;
    }

    operator HANDLE() const noexcept { return handle_; }
    HANDLE* operator&() noexcept { return &handle_; }
    HANDLE invalidHandle() const noexcept { return kInvalidHandle_; }

   private:
    const HANDLE kInvalidHandle_;
    HANDLE handle_;

    JHC_DISALLOW_COPY_MOVE(ScopedHandle);
};

#else
#endif
}  // namespace jhc
#endif  // !SCOPED_OBJECT_HPP__