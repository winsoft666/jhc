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
#ifndef JHC_SINGLETON_PROCESS_HPP__
#define JHC_SINGLETON_PROCESS_HPP__
#include "jhc/config.hpp"
#include "jhc/arch.hpp"
#include <string>
#include "jhc/singleton_class.hpp"

namespace jhc {
class SingletonProcess : public SingletonClass<SingletonProcess> {
   public:
    void markAndCheckStartup(const std::string& uniqueName);

    const std::string& uniqueName() const;

    bool isPrimary() const;

   protected:
    void check();

   private:
    ~SingletonProcess();

    std::string uniqueName_;
    bool isPrimary_ = true;
#ifdef JHC_WIN
    void* mutex_ = NULL;
#else
    int pidFile_ = -1;
#endif
    JHC_SINGLETON_CLASS_DECLARE(SingletonProcess);
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/singleton_process.cc"
#endif
#endif  // !JHC_SINGLETON_PROCESS_HPP__
