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

#ifndef JHC_ENUM_CLASS_ALLOW_FLAGS_HPP_
#define JHC_ENUM_CLASS_ALLOW_FLAGS_HPP_

#include <type_traits>

namespace jhc {
namespace flags {

//template <class E, class Enabler = void>
//struct is_flags
//    : public std::false_type {};

template <typename T, typename = void>
struct is_flags : std::false_type {};

template <typename T>
struct is_flags<T, decltype(enableEnumFlags(T{}))> : std::true_type {};

}  // namespace flags
}  // namespace jhc

//#define ALLOW_FLAGS_FOR_ENUM(name)             \
//    namespace jhc {                            \
//    namespace flags {                          \
//    template <>                                \
//    struct is_flags<name> : std::true_type {}; \
//    }                                          \
//    }                                          \
//    using name##s = jhc::flags::flags<name>

#define ALLOW_FLAGS_FOR_ENUM(name)       \
    inline void enableEnumFlags(name) {} \
    using name##s = jhc::flags::flags<name>

#define ALLOW_FLAGS_FOR_ENUM_IN_CLASS(name)     \
    friend inline void enableEnumFlags(name) {} \
    using name##s = jhc::flags::flags<name>

#endif  // JHC_ENUM_CLASS_ALLOW_FLAGS_HPP_
