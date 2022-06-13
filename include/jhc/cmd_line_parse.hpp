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

#ifndef JHC_CMDLIBE_PARSE_HPP__
#define JHC_CMDLIBE_PARSE_HPP__
#pragma once

#include "jhc/config.hpp"
#include <unordered_map>

namespace jhc {
class CmdLineParser {
   public:
    typedef std::unordered_map<std::wstring, std::wstring> ValsMap;
    typedef ValsMap::const_iterator ITERPOS;

    explicit CmdLineParser(const std::wstring& cmdline);
    explicit CmdLineParser(std::wstring&& cmdline);
    CmdLineParser(const CmdLineParser& that);
    CmdLineParser(CmdLineParser&& that) noexcept;
    ~CmdLineParser() = default;

    ITERPOS begin() const;
    ITERPOS end() const;

    bool hasKey(const std::wstring& key) const;
    bool hasVal(const std::wstring& key) const;

    std::wstring getVal(const std::wstring& key) const;
    int getKeyCount() const;

   private:
    void parse();
    ITERPOS findKey(const std::wstring& key) const;

    std::wstring cmdline_;
    ValsMap value_map_;
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/cmd_line_parse.cc"
#endif

#endif  //!JHC_CMDLIBE_PARSE_HPP__
