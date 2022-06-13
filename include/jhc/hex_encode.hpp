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

#ifndef JHC_HEX_ENCODE_HPP__
#define JHC_HEX_ENCODE_HPP__
#include "jhc/config.hpp"
#include <cassert>
#include <string>

namespace jhc {
class HexEncode {
   public:
    static char Encode(unsigned char val);

    static bool Decode(char ch, unsigned char* val);

    static size_t EncodeWithDelimiter(char* buffer,
                                      size_t buflen,
                                      const char* csource,
                                      size_t srclen,
                                      char delimiter);

    static std::string Encode(const std::string& str);

    static std::string Encode(const char* source, size_t srclen);

    static std::string EncodeWithDelimiter(const char* source, size_t srclen, char delimiter);

    static size_t DecodeWithDelimiter(char* cbuffer,
                                      size_t buflen,
                                      const char* source,
                                      size_t srclen,
                                      char delimiter);

    static size_t Decode(char* buffer, size_t buflen, const std::string& source);

    static std::string Decode(const std::string& str);

    static size_t DecodeWithDelimiter(char* buffer,
                                      size_t buflen,
                                      const std::string& source,
                                      char delimiter);
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/hex_encode.cc"
#endif
#endif  // !JHC_HEX_ENCODE_HPP__
