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
#ifndef AKALI_BYTE_ORDER_HPP__
#define AKALI_BYTE_ORDER_HPP__

#include "akali_hpp/arch.hpp"

#include <stdlib.h>
#include <stddef.h>  // for NULL, size_t
#include <stdint.h>  // for uintptr_t and (u)int_t types.
#ifdef AKALI_WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#if (defined __APPLE__)
#include <libkern/OSByteOrder.h>
#endif

#ifdef AKALI_WIN
#pragma comment(lib, "ws2_32.lib")
#endif

namespace akl {
#ifdef AKALI_WIN
#define htobe16(v) htons(v)
#define htobe32(v) htonl(v)
#define be16toh(v) ntohs(v)
#define be32toh(v) ntohl(v)

#elif (defined AKALI_MACOS)

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

#if defined(AKALI_ARCH_LITTLE_ENDIAN)
#define htole16(v) (v)
#define htole32(v) (v)
#define htole64(v) (v)
#define le16toh(v) (v)
#define le32toh(v) (v)
#define le64toh(v) (v)
#elif defined(AKALI_ARCH_BIG_ENDIAN)
#define htole16(v) __builtin_bswap16(v)
#define htole32(v) __builtin_bswap32(v)
#define htole64(v) __builtin_bswap64(v)
#define le16toh(v) __builtin_bswap16(v)
#define le32toh(v) __builtin_bswap32(v)
#define le64toh(v) __builtin_bswap64(v)
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
};

inline bool ByteOrder::IsHostBigEndian() {
    int nl = 0x12345678;
    short ns = 0x1234;
    bool big_endian = false;

    unsigned char* p = (unsigned char*)(&nl);
    unsigned char* sp = (unsigned char*)(&ns);

    if (p[0] == 0x12 && p[1] == 0x34 && p[2] == 0x56 && p[3] == 0x78) {
        big_endian = true;
    }
    else if (p[0] == 0x78 && p[1] == 0x56 && p[2] == 0x34 && p[3] == 0x12) {
        big_endian = false;
    }
    else {
        big_endian = (*sp != 0x12);
    }

    return big_endian;
}

inline void ByteOrder::Set8(void* memory, size_t offset, uint8_t v) {
    static_cast<uint8_t*>(memory)[offset] = v;
}

inline uint8_t ByteOrder::Get8(const void* memory, size_t offset) {
    return static_cast<const uint8_t*>(memory)[offset];
}

inline void ByteOrder::SetBE16(void* memory, uint16_t v) {
    *static_cast<uint16_t*>(memory) = htobe16(v);
}

inline void ByteOrder::SetBE32(void* memory, uint32_t v) {
    *static_cast<uint32_t*>(memory) = htobe32(v);
}

#if _WIN32_WINNT >= 0x0602 || WINVER >= 0x0602  // Win8
inline void ByteOrder::SetBE64(void* memory, uint64_t v) {
    *static_cast<uint64_t*>(memory) = htobe64(v);
}

inline uint64_t ByteOrder::GetBE64(const void* memory) {
    return be64toh(*static_cast<const uint64_t*>(memory));
}

inline uint64_t ByteOrder::HostToNetwork64(uint64_t n) {
    return htobe64(n);
}

inline uint64_t ByteOrder::NetworkToHost64(uint64_t n) {
    return be64toh(n);
}
#endif
inline uint16_t ByteOrder::GetBE16(const void* memory) {
    return be16toh(*static_cast<const uint16_t*>(memory));
}

inline uint32_t ByteOrder::GetBE32(const void* memory) {
    return be32toh(*static_cast<const uint32_t*>(memory));
}

inline void ByteOrder::SetLE16(void* memory, uint16_t v) {
    *static_cast<uint16_t*>(memory) = htole16(v);
}

inline void ByteOrder::SetLE32(void* memory, uint32_t v) {
    *static_cast<uint32_t*>(memory) = htole32(v);
}

inline void ByteOrder::SetLE64(void* memory, uint64_t v) {
    *static_cast<uint64_t*>(memory) = htole64(v);
}

inline uint16_t ByteOrder::GetLE16(const void* memory) {
    return le16toh(*static_cast<const uint16_t*>(memory));
}

inline uint32_t ByteOrder::GetLE32(const void* memory) {
    return le32toh(*static_cast<const uint32_t*>(memory));
}

inline uint64_t ByteOrder::GetLE64(const void* memory) {
    return le64toh(*static_cast<const uint64_t*>(memory));
}

inline uint16_t ByteOrder::HostToNetwork16(uint16_t n) {
    return htobe16(n);
}

inline uint32_t ByteOrder::HostToNetwork32(uint32_t n) {
    return htobe32(n);
}

inline uint16_t ByteOrder::NetworkToHost16(uint16_t n) {
    return be16toh(n);
}

inline uint32_t ByteOrder::NetworkToHost32(uint32_t n) {
    return be32toh(n);
}
}  // namespace akl

#endif  // !AKALI_BYTE_ORDER_HPP__