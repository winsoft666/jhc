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

#ifndef JHC_WIN_INI_HPP_
#define JHC_WIN_INI_HPP_
#pragma once

#include "jhc/config.hpp"
#include "jhc/arch.hpp"
#ifdef JHC_WIN
#include <string>

namespace jhc {
class Ini {
   public:
    Ini() = default;

    Ini(const std::wstring& file_path);

    Ini(std::wstring&& file_path);

    ~Ini() = default;

    void setIniFilePath(const std::wstring& file_path) noexcept;

    std::wstring iniFilePath() const noexcept;

    bool readInt(const std::wstring& item, const std::wstring& sub_item, unsigned int& result) noexcept;

    unsigned int readInt(const std::wstring& item,
                         const std::wstring& sub_item,
                         unsigned int default_value) noexcept;

    std::wstring readString(const std::wstring& item,
                            const std::wstring& sub_item,
                            const std::wstring& default_value) noexcept;

    bool readString(const std::wstring& item,
                    const std::wstring& sub_item,
                    std::wstring& result) noexcept;

    bool writeInt(const std::wstring& item, const std::wstring& sub_item, unsigned int value) noexcept;

    bool writeString(const std::wstring& item,
                     const std::wstring& sub_item,
                     const std::wstring& value) noexcept;

   protected:
    std::wstring ini_file_path_;
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/win_ini.cc"
#endif
#endif  // !JHC_WIN
#endif  // !JHC_WIN_INI_HPP_
