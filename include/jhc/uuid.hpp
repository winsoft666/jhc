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
#ifndef JHC_GUID_HPP__
#define JHC_GUID_HPP__
#pragma once

#include "jhc/config.hpp"
#include "jhc/arch.hpp"
#include <string>

namespace jhc {
class UUID {
   public:
    static std::string Create();

   private:
#ifndef JHC_WIN
    static uint32_t Rand32();
    static std::string GenUuid4();
#endif
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/uuid.cc"
#endif
#endif  // !JHC_GUID_HPP__
