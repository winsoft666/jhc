#include "jhc/config.hpp"

#ifdef JHC_NOT_HEADER_ONLY
#include "../byteorder.hpp"
#endif

JHC_INLINE bool jhc::ByteOrder::IsHostBigEndian() {
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

JHC_INLINE void jhc::ByteOrder::Set8(void* memory, size_t offset, uint8_t v) {
    static_cast<uint8_t*>(memory)[offset] = v;
}

JHC_INLINE uint8_t jhc::ByteOrder::Get8(const void* memory, size_t offset) {
    return static_cast<const uint8_t*>(memory)[offset];
}

JHC_INLINE void jhc::ByteOrder::SetBE16(void* memory, uint16_t v) {
    *static_cast<uint16_t*>(memory) = htobe16(v);
}

JHC_INLINE void jhc::ByteOrder::SetBE32(void* memory, uint32_t v) {
    *static_cast<uint32_t*>(memory) = htobe32(v);
}

#if _WIN32_WINNT >= 0x0602 || WINVER >= 0x0602  // Win8
JHC_INLINE void jhc::ByteOrder::SetBE64(void* memory, uint64_t v) {
    *static_cast<uint64_t*>(memory) = htobe64(v);
}

JHC_INLINE uint64_t jhc::ByteOrder::GetBE64(const void* memory) {
    return be64toh(*static_cast<const uint64_t*>(memory));
}

JHC_INLINE uint64_t jhc::ByteOrder::HostToNetwork64(uint64_t n) {
    return htobe64(n);
}

JHC_INLINE uint64_t jhc::ByteOrder::NetworkToHost64(uint64_t n) {
    return be64toh(n);
}
#endif
JHC_INLINE uint16_t jhc::ByteOrder::GetBE16(const void* memory) {
    return be16toh(*static_cast<const uint16_t*>(memory));
}

JHC_INLINE uint32_t jhc::ByteOrder::GetBE32(const void* memory) {
    return be32toh(*static_cast<const uint32_t*>(memory));
}

JHC_INLINE void jhc::ByteOrder::SetLE16(void* memory, uint16_t v) {
    *static_cast<uint16_t*>(memory) = htole16(v);
}

JHC_INLINE void jhc::ByteOrder::SetLE32(void* memory, uint32_t v) {
    *static_cast<uint32_t*>(memory) = htole32(v);
}

JHC_INLINE void jhc::ByteOrder::SetLE64(void* memory, uint64_t v) {
    *static_cast<uint64_t*>(memory) = htole64(v);
}

JHC_INLINE uint16_t jhc::ByteOrder::GetLE16(const void* memory) {
    return le16toh(*static_cast<const uint16_t*>(memory));
}

JHC_INLINE uint32_t jhc::ByteOrder::GetLE32(const void* memory) {
    return le32toh(*static_cast<const uint32_t*>(memory));
}

JHC_INLINE uint64_t jhc::ByteOrder::GetLE64(const void* memory) {
    return le64toh(*static_cast<const uint64_t*>(memory));
}

JHC_INLINE uint16_t jhc::ByteOrder::HostToNetwork16(uint16_t n) {
    return htobe16(n);
}

JHC_INLINE uint32_t jhc::ByteOrder::HostToNetwork32(uint32_t n) {
    return htobe32(n);
}

JHC_INLINE uint16_t jhc::ByteOrder::NetworkToHost16(uint16_t n) {
    return be16toh(n);
}

JHC_INLINE uint32_t jhc::ByteOrder::NetworkToHost32(uint32_t n) {
    return be32toh(n);
}

JHC_INLINE void jhc::ByteOrder::ByteSwap(void* inp, size_t size) {
    for (int i = 0; i < size / 2; i++) {
        uint8_t t = ((uint8_t*)inp)[size - 1 - i];
        ((uint8_t*)inp)[size - 1 - i] = ((uint8_t*)inp)[i];
        ((uint8_t*)inp)[i] = t;
    }
}
