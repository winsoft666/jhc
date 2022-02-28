/*******************************************************************************
*    Copyright (C) <2022>  <winsoft666@outlook.com>.
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

#ifndef AKALI_XOR_STR_HPP__
#define AKALI_XOR_STR_HPP__

#include <array>
#include <type_traits>
#include <cstdlib>

namespace akali_hpp {

constexpr auto seed = static_cast<int>(__TIME__[7]) + static_cast<int>(__TIME__[6]) * 10 + static_cast<int>(__TIME__[4]) * 60 + static_cast<int>(__TIME__[3]) * 600 + static_cast<int>(__TIME__[1]) * 3600 + static_cast<int>(__TIME__[0]) * 36000;

template <int N>
struct RandomGenerator {
   private:
    static constexpr unsigned a = 16807;       // 7^5
    static constexpr unsigned m = 2147483647;  // 2^31 - 1

    static constexpr unsigned s = RandomGenerator<N - 1>::value;
    static constexpr unsigned lo = a * (s & 0xFFFF);             // Multiply lower 16 bits by 16807
    static constexpr unsigned hi = a * (s >> 16);                // Multiply higher 16 bits by 16807
    static constexpr unsigned lo2 = lo + ((hi & 0x7FFF) << 16);  // Combine lower 15 bits of hi with lo's upper bits
    static constexpr unsigned hi2 = hi >> 15;                    // Discard lower 15 bits of hi
    static constexpr unsigned lo3 = lo2 + hi;

   public:
    static constexpr unsigned max = m;
    static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
};

template <>
struct RandomGenerator<0> {
    static constexpr unsigned value = seed;
};

template <int N, int M>
struct RandomInt {
    static constexpr auto value = RandomGenerator<N + 1>::value % M;
};

template <int N>
struct RandomChar {
    static const char value = static_cast<char>(1 + RandomInt<N, 0x7F - 1>::value);
};

template <typename T, size_t BUFLEN, int K>
class XorStr {
   public:
    template <size_t... Is>
    constexpr __forceinline XorStr(const T* str, std::index_sequence<Is...>) :
        key(RandomChar<K>::value), s{enc(str[Is], Is)...} {
    }
    constexpr T enc(T c, int i) const {
        return c ^ key;  // ((key + i) & (1 << (sizeof(T) * 8)));
    }
    T dec(T c, int i) const {
        return c ^ key;  // ((key - i) & (1 << (sizeof(T) * 8)));
    }
    __forceinline decltype(auto) decrypt(void) {
        for (size_t i = 0; i < BUFLEN - 1; ++i) {
            d[i] = dec(s[i], i);
        }
        d[BUFLEN - 1] = 0;
        return d.data();
    }
#ifndef _DEBUG
    __forceinline ~XorStr() {
        memset(s.data(), 0, sizeof(T) * BUFLEN);
    }
#endif
    T key;
    std::array<T, BUFLEN> s;
    std::array<T, BUFLEN> d;
};

template <typename T>
class DynXorStr {
   public:
    __forceinline DynXorStr() {
        m_key = 0;
    }
    __forceinline DynXorStr(const std::basic_string<T>& str) :
        m_key(std::rand() & (1 << (sizeof(T) * 8))) {
        m_buffer.resize(str.length());
        for (size_t i = 0; i < str.length(); ++i)
            m_buffer[i] = enc(str[i], i);
    }
    __forceinline DynXorStr(const T* str, size_t len) :
        m_key(std::rand() & (1 << (sizeof(T) * 8))) {
        m_buffer.resize(len);
        for (size_t i = 0; i < len; ++i)
            m_buffer[i] = enc(str[i], i);
    }
    __forceinline DynXorStr(size_t len) :
        m_key(std::rand()) {
        m_buffer.resize(len);
    }
    __forceinline ~DynXorStr() {
        memset((void*)m_buffer.data(), 0, sizeof(T) * m_buffer.size());
    }
    __forceinline DynXorStr<T>& operator=(const T* str) {
        m_key = std::rand() & (1 << (sizeof(T) * 8));
        m_buffer = str;
        for (size_t i = 0; i < m_buffer.size(); ++i)
            m_buffer[i] = (T)enc(m_buffer[i], i);

        return *this;
    }
    __forceinline DynXorStr<T>& operator=(const std::basic_string<T>& str) {
        m_key = std::rand() & (1 << (sizeof(T) * 8));
        m_buffer.resize(str.length());
        for (size_t i = 0; i < m_buffer.size(); ++i)
            m_buffer[i] = (T)enc(str[i], i);

        return *this;
    }
    __forceinline std::basic_string<T> decrypt(void) const {
        if (m_buffer.empty())
            return m_buffer;
        std::basic_string<T> str(m_buffer);
        for (size_t i = 0; i < m_buffer.size(); ++i)
            str[i] = (T)dec(str[i], i);
        return str;
    }
    __forceinline std::basic_string<T> raw(void) const {
        return m_buffer;
    }
    __forceinline size_t length() const {
        return m_buffer.size();
    }
    __forceinline void resize(size_t len) {
        m_buffer.resize(len);
    }
    __forceinline void set(int i, T c) {
        m_buffer[i] = enc(c, i);
    }
    __forceinline T operator[](const int& i) {
        return (T)dec(m_buffer[i], i);
    }

   private:
    __forceinline T enc(T c, int i) const {
#ifdef _DEBUG
        return c;
#endif
        return c ^ (m_key + i);
    }
    __forceinline T dec(T c, int i) const {
#ifdef _DEBUG
        return c;
#endif
        return c ^ (m_key + i);
    }

    T m_key;
    std::basic_string<T> m_buffer;
};
}  // namespace akali_hpp

#define XORSTRA(s) (akali_hpp::XorStr<char, _countof(s), __COUNTER__>(s, std::make_index_sequence<_countof(s) - 1>()).decrypt())
#define XORSTRW(s) (akali_hpp::XorStr<wchar_t, _countof(s), __COUNTER__>(s, std::make_index_sequence<_countof(s) - 1>()).decrypt())

#endif