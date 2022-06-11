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

#ifndef JHC_URL_ENCODE_HPP__
#define JHC_URL_ENCODE_HPP__
#include <string>
#include "hex_encode.hpp"

namespace jhc {
class UrlEncode {
   public:
    static std::string Encode(const std::string& str);

    static size_t Decode(char* buffer, size_t buflen, const char* source, size_t srclen);

    static std::string Decode(const std::string& source);

   private:
    // Apply any suitable string transform (including the ones above) to an STL
    // string. Stack-allocated temporary space is used for the transformation, so
    // value and source may refer to the same string.
    typedef size_t (*Transform)(char* buffer, size_t buflen, const char* source, size_t srclen);

    // Return the result of applying transform t to source.
    static std::string s_transform(const std::string& source, Transform t);
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/url_encode.cc"
#endif
#endif  // !JHC_URL_ENCODE_HPP__