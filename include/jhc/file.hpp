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

#ifndef JHC_FILE_UTIL_HPP_
#define JHC_FILE_UTIL_HPP_
#include "jhc/arch.hpp"
#include <stdio.h>
#include <mutex>
#include <string>
#include "jhc/macros.hpp"
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include "jhc/filesystem.hpp"

namespace jhc {
class File {
   public:
    JHC_DISALLOW_COPY_MOVE(File);

    File(const fs::path& path);
    File(fs::path&& path);

    virtual ~File();

    fs::path path() const;

    bool isOpen();

    // [r]  read
    //      Open file for input operations.
    //      The file must exist.
    //
    // [w]  write
    //      Create an empty file for output operations.
    //      If a file with the same name already exists, its contents are discarded and the file is treated as a new empty file.
    //
    // [a]  append
    //      Open file for output at the end of a file.
    //      Output operations always write data at the end of the file, expanding it.
    //      Repositioning operations (fseek, fsetpos, rewind) are ignored.
    //      The file is created if it does not exist.
    //
    // [r+] read/update
    //      Open a file for update (both for input and output).
    //      The file must exist.
    //
    // [w+] write/update
    //      Create an empty file and open it for update (both for input and output).
    //      If a file with the same name already exists its contents are discarded and the file is treated as a new empty file.
    //
    // [a+] append/update
    //      Open a file for update (both for input and output) with all output operations writing data at the end of the file.
    //      Repositioning operations (fseek, fsetpos, rewind) affects the next input operations,
    //          but output operations move the position back to the end of file.
    //      The file is created if it does not exist.
    //
    // [b]  binary mode
    //      With the mode specifiers above the file is open as a text file.
    //      In order to open a file as a binary file, a "b" character has to be included in the mode string.
    //      This additional "b" character can either be appended at the end of the string ("rb", "wb", "ab", "r+b", "w+b", "a+b")
    //         or be inserted between the letter and the "+" sign for the mixed modes ("rb+", "wb+", "ab+").
    //
    // see: https://www.cplusplus.com/reference/cstdio/fopen/
    //
    bool open(const fs::path& openMode);

    bool close();

    bool flush();

    bool exist() const;

    bool canRW() const;

    // Must be call open(...) first!
    // This function will NOT change file pointer position.
    // Return: < 0 failed
    //
    int64_t fileSize();

    // Must be call open(...) first!
    bool seekFromCurrent(int64_t offset);

    // Return: -1 is failed
    int64_t currentPointerPos();

    // Must be call open(...) first!
    bool seekFromBeginning(int64_t offset);

    // Must be call open(...) first!
    bool seekFromEnd(int64_t offset);

    // Must be call open(...) first!
    // This function will change file pointer position.
    // Caller need allocate/free buffer.
    size_t readFrom(void* buffer, size_t needRead, int64_t from = -1);

    // Must be call open(...) first!
    // This function will change file pointer position.
    // Caller need allocate/free buffer.
    size_t writeFrom(const void* buffer, size_t needWrite, int64_t from = -1);

    // Must be call open(...) first!
    // This function will NOT change file pointer position.
    // readAll function will malloc memory, caller need free it.
    size_t readAll(void** buffer);

    // Must be call open(...) first!
    // This function will NOT change file pointer position.
    std::string readAll();

    // Must be call open(...) first!
    // This function will NOT change file pointer position.
    bool readAll(std::string& ret);

   protected:
    FILE* f_ = nullptr;
    jhc::fs::path path_;
    std::recursive_mutex mutex_;
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/file.cc"
#endif
#endif  // !JHC_FILE_UTIL_HPP_
