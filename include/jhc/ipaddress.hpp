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

#ifndef JHC_IP_ADDRESS_HPP__
#define JHC_IP_ADDRESS_HPP__

#include "jhc/arch.hpp"
#ifdef JHC_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <netdb.h>
#endif
#include <string.h>
#include <string>
#include <vector>
#include "jhc/byteorder.hpp"

#ifdef JHC_WIN
#pragma comment(lib, "ws2_32.lib")
#endif

namespace jhc {
enum IPv6AddressFlag {
    IPV6_ADDRESS_FLAG_NONE = 0x00,

    // Temporary address is dynamic by nature and will not carry MAC address.
    IPV6_ADDRESS_FLAG_TEMPORARY = 1 << 0,

    // Temporary address could become deprecated once the preferred
    // lifetime is reached. It is still valid but just shouldn't be used
    // to create new connection.
    IPV6_ADDRESS_FLAG_DEPRECATED = 1 << 1,
};

// Version-agnostic IP address class, wraps a union of in_addr and in6_addr.
class IPAddress {
   public:
    IPAddress() :
        family_(AF_UNSPEC) {
        memset(&u_, 0, sizeof(u_));
    }

    explicit IPAddress(const in_addr& ip4) :
        family_(AF_INET) {
        memset(&u_, 0, sizeof(u_));
        u_.ip4 = ip4;
    }

    explicit IPAddress(const in6_addr& ip6) :
        family_(AF_INET6) { u_.ip6 = ip6; }

    explicit IPAddress(uint32_t ip_in_host_byte_order);

    explicit IPAddress(const std::string& str);

    IPAddress(const IPAddress& other) :
        family_(other.family_) {
        ::memcpy(&u_, &other.u_, sizeof(u_));
    }

    virtual ~IPAddress() {}

    const IPAddress& operator=(const IPAddress& other) {
        family_ = other.family_;
        memcpy(&u_, &other.u_, sizeof(u_));
        return *this;
    }

    bool operator==(const IPAddress& other) const;
    bool operator!=(const IPAddress& other) const;
    bool operator<(const IPAddress& other) const;
    bool operator>(const IPAddress& other) const;
    friend std::ostream& operator<<(std::ostream& os, const IPAddress& addr);

    int getFamily() const { return family_; }
    in_addr getIPv4Address() const;
    in6_addr getIPv6Address() const;

    // Returns the number of bytes needed to store the raw address.
    size_t size() const;

    // Wraps inet_ntop.
    std::string toString() const;

    // Same as ToString but annoymizes it by hiding the last part.
    std::string toSensitiveString() const;

    // Returns an unmapped address from a possibly-mapped address.
    // Returns the same address if this isn't a mapped address.
    IPAddress normalized() const;

    // Returns this address as an IPv6 address.
    // Maps v4 addresses (as ::ffff:a.b.c.d), returns v6 addresses unchanged.
    IPAddress asIPv6Address() const;

    // For socketaddress' benefit. Returns the IP in host byte order.
    uint32_t v4AddressAsHostOrderInteger() const;

    // Whether this is an unspecified IP address.
    bool isUnspecifiedIP() const;

    static bool IPFromAddrInfo(struct addrinfo* info, IPAddress* out);
    static bool IPFromString(const std::string& str, IPAddress* out);

    static bool IPIsAny(const IPAddress& ip);
    static bool IPIsLoopback(const IPAddress& ip);
    static bool IPIsPrivate(const IPAddress& ip);
    static bool IPIsUnspec(const IPAddress& ip);
    static size_t HashIP(const IPAddress& ip);

    // These are only really applicable for IPv6 addresses.
    static bool IPIs6Bone(const IPAddress& ip);
    static bool IPIs6To4(const IPAddress& ip);
    static bool IPIsLinkLocal(const IPAddress& ip);
    static bool IPIsMacBased(const IPAddress& ip);
    static bool IPIsSiteLocal(const IPAddress& ip);
    static bool IPIsTeredo(const IPAddress& ip);
    static bool IPIsULA(const IPAddress& ip);
    static bool IPIsV4Compatibility(const IPAddress& ip);
    static bool IPIsV4Mapped(const IPAddress& ip);

    // Returns the precedence value for this IP as given in RFC3484.
    static int IPAddressPrecedence(const IPAddress& ip);

    // Returns 'ip' truncated to be 'length' bits long.
    static IPAddress TruncateIP(const IPAddress& ip, int length);

    static IPAddress GetLoopbackIP(int family);
    static IPAddress GetAnyIP(int family);

    // Returns the number of contiguously set bits, counting from the MSB in network
    // byte order, in this IPAddress. Bits after the first 0 encountered are not counted.
    static int CountIPMaskBits(IPAddress mask);

    static bool IPIsHelper(const IPAddress& ip, const in6_addr& tomatch, int length);

    static bool IsPrivateV4(uint32_t ip_in_host_order);
    static in_addr ExtractMappedAddress(const in6_addr& in6);

   private:
    int family_;
    union {
        in_addr ip4;
        in6_addr ip6;
    } u_;
};

// IP class which could represent IPv6 address flags which is only meaningful in IPv6 case.
class InterfaceAddress : public IPAddress {
   public:
    InterfaceAddress() :
        ipv6_flags_(IPV6_ADDRESS_FLAG_NONE) {}

    InterfaceAddress(IPAddress ip) :
        IPAddress(ip), ipv6_flags_(IPV6_ADDRESS_FLAG_NONE) {}

    InterfaceAddress(IPAddress addr, int ipv6_flags) :
        IPAddress(addr), ipv6_flags_(ipv6_flags) {}

    InterfaceAddress(const in6_addr& ip6, int ipv6_flags) :
        IPAddress(ip6), ipv6_flags_(ipv6_flags) {}

    const InterfaceAddress& operator=(const InterfaceAddress& other);

    bool operator==(const InterfaceAddress& other) const;
    bool operator!=(const InterfaceAddress& other) const;

    int ipv6_flags() const { return ipv6_flags_; }

    static bool IPFromString(const std::string& str, int flags, InterfaceAddress* out);

   private:
    int ipv6_flags_;
};

/////////////////////////////////////////////////////////////////////
/// Implement
/////////////////////////////////////////////////////////////////////
///
///
// Prefixes used for categorizing IPv6 addresses.
static const in6_addr kV4MappedPrefix = {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF, 0}}};
static const in6_addr k6To4Prefix = {{{0x20, 0x02, 0}}};
static const in6_addr kTeredoPrefix = {{{0x20, 0x01, 0x00, 0x00}}};
static const in6_addr kV4CompatibilityPrefix = {{{0}}};
static const in6_addr k6BonePrefix = {{{0x3f, 0xfe, 0}}};

inline uint32_t IPAddress::v4AddressAsHostOrderInteger() const {
    if (family_ == AF_INET) {
        return ByteOrder::NetworkToHost32(u_.ip4.s_addr);
    }
    else {
        return 0;
    }
}

inline bool IPAddress::isUnspecifiedIP() const {
    return IPIsUnspec(*this);
}

inline size_t IPAddress::size() const {
    switch (family_) {
        case AF_INET:
            return sizeof(in_addr);

        case AF_INET6:
            return sizeof(in6_addr);
    }

    return 0;
}

inline bool IPAddress::operator==(const IPAddress& other) const {
    if (family_ != other.family_) {
        return false;
    }

    if (family_ == AF_INET) {
        return memcmp(&u_.ip4, &other.u_.ip4, sizeof(u_.ip4)) == 0;
    }

    if (family_ == AF_INET6) {
        return memcmp(&u_.ip6, &other.u_.ip6, sizeof(u_.ip6)) == 0;
    }

    return family_ == AF_UNSPEC;
}

inline bool IPAddress::operator!=(const IPAddress& other) const {
    return !((*this) == other);
}

inline bool IPAddress::operator>(const IPAddress& other) const {
    return (*this) != other && !((*this) < other);
}

inline IPAddress::IPAddress(uint32_t ip_in_host_byte_order) :
    family_(AF_INET) {
    memset(&u_, 0, sizeof(u_));
    u_.ip4.s_addr = ByteOrder::HostToNetwork32(ip_in_host_byte_order);
}

inline IPAddress::IPAddress(const std::string& str) :
    family_(AF_INET) {
    memset(&u_, 0, sizeof(u_));
    if (!str.empty()) {
        in_addr addr;

        if (inet_pton(AF_INET, str.c_str(), &addr) == 1) {
            family_ = AF_INET;
            memset(&u_, 0, sizeof(u_));
            u_.ip4 = addr;
        }
        else {
            in6_addr addr6;
            if (inet_pton(AF_INET6, str.c_str(), &addr6) == 1) {
                family_ = AF_INET6;
                u_.ip6 = addr6;
            }
        }
    }
}

inline bool IPAddress::operator<(const IPAddress& other) const {
    // IPv4 is 'less than' IPv6
    if (family_ != other.family_) {
        if (family_ == AF_UNSPEC) {
            return true;
        }

        if (family_ == AF_INET && other.family_ == AF_INET6) {
            return true;
        }

        return false;
    }

    // Comparing addresses of the same family.
    switch (family_) {
        case AF_INET: {
            return ByteOrder::NetworkToHost32(u_.ip4.s_addr) < ByteOrder::NetworkToHost32(other.u_.ip4.s_addr);
        }

        case AF_INET6: {
            return memcmp(&u_.ip6.s6_addr, &other.u_.ip6.s6_addr, 16) < 0;
        }
    }

    // Catches AF_UNSPEC and invalid addresses.
    return false;
}

inline std::ostream& operator<<(std::ostream& os, const IPAddress& ip) {
    os << ip.toString();
    return os;
}

inline in6_addr IPAddress::getIPv6Address() const {
    return u_.ip6;
}

inline in_addr IPAddress::getIPv4Address() const {
    return u_.ip4;
}

inline std::string IPAddress::toString() const {
    if (family_ != AF_INET && family_ != AF_INET6) {
        return std::string();
    }

    char buf[INET6_ADDRSTRLEN] = {0};
    const void* src = &u_.ip4;

    if (family_ == AF_INET6) {
        src = &u_.ip6;
    }

    if (!inet_ntop(family_, (void*)src, buf, sizeof(buf))) {
        return std::string();
    }

    return std::string(buf);
}

inline std::string IPAddress::toSensitiveString() const {
#if !defined(NDEBUG)
    // Return non-stripped in debug.
    return toString();
#else

    switch (family_) {
        case AF_INET: {
            std::string address = ToString();
            size_t find_pos = address.rfind('.');

            if (find_pos == std::string::npos)
                return std::string();

            address.resize(find_pos);
            address += ".x";
            return address;
        }

        case AF_INET6: {
            std::string result;
            result.resize(INET6_ADDRSTRLEN);
            in6_addr addr = getIPv6Address();
            size_t len = snprintf(&(result[0]), result.size(), "%x:%x:%x:x:x:x:x:x",
                                  (addr.s6_addr[0] << 8) + addr.s6_addr[1],
                                  (addr.s6_addr[2] << 8) + addr.s6_addr[3],
                                  (addr.s6_addr[4] << 8) + addr.s6_addr[5]);
            result.resize(len);
            return result;
        }
    }

    return std::string();
#endif
}

inline IPAddress IPAddress::normalized() const {
    if (family_ != AF_INET6) {
        return *this;
    }

    if (!IPIsV4Mapped(*this)) {
        return *this;
    }

    in_addr addr = ExtractMappedAddress(u_.ip6);
    return IPAddress(addr);
}

inline IPAddress IPAddress::asIPv6Address() const {
    if (family_ != AF_INET) {
        return *this;
    }

    in6_addr v6addr = kV4MappedPrefix;
    ::memcpy(&v6addr.s6_addr[12], &u_.ip4.s_addr, sizeof(u_.ip4.s_addr));
    return IPAddress(v6addr);
}

inline bool InterfaceAddress::operator==(const InterfaceAddress& other) const {
    return ipv6_flags_ == other.ipv6_flags() && static_cast<const IPAddress&>(*this) == other;
}

inline bool InterfaceAddress::operator!=(const InterfaceAddress& other) const {
    return !((*this) == other);
}

inline const InterfaceAddress& InterfaceAddress::operator=(const InterfaceAddress& other) {
    ipv6_flags_ = other.ipv6_flags_;
    static_cast<IPAddress&>(*this) = other;
    return *this;
}

inline bool IPAddress::IsPrivateV4(uint32_t ip_in_host_order) {
    return ((ip_in_host_order >> 24) == 127) || ((ip_in_host_order >> 24) == 10) ||
           ((ip_in_host_order >> 20) == ((172 << 4) | 1)) ||
           ((ip_in_host_order >> 16) == ((192 << 8) | 168)) ||
           ((ip_in_host_order >> 16) == ((169 << 8) | 254));
}

inline in_addr IPAddress::ExtractMappedAddress(const in6_addr& in6) {
    in_addr ipv4;
    ::memcpy(&ipv4.s_addr, &in6.s6_addr[12], sizeof(ipv4.s_addr));
    return ipv4;
}

inline bool IPAddress::IPIsHelper(const IPAddress& ip, const in6_addr& tomatch, int length) {
    // Helper method for checking IP prefix matches (but only on whole byte lengths). Length is in bits.
    in6_addr addr = ip.getIPv6Address();
    return memcmp(&addr, &tomatch, (length >> 3)) == 0;
}

inline bool IPAddress::IPFromAddrInfo(struct addrinfo* info, IPAddress* out) {
    if (!info || !info->ai_addr) {
        return false;
    }

    if (info->ai_addr->sa_family == AF_INET) {
        sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(info->ai_addr);
        *out = IPAddress(addr->sin_addr);
        return true;
    }
    else if (info->ai_addr->sa_family == AF_INET6) {
        sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(info->ai_addr);
        *out = IPAddress(addr->sin6_addr);
        return true;
    }

    return false;
}

inline bool IPAddress::IPFromString(const std::string& str, IPAddress* out) {
    if (!out) {
        return false;
    }

    in_addr addr;

    if (inet_pton(AF_INET, str.c_str(), &addr) == 0) {
        in6_addr addr6;

        if (inet_pton(AF_INET6, str.c_str(), &addr6) == 0) {
            *out = IPAddress();
            return false;
        }

        *out = IPAddress(addr6);
    }
    else {
        *out = IPAddress(addr);
    }

    return true;
}

inline bool IPAddress::IPIsAny(const IPAddress& ip) {
    switch (ip.getFamily()) {
        case AF_INET:
            return ip == IPAddress(INADDR_ANY);

        case AF_INET6:
            return ip == IPAddress(in6addr_any) || ip == IPAddress(kV4MappedPrefix);

        case AF_UNSPEC:
            return false;
    }

    return false;
}

inline bool IPAddress::IPIsLoopback(const IPAddress& ip) {
    switch (ip.getFamily()) {
        case AF_INET: {
            return (ip.v4AddressAsHostOrderInteger() >> 24) == 127;
        }

        case AF_INET6: {
            return ip == IPAddress(in6addr_loopback);
        }
    }

    return false;
}

inline bool IPAddress::IPIsPrivate(const IPAddress& ip) {
    switch (ip.getFamily()) {
        case AF_INET: {
            return IsPrivateV4(ip.v4AddressAsHostOrderInteger());
        }

        case AF_INET6: {
            return IPIsLinkLocal(ip) || IPIsLoopback(ip);
        }
    }

    return false;
}

inline bool IPAddress::IPIsUnspec(const IPAddress& ip) {
    return ip.getFamily() == AF_UNSPEC;
}

inline size_t IPAddress::HashIP(const IPAddress& ip) {
    switch (ip.getFamily()) {
        case AF_INET: {
            return ip.getIPv4Address().s_addr;
        }

        case AF_INET6: {
            in6_addr v6addr = ip.getIPv6Address();
            const uint32_t* v6_as_ints = reinterpret_cast<const uint32_t*>(&v6addr.s6_addr);
            return v6_as_ints[0] ^ v6_as_ints[1] ^ v6_as_ints[2] ^ v6_as_ints[3];
        }
    }

    return 0;
}

inline IPAddress IPAddress::TruncateIP(const IPAddress& ip, int length) {
    if (length < 0) {
        return IPAddress();
    }

    if (ip.getFamily() == AF_INET) {
        if (length > 31) {
            return ip;
        }

        if (length == 0) {
            return IPAddress(INADDR_ANY);
        }

        int mask = (0xFFFFFFFF << (32 - length));
        uint32_t host_order_ip = ByteOrder::NetworkToHost32(ip.getIPv4Address().s_addr);
        in_addr masked;
        masked.s_addr = ByteOrder::HostToNetwork32(host_order_ip & mask);
        return IPAddress(masked);
    }
    else if (ip.getFamily() == AF_INET6) {
        if (length > 127) {
            return ip;
        }

        if (length == 0) {
            return IPAddress(in6addr_any);
        }

        in6_addr v6addr = ip.getIPv6Address();
        int position = length / 32;
        int inner_length = 32 - (length - (position * 32));
        // Note: 64bit mask constant needed to allow possible 32-bit left shift.
        uint32_t inner_mask = 0xFFFFFFFFLL << inner_length;
        uint32_t* v6_as_ints = reinterpret_cast<uint32_t*>(&v6addr.s6_addr);

        for (int i = 0; i < 4; ++i) {
            if (i == position) {
                uint32_t host_order_inner = ByteOrder::NetworkToHost32(v6_as_ints[i]);
                v6_as_ints[i] = ByteOrder::HostToNetwork32(host_order_inner & inner_mask);
            }
            else if (i > position) {
                v6_as_ints[i] = 0;
            }
        }

        return IPAddress(v6addr);
    }

    return IPAddress();
}

inline int IPAddress::CountIPMaskBits(IPAddress mask) {
    uint32_t word_to_count = 0;
    int bits = 0;

    switch (mask.getFamily()) {
        case AF_INET: {
            word_to_count = ByteOrder::NetworkToHost32(mask.getIPv4Address().s_addr);
            break;
        }

        case AF_INET6: {
            in6_addr v6addr = mask.getIPv6Address();
            const uint32_t* v6_as_ints = reinterpret_cast<const uint32_t*>(&v6addr.s6_addr);
            int i = 0;

            for (; i < 4; ++i) {
                if (v6_as_ints[i] != 0xFFFFFFFF) {
                    break;
                }
            }

            if (i < 4) {
                word_to_count = ByteOrder::NetworkToHost32(v6_as_ints[i]);
            }

            bits = (i * 32);
            break;
        }

        default: {
            return 0;
        }
    }

    if (word_to_count == 0) {
        return bits;
    }

    // Public domain bit-twiddling hack from:
    // http://graphics.stanford.edu/~seander/bithacks.html
    // Counts the trailing 0s in the word.
    unsigned int zeroes = 32;
    // This could also be written word_to_count &= -word_to_count, but
    // MSVC emits warning C4146 when negating an unsigned number.
    word_to_count &= ~word_to_count + 1;  // Isolate lowest set bit.

    if (word_to_count)
        zeroes--;

    if (word_to_count & 0x0000FFFF)
        zeroes -= 16;

    if (word_to_count & 0x00FF00FF)
        zeroes -= 8;

    if (word_to_count & 0x0F0F0F0F)
        zeroes -= 4;

    if (word_to_count & 0x33333333)
        zeroes -= 2;

    if (word_to_count & 0x55555555)
        zeroes -= 1;

    return bits + (32 - zeroes);
}

inline bool IPAddress::IPIs6Bone(const IPAddress& ip) {
    return IPIsHelper(ip, k6BonePrefix, 16);
}

inline bool IPAddress::IPIs6To4(const IPAddress& ip) {
    return IPIsHelper(ip, k6To4Prefix, 16);
}

inline bool IPAddress::IPIsLinkLocal(const IPAddress& ip) {
    // Can't use the helper because the prefix is 10 bits.
    in6_addr addr = ip.getIPv6Address();
    return addr.s6_addr[0] == 0xFE && addr.s6_addr[1] == 0x80;
}

// According to http://www.ietf.org/rfc/rfc2373.txt, Appendix A, page 19.  An
// address which contains MAC will have its 11th and 12th bytes as FF:FE as well as the U/L bit as 1.
inline bool IPAddress::IPIsMacBased(const IPAddress& ip) {
    in6_addr addr = ip.getIPv6Address();
    return ((addr.s6_addr[8] & 0x02) && addr.s6_addr[11] == 0xFF && addr.s6_addr[12] == 0xFE);
}

inline bool IPAddress::IPIsSiteLocal(const IPAddress& ip) {
    // Can't use the helper because the prefix is 10 bits.
    in6_addr addr = ip.getIPv6Address();
    return addr.s6_addr[0] == 0xFE && (addr.s6_addr[1] & 0xC0) == 0xC0;
}

inline bool IPAddress::IPIsULA(const IPAddress& ip) {
    // Can't use the helper because the prefix is 7 bits.
    in6_addr addr = ip.getIPv6Address();
    return (addr.s6_addr[0] & 0xFE) == 0xFC;
}

inline bool IPAddress::IPIsTeredo(const IPAddress& ip) {
    return IPIsHelper(ip, kTeredoPrefix, 32);
}

inline bool IPAddress::IPIsV4Compatibility(const IPAddress& ip) {
    return IPIsHelper(ip, kV4CompatibilityPrefix, 96);
}

inline bool IPAddress::IPIsV4Mapped(const IPAddress& ip) {
    return IPIsHelper(ip, kV4MappedPrefix, 96);
}

inline int IPAddress::IPAddressPrecedence(const IPAddress& ip) {
    // Precedence values from RFC 3484-bis. Prefers native v4 over 6to4/Teredo.
    if (ip.getFamily() == AF_INET) {
        return 30;
    }
    else if (ip.getFamily() == AF_INET6) {
        if (IPIsLoopback(ip)) {
            return 60;
        }
        else if (IPIsULA(ip)) {
            return 50;
        }
        else if (IPIsV4Mapped(ip)) {
            return 30;
        }
        else if (IPIs6To4(ip)) {
            return 20;
        }
        else if (IPIsTeredo(ip)) {
            return 10;
        }
        else if (IPIsV4Compatibility(ip) || IPIsSiteLocal(ip) || IPIs6Bone(ip)) {
            return 1;
        }
        else {
            // A 'normal' IPv6 address.
            return 40;
        }
    }

    return 0;
}

inline IPAddress IPAddress::GetLoopbackIP(int family) {
    if (family == AF_INET) {
        return IPAddress(INADDR_LOOPBACK);
    }

    if (family == AF_INET6) {
        return IPAddress(in6addr_loopback);
    }

    return IPAddress();
}

inline IPAddress IPAddress::GetAnyIP(int family) {
    if (family == AF_INET) {
        return IPAddress(INADDR_ANY);
    }

    if (family == AF_INET6) {
        return IPAddress(in6addr_any);
    }

    return IPAddress();
}

inline bool InterfaceAddress::IPFromString(const std::string& str, int flags, InterfaceAddress* out) {
    IPAddress ip;

    if (!IPAddress::IPFromString(str, &ip)) {
        return false;
    }

    *out = InterfaceAddress(ip, flags);
    return true;
}
}  // namespace jhc
#endif  // !JHC_IP_ADDRESS_HPP__