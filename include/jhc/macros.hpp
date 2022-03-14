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
#define SAFE_FREE(p)   \
  do {                 \
    if ((p) != NULL) { \
      free(p);         \
      (p) = NULL;      \
    }                  \
  } while (false)
#endif

#ifndef SAFE_CLOSE_ON_NONULL
#define SAFE_CLOSE(p)   \
  do {                  \
    if ((p) != NULL) {  \
      CloseHandle((p)); \
      (p) = NULL;       \
    }                   \
  } while (false)
#endif

#ifndef SAFE_CLOSE_ON_VALID_HANDLE
#define SAFE_CLOSE_ON_VALID_HANDLE(p)  \
  do {                                 \
    if ((p) != INVALID_HANDLE_VALUE) { \
      CloseHandle((p));                \
      (p) = INVALID_HANDLE_VALUE;      \
    }                                  \
  } while (false)
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) \
  do {                       \
    if ((p) != NULL) {       \
      delete[](p);           \
      (p) = NULL;            \
    }                        \
  } while (false)
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) \
  do {                 \
    if ((p) != NULL) { \
      delete (p);      \
      (p) = NULL;      \
    }                  \
  } while (false)
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) \
  do {                  \
    if ((p)) {          \
      (p)->Release();   \
      (p) = NULL;       \
    }                   \
  } while (false)
#endif

#ifndef JHC_DISALLOW_COPY
// Put this in the declarations for a class to be uncopyable.
#define JHC_DISALLOW_COPY(TypeName) TypeName(const TypeName&) = delete
#endif

#ifndef JHC_DISALLOW_ASSIGN
// Put this in the declarations for a class to be unassignable.
#define JHC_DISALLOW_ASSIGN(TypeName) void operator=(const TypeName&) = delete
#endif

#ifndef JHC_DISALLOW_COPY_AND_ASSIGN
// A macro to disallow the copy constructor and operator= functions. This should
// be used in the declarations for a class.
#define JHC_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;            \
  JHC_DISALLOW_ASSIGN(TypeName)
#endif

#ifndef JHC_DISALLOW_IMPLICIT_CONSTRUCTORS
// A macro to disallow all the implicit constructors, namely the default
// constructor, copy constructor and operator= functions.
//
// This should be used in the declarations for a class that wants to prevent
// anyone from instantiating it. This is especially useful for classes
// containing only static methods.
#define JHC_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName() = delete;                                 \
  JHC_DISALLOW_COPY_AND_ASSIGN(TypeName)
#endif

#endif  // ! JHC_MACROS_HPP__
