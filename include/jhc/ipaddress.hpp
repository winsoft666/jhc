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
    IPAddress();

    explicit IPAddress(const in_addr& ip4);

    explicit IPAddress(const in6_addr& ip6);

    explicit IPAddress(uint32_t ip_in_host_byte_order);

    explicit IPAddress(const std::string& str);

    IPAddress(const IPAddress& other);

    virtual ~IPAddress() = default;

    const IPAddress& operator=(const IPAddress& other);

    bool operator==(const IPAddress& other) const;
    bool operator!=(const IPAddress& other) const;
    bool operator<(const IPAddress& other) const;
    bool operator>(const IPAddress& other) const;

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
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/ipaddress.cc"
#endif
#endif  // !JHC_IP_ADDRESS_HPP__