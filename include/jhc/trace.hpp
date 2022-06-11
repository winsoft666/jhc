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

#ifndef JHC_TRACE_HPP__
#define JHC_TRACE_HPP__
#pragma once

namespace jhc {
class Trace {
   public:
    static void MsgW(const wchar_t* lpFormat, ...);
    static void MsgA(const char* lpFormat, ...);
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/trace.cc"
#endif

#endif  // !JHC_TRACE_H__