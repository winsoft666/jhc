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

#ifndef JHC_SHA512_HPP__
#define JHC_SHA512_HPP__
#include <string>
#include "jhc/filesystem.hpp"

namespace jhc {
class SHA512 {
   public:
    void init();

    void update(const unsigned char* message, unsigned int len);

    void final(unsigned char* digest);

    void transform(const unsigned char* message, unsigned int block_nb);

    static std::string GetFileSHA512(const fs::path& filePath);

    static std::string GetDataSHA512(const unsigned char* data, size_t dataSize);

   private:
    typedef unsigned char uint8;
    typedef unsigned long uint32;
    typedef unsigned long long uint64;

    static uint64 getSha512K(int i);

    static const unsigned int SHA384_512_BLOCK_SIZE = (1024 / 8);
    static const unsigned int DIGEST_SIZE = (512 / 8);
    unsigned int m_tot_len;
    unsigned int m_len;
    unsigned char m_block[2 * SHA384_512_BLOCK_SIZE];
    uint64 m_h[8];
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/sha512.cc"
#endif

#endif  // !JHC_SHA512_HPP__
