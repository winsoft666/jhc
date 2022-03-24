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

#include <string>
#include "jhc/arch.hpp"
#ifdef JHC_WIN
#include <combaseapi.h>
#else
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#endif
#include "jhc/string_helper.hpp"

namespace jhc {
    class UUID {
    public:
#ifndef JHC_WIN
        static uint32_t Rand32()
        {
            return ((rand() & 0x3) << 30) | ((rand() & 0x7fff) << 15) | (rand() & 0x7fff);
        }

        static std::string GenUuid4()
        {
            return StringHelper::StringPrintf("%08x-%04x-%04x-%04x-%04x%08x",
                Rand32(),                         // Generates a 32-bit Hex number
                Rand32() & 0xffff,                // Generates a 16-bit Hex number
                ((Rand32() & 0x0fff) | 0x4000),   // Generates a 16-bit Hex number of the form 4xxx (4 indicates the UUID version)
                (Rand32() & 0x3fff) + 0x8000,     // Generates a 16-bit Hex number in the range [0x8000, 0xbfff]
                Rand32() & 0xffff, Rand32());
        }
#endif

        static std::string Create() {
#ifdef JHC_WIN
            GUID guid;
            CoCreateGuid(&guid);

            return StringHelper::StringPrintf("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                guid.Data1, guid.Data2, guid.Data3,
                guid.Data4[0], guid.Data4[1],
                guid.Data4[2], guid.Data4[3],
                guid.Data4[4], guid.Data4[5],
                guid.Data4[6], guid.Data4[7]);
#else
            return GenUuid4();
#endif
        }
    };
} // namespace jhc
#endif // !JHC_GUID_HPP__