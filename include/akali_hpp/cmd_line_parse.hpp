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

#ifndef AKALI_CMDLIBE_PARSE_HPP__
#define AKALI_CMDLIBE_PARSE_HPP__

#include "akali_hpp/arch.hpp"
#include "akali_hpp/string_helper.hpp"
#include "akali_hpp/string_encode.hpp"
#include <unordered_map>

namespace akali_hpp {
namespace {
const wchar_t delims[] = L"-/";
const wchar_t quotes[] = L"\"";
const wchar_t value_sep[] = L" :=";  // don't forget space!!
}  // namespace

class CmdLineParser {
   public:
    typedef std::unordered_map<std::wstring, std::wstring> ValsMap;
    typedef ValsMap::const_iterator ITERPOS;

    explicit CmdLineParser(const std::wstring& cmdline) {
        if (cmdline.length() > 0) {
            cmdline_ = cmdline;
            parse();
        }
    }

    ~CmdLineParser() = default;

    ITERPOS begin() const {
        return value_map_.begin();
    }

    ITERPOS end() const {
        return value_map_.end();
    }

    bool hasKey(const std::wstring& key) const {
        ITERPOS it = findKey(key);

        if (it == value_map_.end())
            return false;

        return true;
    }

    bool hasVal(const std::wstring& key) const {
        ITERPOS it = findKey(key);

        if (it == value_map_.end())
            return false;

        if (it->second.length() == 0)
            return false;

        return true;
    }

    std::wstring getVal(const std::wstring& key) const {
        ITERPOS it = findKey(key);

        if (it == value_map_.end())
            return std::wstring();

        return it->second;
    }

    int getKeyCount() const {
        return value_map_.size();
    }

   private:
    void parse() {
        value_map_.clear();

        std::wstring strW = cmdline_;
        const wchar_t* sCurrent = strW.c_str();

        for (;;) {
            if (sCurrent[0] == L'\0')
                break;

            const wchar_t* sArg = wcspbrk(sCurrent, delims);

            if (!sArg)
                break;

            sArg++;

            if (sArg[0] == L'\0')
                break;  // ends with delim

            const wchar_t* sVal = wcspbrk(sArg, value_sep);

            if (sVal == NULL) {
                std::wstring Key(sArg);
                Key = StringHelper::ToLower(Key);
                value_map_.insert(CmdLineParser::ValsMap::value_type(Key, L""));
                break;
            }
            else if (sVal[0] == L' ' || wcslen(sVal) == 1) {
                // cmdline ends with /Key: or a key with no value
                std::wstring Key(sArg, (int)(sVal - sArg));

                if (Key.length() > 0) {
                    Key = StringHelper::ToLower(Key);
                    value_map_.insert(CmdLineParser::ValsMap::value_type(Key, L""));
                }

                sCurrent = sVal + 1;
                continue;
            }
            else {
                // key has value
                std::wstring Key(sArg, (int)(sVal - sArg));
                Key = StringHelper::ToLower(Key);

                sVal++;

                const wchar_t* sQuote = wcspbrk(sVal, quotes);
                const wchar_t* sEndQuote = NULL;

                if (sQuote == sVal) {
                    // string with quotes (defined in quotes, e.g. '")
                    sQuote = sVal + 1;
                    sEndQuote = wcspbrk(sQuote, quotes);
                }
                else {
                    sQuote = sVal;
                    sEndQuote = wcschr(sQuote, L' ');
                }

                if (sEndQuote == NULL) {
                    // no end quotes or terminating space, take the rest of the string to its end
                    std::wstring csVal(sQuote);

                    if (Key.length() > 0) {
                        value_map_.insert(CmdLineParser::ValsMap::value_type(Key, csVal));
                    }

                    break;
                }
                else {
                    // end quote
                    if (Key.length() > 0) {
                        std::wstring csVal(sQuote, (int)(sEndQuote - sQuote));
                        value_map_.insert(CmdLineParser::ValsMap::value_type(Key, csVal));
                    }

                    sCurrent = sEndQuote + 1;
                    continue;
                }
            }
        }
    }

    ITERPOS findKey(const std::wstring& key) const {
        const std::wstring keyLower = StringHelper::ToLower(key);
        return value_map_.find(keyLower);
    }

    std::wstring cmdline_;
    ValsMap value_map_;
};
}  // namespace akali_hpp
#endif  //!AKALI_CMDLIBE_PARSE_HPP__