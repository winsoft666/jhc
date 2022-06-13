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

#ifndef JHC_CRC32_HPP__
#define JHC_CRC32_HPP__
#pragma once

#include "jhc/config.hpp"
#include <stdint.h>
#include <string>
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include "jhc/filesystem.hpp"

namespace jhc {
class CRC32 {
   public:
    void init();

    void update(const unsigned char* pData, uint32_t uSize);

    void finish();

    std::string digest();

    static std::string GetFileCRC32(const fs::path& filePath);
    static std::string GetDataCRC32(const unsigned char* data, size_t dataSize);

   private:
    uint32_t ulCRC32_ = 0;
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/crc32.cc"
#endif
#endif  // !JHC_CRC32_HPP__