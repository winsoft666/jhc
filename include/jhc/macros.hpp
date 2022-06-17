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

#ifndef JHC_MACROS_HPP__
#define JHC_MACROS_HPP__

#include <float.h>

// Compare two float point value
#ifndef IS_NEARLY_EQUAL
#define IS_NEARLY_EQUAL(x, y) (fabs((x) - (y)) < FLT_EPSILON)
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(p)       \
    do {                   \
        if ((p) != NULL) { \
            free(p);       \
            (p) = NULL;    \
        }                  \
    } while (false)
#endif

#ifndef SAFE_CLOSE_ON_NONULL
#define SAFE_CLOSE(p)         \
    do {                      \
        if ((p) != NULL) {    \
            CloseHandle((p)); \
            (p) = NULL;       \
        }                     \
    } while (false)
#endif

#ifndef SAFE_CLOSE_ON_VALID_HANDLE
#define SAFE_CLOSE_ON_VALID_HANDLE(p)      \
    do {                                   \
        if ((p) != INVALID_HANDLE_VALUE) { \
            CloseHandle((p));              \
            (p) = INVALID_HANDLE_VALUE;    \
        }                                  \
    } while (false)
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) \
    do {                     \
        if ((p) != NULL) {   \
            delete[](p);     \
            (p) = NULL;      \
        }                    \
    } while (false)
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)     \
    do {                   \
        if ((p) != NULL) { \
            delete (p);    \
            (p) = NULL;    \
        }                  \
    } while (false)
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)     \
    do {                    \
        if ((p)) {          \
            (p)->Release(); \
            (p) = NULL;     \
        }                   \
    } while (false)
#endif

#ifndef JHC_DISALLOW_MOVE
#define JHC_DISALLOW_MOVE(TypeName)      \
    TypeName(const TypeName&&) = delete; \
    TypeName& operator=(const TypeName&&) = delete
#endif  // !JHC_DISALLOW_MOVE

#ifndef JHC_DISALLOW_COPY
#define JHC_DISALLOW_COPY(TypeName)     \
    TypeName(const TypeName&) = delete; \
    TypeName& operator=(const TypeName&) = delete
#endif  // !JHC_DISALLOW_COPY

#ifndef JHC_DISALLOW_COPY_MOVE
#define JHC_DISALLOW_COPY_MOVE(TypeName) \
    JHC_DISALLOW_MOVE(TypeName);         \
    JHC_DISALLOW_COPY(TypeName)
#endif  // !JHC_DISALLOW_COPY_MOVE

#ifndef IS_FLAG_SET
#define IS_FLAG_SET(var, flag) (((var) & (flag)) == (flag))
#endif  // !IS_FLAG_SET

#endif  // ! JHC_MACROS_HPP__
