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

#ifndef JHC_MD5_HPP__
#define JHC_MD5_HPP__
#include "jhc/config.hpp"
#include <string>
#include <memory.h>
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include "jhc/filesystem.hpp"

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
    static std::string GetDataMD5(const unsigned char* buffer, size_t buffer_size);
    static std::string GetFileMD5(const fs::path& file_path);

   public:
    /*
       * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
       * initialization constants.
    */
    void MD5Init(struct MD5Context* ctx);

    /*
       * Update context to reflect the concatenation of another buffer full
       * of bytes.
    */
    void MD5Update(struct MD5Context* ctx, unsigned char const* buf, unsigned len);

    /*
       * Final wrapup - pad to 64-byte boundary with the bit pattern
       * 1 0* (64-bit count of bits processed, MSB-first)
    */
    void MD5Final(unsigned char digest[16], struct MD5Context* ctx);

    void MD5Buffer(const unsigned char* buf, unsigned int len, unsigned char sig[16]);

    void MD5SigToString(unsigned char signature[16], char* str, int len);

   private:
#ifndef ASM_MD5
     /*
     * The core of the MD5 algorithm, this alters an existing MD5 hash to
     * reflect the addition of 16 longwords of new data.  MD5Update blocks
     * the data and converts bytes into longwords for this routine.
     */
    static void MD5Transform(unsigned int buf[4], unsigned int const in[16]);

#endif

    void byteSwap(unsigned int* buf, unsigned words);

    bool bigEndian_ = false;
    const char HEX_STRING[17] = "0123456789abcdef"; /* to convert to hex */
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/md5.cc"
#endif
#endif  // !JHC_MD5_HPP__