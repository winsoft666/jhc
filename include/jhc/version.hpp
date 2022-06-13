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

#ifndef JHC_VERSION_HPP__
#define JHC_VERSION_HPP__
#pragma once
#include "jhc/config.hpp"
#include <stdlib.h>
#include <string>
#include <vector>
#include <jhc/string_helper.hpp>

namespace jhc {
class Version {
   public:
    Version(const std::string& s) {
        const std::string sc = StringHelper::Trim(s);
        const std::vector<std::string> v = StringHelper::Split(sc, ".");
        const bool valid = !v.empty() &&
                           std::find_if(v.begin(),
                                        v.end(),
                                        [](const std::string& c) {
                                            return !StringHelper::IsDigit(c);
                                        }) == v.end();
        if (valid) {
            for (const auto& c : v) {
                verElems_.push_back(atoi(c.c_str()));
            }
        }
    }

    Version(const std::wstring& s) {
        const std::wstring sc = StringHelper::Trim(s);
        const std::vector<std::wstring> v = StringHelper::Split(sc, L".");
        const bool valid = !v.empty() &&
                           std::find_if(v.begin(),
                                        v.end(),
                                        [](const std::wstring& c) {
                                            return !StringHelper::IsDigit(c);
                                        }) == v.end();
        if (valid) {
            for (const auto& c : v) {
                verElems_.push_back(_wtoi(c.c_str()));
            }
        }
    }

    Version(const Version& that) noexcept {
        this->verElems_ = that.verElems_;
    }

    bool isValid() const noexcept {
        return !verElems_.empty();
    }

    bool isSameFormat(const Version& that) const noexcept {
        return (this->verElems_.size() == that.verElems_.size());
    }

    bool operator==(const Version& that) noexcept {
        if (!isSameFormat(that))
            return false;

        bool result = true;
        for (size_t i = 0; i < verElems_.size(); i++) {
            if (verElems_[i] != that.verElems_[i]) {
                result = false;
                break;
            }
        }
        return result;
    }

    bool operator!=(const Version& that) noexcept {
        return !(*this == that);
    }

    bool operator>(const Version& that) {
        return compare(that) > 0;
    }

    bool operator<(const Version& that) {
        return compare(that) < 0;
    }

    bool operator<=(const Version& that) {
        return compare(that) <= 0;
    }

    bool operator>=(const Version& that) {
        return compare(that) >= 0;
    }

    Version& operator=(const Version& that) noexcept {
        this->verElems_ = that.verElems_;
        return *this;
    }

    int compare(const Version& that) {
        assert(isSameFormat(that));

        for (size_t i = 0; i < verElems_.size(); i++) {
            if (verElems_[i] > that.verElems_[i])
                return 1;

            if (verElems_[i] < that.verElems_[i])
                return -1;
        }
        return 0;
    }

   protected:
    std::vector<unsigned int> verElems_;
};
}  // namespace jhc

#endif  // !JHC_VERSION_HPP__