#ifdef JHC_NOT_HEADER_ONLY
#include "../ipaddress.hpp"
#endif
#include "jhc/byteorder.hpp"

#ifdef JHC_WIN
#pragma comment(lib, "ws2_32.lib")
#endif

jhc::IPAddress::IPAddress() :
    family_(AF_UNSPEC) {
    memset(&u_, 0, sizeof(u_));
}

jhc::IPAddress::IPAddress(const in_addr& ip4) :
    family_(AF_INET) {
    memset(&u_, 0, sizeof(u_));
    u_.ip4 = ip4;
}

jhc::IPAddress::IPAddress(const in6_addr& ip6) :
    family_(AF_INET6) {
    u_.ip6 = ip6;
}

jhc::IPAddress::IPAddress(const IPAddress& other) :
    family_(other.family_) {
    ::memcpy(&u_, &other.u_, sizeof(u_));
}

const jhc::IPAddress& jhc::IPAddress::operator=(const jhc::IPAddress& other) {
    family_ = other.family_;
    memcpy(&u_, &other.u_, sizeof(u_));
    return *this;
}

static const in6_addr kV4MappedPrefix = {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF, 0}}};
static const in6_addr k6To4Prefix = {{{0x20, 0x02, 0}}};
static const in6_addr kTeredoPrefix = {{{0x20, 0x01, 0x00, 0x00}}};
static const in6_addr kV4CompatibilityPrefix = {{{0}}};
static const in6_addr k6BonePrefix = {{{0x3f, 0xfe, 0}}};

uint32_t jhc::IPAddress::v4AddressAsHostOrderInteger() const {
    if (family_ == AF_INET) {
        return ByteOrder::NetworkToHost32(u_.ip4.s_addr);
    }
    else {
        return 0;
    }
}

bool jhc::IPAddress::isUnspecifiedIP() const {
    return IPIsUnspec(*this);
}

size_t jhc::IPAddress::size() const {
    switch (family_) {
        case AF_INET:
            return sizeof(in_addr);

        case AF_INET6:
            return sizeof(in6_addr);
    }

    return 0;
}

bool jhc::IPAddress::operator==(const jhc::IPAddress& other) const {
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

bool jhc::IPAddress::operator!=(const jhc::IPAddress& other) const {
    return !((*this) == other);
}

bool jhc::IPAddress::operator>(const jhc::IPAddress& other) const {
    return (*this) != other && !((*this) < other);
}

jhc::IPAddress::IPAddress(uint32_t ip_in_host_byte_order) :
    family_(AF_INET) {
    memset(&u_, 0, sizeof(u_));
    u_.ip4.s_addr = jhc::ByteOrder::HostToNetwork32(ip_in_host_byte_order);
}

jhc::IPAddress::IPAddress(const std::string& str) :
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

bool jhc::IPAddress::operator<(const jhc::IPAddress& other) const {
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

in6_addr jhc::IPAddress::getIPv6Address() const {
    return u_.ip6;
}

in_addr jhc::IPAddress::getIPv4Address() const {
    return u_.ip4;
}

std::string jhc::IPAddress::toString() const {
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

std::string jhc::IPAddress::toSensitiveString() const {
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

jhc::IPAddress jhc::IPAddress::normalized() const {
    if (family_ != AF_INET6) {
        return *this;
    }

    if (!IPIsV4Mapped(*this)) {
        return *this;
    }

    in_addr addr = ExtractMappedAddress(u_.ip6);
    return IPAddress(addr);
}

jhc::IPAddress jhc::IPAddress::asIPv6Address() const {
    if (family_ != AF_INET) {
        return *this;
    }

    in6_addr v6addr = kV4MappedPrefix;
    ::memcpy(&v6addr.s6_addr[12], &u_.ip4.s_addr, sizeof(u_.ip4.s_addr));
    return jhc::IPAddress(v6addr);
}

bool jhc::InterfaceAddress::operator==(const jhc::InterfaceAddress& other) const {
    return ipv6_flags_ == other.ipv6_flags() && static_cast<const IPAddress&>(*this) == other;
}

bool jhc::InterfaceAddress::operator!=(const jhc::InterfaceAddress& other) const {
    return !((*this) == other);
}

const jhc::InterfaceAddress& jhc::InterfaceAddress::operator=(const jhc::InterfaceAddress& other) {
    ipv6_flags_ = other.ipv6_flags_;
    static_cast<jhc::IPAddress&>(*this) = other;
    return *this;
}

bool jhc::IPAddress::IsPrivateV4(uint32_t ip_in_host_order) {
    return ((ip_in_host_order >> 24) == 127) || ((ip_in_host_order >> 24) == 10) ||
           ((ip_in_host_order >> 20) == ((172 << 4) | 1)) ||
           ((ip_in_host_order >> 16) == ((192 << 8) | 168)) ||
           ((ip_in_host_order >> 16) == ((169 << 8) | 254));
}

in_addr jhc::IPAddress::ExtractMappedAddress(const in6_addr& in6) {
    in_addr ipv4;
    ::memcpy(&ipv4.s_addr, &in6.s6_addr[12], sizeof(ipv4.s_addr));
    return ipv4;
}

bool jhc::IPAddress::IPIsHelper(const IPAddress& ip, const in6_addr& tomatch, int length) {
    // Helper method for checking IP prefix matches (but only on whole byte lengths). Length is in bits.
    in6_addr addr = ip.getIPv6Address();
    return memcmp(&addr, &tomatch, (length >> 3)) == 0;
}

bool jhc::IPAddress::IPFromAddrInfo(struct addrinfo* info, jhc::IPAddress* out) {
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
        *out = jhc::IPAddress(addr->sin6_addr);
        return true;
    }

    return false;
}

bool jhc::IPAddress::IPFromString(const std::string& str, jhc::IPAddress* out) {
    if (!out) {
        return false;
    }

    in_addr addr;

    if (inet_pton(AF_INET, str.c_str(), &addr) == 0) {
        in6_addr addr6;

        if (inet_pton(AF_INET6, str.c_str(), &addr6) == 0) {
            *out = jhc::IPAddress();
            return false;
        }

        *out = jhc::IPAddress(addr6);
    }
    else {
        *out = jhc::IPAddress(addr);
    }

    return true;
}

bool jhc::IPAddress::IPIsAny(const jhc::IPAddress& ip) {
    switch (ip.getFamily()) {
        case AF_INET:
            return ip == jhc::IPAddress(INADDR_ANY);

        case AF_INET6:
            return ip == jhc::IPAddress(in6addr_any) || ip == jhc::IPAddress(kV4MappedPrefix);

        case AF_UNSPEC:
            return false;
    }

    return false;
}

bool jhc::IPAddress::IPIsLoopback(const jhc::IPAddress& ip) {
    switch (ip.getFamily()) {
        case AF_INET: {
            return (ip.v4AddressAsHostOrderInteger() >> 24) == 127;
        }

        case AF_INET6: {
            return ip == jhc::IPAddress(in6addr_loopback);
        }
    }

    return false;
}

bool jhc::IPAddress::IPIsPrivate(const jhc::IPAddress& ip) {
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

bool jhc::IPAddress::IPIsUnspec(const jhc::IPAddress& ip) {
    return ip.getFamily() == AF_UNSPEC;
}

size_t jhc::IPAddress::HashIP(const jhc::IPAddress& ip) {
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

jhc::IPAddress jhc::IPAddress::TruncateIP(const jhc::IPAddress& ip, int length) {
    if (length < 0) {
        return jhc::IPAddress();
    }

    if (ip.getFamily() == AF_INET) {
        if (length > 31) {
            return ip;
        }

        if (length == 0) {
            return jhc::IPAddress(INADDR_ANY);
        }

        int mask = (0xFFFFFFFF << (32 - length));
        uint32_t host_order_ip = jhc::ByteOrder::NetworkToHost32(ip.getIPv4Address().s_addr);
        in_addr masked;
        masked.s_addr = jhc::ByteOrder::HostToNetwork32(host_order_ip & mask);
        return jhc::IPAddress(masked);
    }
    else if (ip.getFamily() == AF_INET6) {
        if (length > 127) {
            return ip;
        }

        if (length == 0) {
            return jhc::IPAddress(in6addr_any);
        }

        in6_addr v6addr = ip.getIPv6Address();
        int position = length / 32;
        int inner_length = 32 - (length - (position * 32));
        // Note: 64bit mask constant needed to allow possible 32-bit left shift.
        uint32_t inner_mask = 0xFFFFFFFFLL << inner_length;
        uint32_t* v6_as_ints = reinterpret_cast<uint32_t*>(&v6addr.s6_addr);

        for (int i = 0; i < 4; ++i) {
            if (i == position) {
                uint32_t host_order_inner = jhc::ByteOrder::NetworkToHost32(v6_as_ints[i]);
                v6_as_ints[i] = ByteOrder::HostToNetwork32(host_order_inner & inner_mask);
            }
            else if (i > position) {
                v6_as_ints[i] = 0;
            }
        }

        return jhc::IPAddress(v6addr);
    }

    return jhc::IPAddress();
}

int jhc::IPAddress::CountIPMaskBits(jhc::IPAddress mask) {
    uint32_t word_to_count = 0;
    int bits = 0;

    switch (mask.getFamily()) {
        case AF_INET: {
            word_to_count = jhc::ByteOrder::NetworkToHost32(mask.getIPv4Address().s_addr);
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
                word_to_count = jhc::ByteOrder::NetworkToHost32(v6_as_ints[i]);
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

bool jhc::IPAddress::IPIs6Bone(const jhc::IPAddress& ip) {
    return IPIsHelper(ip, k6BonePrefix, 16);
}

bool jhc::IPAddress::IPIs6To4(const jhc::IPAddress& ip) {
    return IPIsHelper(ip, k6To4Prefix, 16);
}

bool jhc::IPAddress::IPIsLinkLocal(const jhc::IPAddress& ip) {
    // Can't use the helper because the prefix is 10 bits.
    in6_addr addr = ip.getIPv6Address();
    return addr.s6_addr[0] == 0xFE && addr.s6_addr[1] == 0x80;
}

// According to http://www.ietf.org/rfc/rfc2373.txt, Appendix A, page 19.  An
// address which contains MAC will have its 11th and 12th bytes as FF:FE as well as the U/L bit as 1.
bool jhc::IPAddress::IPIsMacBased(const jhc::IPAddress& ip) {
    in6_addr addr = ip.getIPv6Address();
    return ((addr.s6_addr[8] & 0x02) && addr.s6_addr[11] == 0xFF && addr.s6_addr[12] == 0xFE);
}

bool jhc::IPAddress::IPIsSiteLocal(const jhc::IPAddress& ip) {
    // Can't use the helper because the prefix is 10 bits.
    in6_addr addr = ip.getIPv6Address();
    return addr.s6_addr[0] == 0xFE && (addr.s6_addr[1] & 0xC0) == 0xC0;
}

bool jhc::IPAddress::IPIsULA(const jhc::IPAddress& ip) {
    // Can't use the helper because the prefix is 7 bits.
    in6_addr addr = ip.getIPv6Address();
    return (addr.s6_addr[0] & 0xFE) == 0xFC;
}

bool jhc::IPAddress::IPIsTeredo(const jhc::IPAddress& ip) {
    return IPIsHelper(ip, kTeredoPrefix, 32);
}

bool jhc::IPAddress::IPIsV4Compatibility(const jhc::IPAddress& ip) {
    return IPIsHelper(ip, kV4CompatibilityPrefix, 96);
}

bool jhc::IPAddress::IPIsV4Mapped(const jhc::IPAddress& ip) {
    return IPIsHelper(ip, kV4MappedPrefix, 96);
}

int jhc::IPAddress::IPAddressPrecedence(const jhc::IPAddress& ip) {
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

jhc::IPAddress jhc::IPAddress::GetLoopbackIP(int family) {
    if (family == AF_INET) {
        return jhc::IPAddress(INADDR_LOOPBACK);
    }

    if (family == AF_INET6) {
        return jhc::IPAddress(in6addr_loopback);
    }

    return jhc::IPAddress();
}

jhc::IPAddress jhc::IPAddress::GetAnyIP(int family) {
    if (family == AF_INET) {
        return jhc::IPAddress(INADDR_ANY);
    }

    if (family == AF_INET6) {
        return jhc::IPAddress(in6addr_any);
    }

    return jhc::IPAddress();
}

bool jhc::InterfaceAddress::IPFromString(const std::string& str, int flags, jhc::InterfaceAddress* out) {
    jhc::IPAddress ip;

    if (!jhc::IPAddress::IPFromString(str, &ip)) {
        return false;
    }

    *out = jhc::InterfaceAddress(ip, flags);
    return true;
}