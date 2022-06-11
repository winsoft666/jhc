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

#ifndef JHC_SHA256_HPP__
#define JHC_SHA256_HPP__
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include "jhc/filesystem.hpp"

namespace jhc {
class SHA256 {
   public:
    SHA256();
    ~SHA256();

    /* Initialize the SHA values */
    void init();

    void update(const unsigned char* buffer, uint32_t length);

    /* Final wrapup - pad to SHA1_DATA_SIZE-byte boundary with the bit pattern
      1 0* (64-bit count of bits processed, MSB-first) */
    void final();

    void digest(unsigned char* s);

    std::string digest();

    static std::string GetFileSHA256(const fs::path& filePath);
    static std::string GetDataSHA256(const unsigned char* data, size_t dataSize);

   private:
    void sha256_block(const unsigned char* block);

    /* Perform the SHA transformation.  Note that this code, like MD5, seems to
      break some optimizing compilers due to the complexity of the expressions
      and the size of the basic block.  It may be necessary to split it into
      sections, e.g. based on the four subrounds

      Note that this function destroys the data area */
    void sha256_transform(uint32_t* state, uint32_t* data);

    class Private;
    Private* p_ = nullptr;
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/sha256.cc"
#endif
#endif  // !JHC_SHA256_HPP__