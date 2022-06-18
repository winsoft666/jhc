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
#ifndef JHC_BYTE_ORDER_HPP__
#define JHC_BYTE_ORDER_HPP__
#pragma once

#include "jhc/config.hpp"
#include "jhc/arch.hpp"

#include <stdlib.h>
#include <stddef.h>  // for NULL, size_t
#include <stdint.h>  // for uintptr_t and (u)int_t types.
#ifdef JHC_WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#if (defined __APPLE__)
#include <libkern/OSByteOrder.h>
#endif

#ifdef JHC_WIN
#pragma comment(lib, "ws2_32.lib")
#endif

namespace jhc {
#ifdef JHC_WIN
#define htobe16(v) htons(v)
#define htobe32(v) htonl(v)
#define be16toh(v) ntohs(v)
#define be32toh(v) ntohl(v)

#elif (defined JHC_MACOS)

#define htobe16(v) OSSwapHostToBigInt16(v)
#define htobe32(v) OSSwapHostToBigInt32(v)
#define htobe64(v) OSSwapHostToBigInt64(v)
#define be16toh(v) OSSwapBigToHostInt16(v)
#define be32toh(v) OSSwapBigToHostInt32(v)
#define be64toh(v) OSSwapBigToHostInt64(v)

#define htole16(v) OSSwapHostToLittleInt16(v)
#define htole32(v) OSSwapHostToLittleInt32(v)
#define htole64(v) OSSwapHostToLittleInt64(v)
#define le16toh(v) OSSwapLittleToHostInt16(v)
#define le32toh(v) OSSwapLittleToHostInt32(v)
#define le64toh(v) OSSwapLittleToHostInt64(v)
#endif

#if _WIN32_WINNT >= 0x0602 || WINVER >= 0x0602  // Win8
#define htobe64(v) htonll((v))
#define be64toh(v) ntohll((v))
#endif

#if defined(JHC_ARCH_LITTLE_ENDIAN)
#ifndef htole16
#define htole16(v) (v)
#endif

#ifndef htole32
#define htole32(v) (v)
#endif

#ifndef htole64
#define htole64(v) (v)
#endif

#ifndef le16toh
#define le16toh(v) (v)
#endif

#ifndef le32toh
#define le32toh(v) (v)
#endif

#ifndef le64toh
#define le64toh(v) (v)
#endif
#elif defined(JHC_ARCH_BIG_ENDIAN)
#ifndef htole16
#define htole16(v) __builtin_bswap16(v)
#endif

#ifndef htole32
#define htole32(v) __builtin_bswap32(v)
#endif

#ifndef htole64
#define htole64(v) __builtin_bswap64(v)
#endif

#ifndef le16toh
#define le16toh(v) __builtin_bswap16(v)
#endif

#ifndef le32toh
#define le32toh(v) __builtin_bswap32(v)
#endif

#ifndef le64toh
#define le64toh(v) __builtin_bswap64(v)
#endif

#endif

class ByteOrder {
   public:
    static bool IsHostBigEndian();

    // Reading and writing of little and big-endian numbers from memory
    static void Set8(void* memory, size_t offset, uint8_t v);
    static uint8_t Get8(const void* memory, size_t offset);
    static void SetBE16(void* memory, uint16_t v);
    static void SetBE32(void* memory, uint32_t v);
#if _WIN32_WINNT >= 0x0602 || WINVER >= 0x0602  // Win8
    static void SetBE64(void* memory, uint64_t v);
    static uint64_t GetBE64(const void* memory);
    static uint64_t HostToNetwork64(uint64_t n);
    static uint64_t NetworkToHost64(uint64_t n);
#endif
    static void SetLE64(void* memory, uint64_t v);
    static uint16_t GetBE16(const void* memory);
    static uint32_t GetBE32(const void* memory);
    static void SetLE16(void* memory, uint16_t v);
    static void SetLE32(void* memory, uint32_t v);
    static uint16_t GetLE16(const void* memory);
    static uint32_t GetLE32(const void* memory);
    static uint64_t GetLE64(const void* memory);
    static uint16_t HostToNetwork16(uint16_t n);
    static uint32_t HostToNetwork32(uint32_t n);
    static uint16_t NetworkToHost16(uint16_t n);
    static uint32_t NetworkToHost32(uint32_t n);
    static void ByteSwap(void* inp, size_t size);
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/byteorder.cc"
#endif

#endif  // !JHC_BYTE_ORDER_HPP__