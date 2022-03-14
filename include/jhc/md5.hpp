﻿/*******************************************************************************
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

#ifndef JHC_MD5_HPP__
#define JHC_MD5_HPP__

#include <string>
#include "jhc/arch.hpp"
#include "jhc/byteorder.hpp"
#include <memory.h>

namespace jhc {
class MD5 {
   public:
    struct MD5Context {
        unsigned int buf[4];
        unsigned int bytes[2];
        unsigned int in[16];
    };

    // Support large memory.
    //
    static std::string GetDataMD5(const unsigned char* buffer, size_t buffer_size) {
        unsigned char md5Sig[16] = {0};
        char szMd5[33] = {0};

        MD5 md5;
        MD5Context md5Context;
        md5.MD5Init(&md5Context);

        size_t offset = 0;
        while (offset < buffer_size) {
            unsigned int needRead = 10240;
            if (offset + needRead > buffer_size)
                needRead = buffer_size - offset;

            md5.MD5Update(&md5Context, buffer + offset, needRead);
            offset += needRead;
        }
        
        md5.MD5Final(md5Sig, &md5Context);
        md5.MD5SigToString(md5Sig, szMd5, 33);

        return szMd5;
    }

#ifdef JHC_WIN
    static std::string GetFileMD5(const std::wstring& file_path) {
#else
    static std::string GetFileMD5(const std::string& file_path) {
#endif

#ifdef JHC_WIN
        FILE* f = nullptr;
        _wfopen_s(&f, file_path.c_str(), L"rb");
#else
        FILE* f = fopen(file_path.c_str(), "rb");
#endif

        if (!f)
            return "";

        MD5 md5;

        unsigned char szMd5Sig[16] = {0};
        char szMd5[33] = {0};
        MD5Context md5Context;
        md5.MD5Init(&md5Context);

        size_t dwReadBytes = 0;
        unsigned char szData[1024] = {0};

        while ((dwReadBytes = fread(szData, 1, 1024, f)) > 0) {
            md5.MD5Update(&md5Context, szData, dwReadBytes);
        }

        fclose(f);

        md5.MD5Final(szMd5Sig, &md5Context);
        md5.MD5SigToString(szMd5Sig, szMd5, 33);

        return szMd5;
    }

   public:
    /*
       * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
       * initialization constants.
       */
    void MD5Init(struct MD5Context* ctx) {
        bigEndian_ = ByteOrder::IsHostBigEndian();

        ctx->buf[0] = 0x67452301;
        ctx->buf[1] = 0xefcdab89;
        ctx->buf[2] = 0x98badcfe;
        ctx->buf[3] = 0x10325476;

        ctx->bytes[0] = 0;
        ctx->bytes[1] = 0;
    }

    /*
       * Update context to reflect the concatenation of another buffer full
       * of bytes.
       */
    void MD5Update(struct MD5Context* ctx, unsigned char const* buf, unsigned len) {
        unsigned int t;

        /* Update byte count */

        t = ctx->bytes[0];

        if ((ctx->bytes[0] = t + len) < t)
            ctx->bytes[1]++; /* Carry from low to high */

        t = 64 - (t & 0x3f); /* Space available in ctx->in (at least 1) */

        if (t > len) {
            memcpy((unsigned char*)ctx->in + 64 - t, buf, len);
            return;
        }

        /* First chunk is an odd size */
        memcpy((unsigned char*)ctx->in + 64 - t, buf, t);
        byteSwap(ctx->in, 16);
        MD5Transform(ctx->buf, ctx->in);
        buf += t;
        len -= t;

        /* Process data in 64-byte chunks */
        while (len >= 64) {
            memcpy(ctx->in, buf, 64);
            byteSwap(ctx->in, 16);
            MD5Transform(ctx->buf, ctx->in);
            buf += 64;
            len -= 64;
        }

        /* Handle any remaining bytes of data. */
        memcpy(ctx->in, buf, len);
    }

    /*
       * Final wrapup - pad to 64-byte boundary with the bit pattern
       * 1 0* (64-bit count of bits processed, MSB-first)
       */
    void MD5Final(unsigned char digest[16], struct MD5Context* ctx) {
        int count = ctx->bytes[0] & 0x3f; /* Number of bytes in ctx->in */
        unsigned char* p = (unsigned char*)ctx->in + count;

        /* Set the first char of padding to 0x80.  There is always room. */
        *p++ = 0x80;

        /* Bytes of padding needed to make 56 bytes (-8..55) */
        count = 56 - 1 - count;

        if (count < 0) { /* Padding forces an extra block */
            memset(p, 0, count + 8);
            byteSwap(ctx->in, 16);
            MD5Transform(ctx->buf, ctx->in);
            p = (unsigned char*)ctx->in;
            count = 56;
        }

        memset(p, 0, count);
        byteSwap(ctx->in, 14);

        /* Append length in bits and transform */
        ctx->in[14] = ctx->bytes[0] << 3;
        ctx->in[15] = ctx->bytes[1] << 3 | ctx->bytes[0] >> 29;
        MD5Transform(ctx->buf, ctx->in);

        byteSwap(ctx->buf, 4);
        memcpy(digest, ctx->buf, 16);
        memset(ctx, 0, sizeof(*ctx)); /* In case it's sensitive */
    }

    void MD5Buffer(const unsigned char* buf, unsigned int len, unsigned char sig[16]) {
        struct MD5Context md5;
        MD5Init(&md5);
        MD5Update(&md5, buf, len);
        MD5Final(sig, &md5);
    }

    void MD5SigToString(unsigned char signature[16], char* str, int len) {
        unsigned char* sig_p;
        char *str_p, *max_p;
        unsigned int high, low;

        str_p = str;
        max_p = str + len;

        for (sig_p = (unsigned char*)signature; sig_p < (unsigned char*)signature + 16; sig_p++) {
            high = *sig_p / 16;
            low = *sig_p % 16;

            /* account for 2 chars */
            if (str_p + 1 >= max_p) {
                break;
            }

            *str_p++ = HEX_STRING[high];
            *str_p++ = HEX_STRING[low];
        }

        /* account for 2 chars */
        if (str_p < max_p) {
            *str_p++ = '\0';
        }
    }

   private:
#ifndef ASM_MD5

    /* The four core functions - F1 is optimized somewhat */

    /* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, in, s) (w += f(x, y, z) + in, w = (w << s | w >> (32 - s)) + x)

    /*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
    static void MD5Transform(unsigned int buf[4], unsigned int const in[16]) {
        unsigned int a, b, c, d;

        a = buf[0];
        b = buf[1];
        c = buf[2];
        d = buf[3];

        MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
        MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
        MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
        MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
        MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
        MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
        MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
        MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
        MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
        MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
        MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
        MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
        MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
        MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
        MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
        MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

        MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
        MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
        MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
        MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
        MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
        MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
        MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
        MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
        MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
        MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
        MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
        MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
        MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
        MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
        MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
        MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

        MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
        MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
        MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
        MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
        MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
        MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
        MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
        MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
        MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
        MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
        MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
        MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
        MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
        MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
        MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
        MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

        MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
        MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
        MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
        MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
        MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
        MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
        MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
        MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
        MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
        MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
        MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
        MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
        MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
        MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
        MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
        MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

        buf[0] += a;
        buf[1] += b;
        buf[2] += c;
        buf[3] += d;
    }

#endif

    void byteSwap(unsigned int* buf, unsigned words) {
        unsigned char* p;

        if (!bigEndian_)
            return;

        p = (unsigned char*)buf;

        do {
            *buf++ = (unsigned int)((unsigned)p[3] << 8 | p[2]) << 16 | ((unsigned)p[1] << 8 | p[0]);
            p += 4;
        } while (--words);
    }

    bool bigEndian_ = false;
    const char HEX_STRING[17] = "0123456789abcdef"; /* to convert to hex */
};
}  // namespace jhc
#endif  // !JHC_MD5_HPP__