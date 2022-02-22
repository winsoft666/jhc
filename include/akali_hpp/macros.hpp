/*******************************************************************************
 * Copyright (C) 2021 - 2026, winsoft666, <winsoft666@outlook.com>.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Expect bugs
 *
 * Please use and enjoy. Please let me know of any bugs/improvements
 * that you have found/implemented and I will fix/incorporate them into this
 * file.
 *******************************************************************************/

#ifndef AKALI_MACROS_HPP__
#define AKALI_MACROS_HPP__

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

#ifndef AKALI_DISALLOW_COPY
// Put this in the declarations for a class to be uncopyable.
#define AKALI_DISALLOW_COPY(TypeName) TypeName(const TypeName&) = delete
#endif

#ifndef AKALI_DISALLOW_ASSIGN
// Put this in the declarations for a class to be unassignable.
#define AKALI_DISALLOW_ASSIGN(TypeName) void operator=(const TypeName&) = delete
#endif

#ifndef AKALI_DISALLOW_COPY_AND_ASSIGN
// A macro to disallow the copy constructor and operator= functions. This should
// be used in the declarations for a class.
#define AKALI_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;            \
  AKALI_DISALLOW_ASSIGN(TypeName)
#endif

#ifndef AKALI_DISALLOW_IMPLICIT_CONSTRUCTORS
// A macro to disallow all the implicit constructors, namely the default
// constructor, copy constructor and operator= functions.
//
// This should be used in the declarations for a class that wants to prevent
// anyone from instantiating it. This is especially useful for classes
// containing only static methods.
#define AKALI_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName() = delete;                                 \
  AKALI_DISALLOW_COPY_AND_ASSIGN(TypeName)
#endif

#endif  // ! AKALI_MACROS_HPP__
