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

#ifndef JHC_WIN_ENV_VARIABLE_HPP__
#define JHC_WIN_ENV_VARIABLE_HPP__
#pragma once

#include "jhc/config.hpp"
#include "jhc/arch.hpp"
#ifdef JHC_WIN
#include <string>

namespace jhc {

// environment variable scopes:
enum EnvScope {
    es_invalid,  // Unspecified (do not use this value)
    es_system,   // System environment variables
    es_user      // Current user's environment variables
};

class EnvVar {
   public:
    // Constructs an environment variable object with the given name. Merely
    // constructing an object does not create a corresponding variable in the
    // specified environment. To actually create the environment variable
    // (assuming it does not already exist), use EnvVar::set.
    //
    EnvVar(EnvScope scope, std::wstring const& name);

    EnvVar(EnvVar const& other);

    ~EnvVar();

    EnvVar& operator=(EnvVar const& other);

    // Removes all matching instance of the specified text from the environment
    // variable's value.
    //
    // text [in]    Text to cut from the variable's value.
    //
    // Return Value: Returns the number of matching instances that were cut.
    unsigned int cut(std::wstring const& text);

    // Appends the specified text to the variable's current value.
    //
    // text [in]    Text to append to the variable's value.
    //
    // Return Value: Nothing.
    void paste(std::wstring const& text);

    // Assigns the specified text as the variable's value. Creats the variable
    // if it does not yet exist in the environment.
    //
    // text [in]    Text to assign as the variable's value.
    void set(std::wstring const& text);

    // Deletes the variable from the environment.
    void unset();

    std::wstring value() const;

   private:
    void broadcastChange() const;

    void copy(EnvVar const& other);

    void destroy();
   private:
    std::wstring name_;   // The environment variable's name.
    EnvScope scope_;      // Scope of the environment variable (user or system).
    std::wstring value_;  // The environment variable's value.
};

class EnvHelper {
   public:
    // Removes all matching instances of the specified value from the named
    // environment variable's value.
    //
    // scope [in]    Environment scope (user environment or system environment).
    //
    // name  [in]    Name of the variable to edit.
    //
    // value [in]    String to match against and cut from the variable.
    //
    // Return Value: Returns the number of matching instances within the variable's value that were cut.
    static unsigned int EnvCut(EnvScope scope, wchar_t const* name, wchar_t const* value);

    // Appends the specified value to the name environment variable's value.
    //
    // scope [in]    Environment scope (user environment or system environment).
    //
    // name  [in]    Name of the variable to edit.
    //
    // value [in]    Value to paste to at the end of the variable's value.
    //
    // Return Value: Nothing.
    static void EnvPaste(EnvScope scope, wchar_t const* name, wchar_t const* value);

    // Sets the named environment variable's value to the specified value. Creates
    // the variable if it does not yet exist in the environment.
    //
    // scope [in]    Environment scope (user environment or system environment).
    //
    // name  [in]    Name of the variable to set.
    //
    // value [in]    Value to be assigned to the variable.
    //
    // Return Value: Nothing.
    static void EnvSet(EnvScope scope, wchar_t const* name, wchar_t const* value);

    // Deletes the named environment variable.
    //
    // scope [in]    Environment scope (user environment or system environment).
    //
    // name  [in]    Name of the variable to delete.
    //
    // Return Value: Nothing.
    static void EnvUnset(EnvScope scope, wchar_t const* name);

    // Retrieves the value currently assigned to the named variable.
    //
    // scope [in]    Environment scope (user environment or system environment).
    //
    // name  [in]    Name of the variable whose value to retrieve.
    //
    // Return Value: Returns a const string containing the variable's value.
    static std::wstring QueryValue(EnvScope scope, wchar_t const* name);

    // Appends the specified path to the Path environment variable. Only adds to
    // the Path environment variable if the specified path is not already in the
    // Path environment variable (i.e. calling this function will not add duplicate
    // entries to the Path environment variable).
    //
    // scope [in]    Environment scope (user path or system path).
    //
    // path  [in]    Path to append to the Path environment variable.
    //
    // Return Value: Nothing.
    static void PathAdd(EnvScope scope, wchar_t const* path);

    // Removes all matching instances of the specified path from the Path
    // environment variable.
    //
    // scope [in]    Environment scope (user path or system path).
    //
    // path  [in]    Path to remove from the Path environment variable.
    //
    // Return value: Returns the number of matching instances that were removed.
    static unsigned int PathRemove(EnvScope scope, wchar_t const* path);
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/win_env_variable.cc"
#endif
#endif
#endif