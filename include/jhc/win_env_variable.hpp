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

#include "jhc/arch.hpp"
#ifdef JHC_WIN
#include <string>
#include <limits>
#include <assert.h>
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

#ifdef max
#undef max
#endif

namespace jhc {
namespace {
UINT const kBroadcastTimeout = 100;  // in milliseconds
wchar_t const* userEnvSubKey = L"Environment";
wchar_t const* systemEnvSubKey =
    L"System\\CurrentControlSet\\Control\\"
    L"Session Manager\\Environment";
}  // namespace

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
    EnvVar(EnvScope scope, std::wstring const& name) :
        name_(name),
        scope_(es_invalid) {
        PBYTE data = NULL;
        HKEY key = NULL;
        DWORD size = 0;
        LONG status = 0;
        HKEY subKey = NULL;
        wchar_t const* subKeyName;

        switch (scope) {
            case es_system:
                key = HKEY_LOCAL_MACHINE;
                subKeyName = systemEnvSubKey;
                break;

            case es_user:
                key = HKEY_CURRENT_USER;
                subKeyName = userEnvSubKey;
                break;

            default:
                assert(false);
                return;
        }
        scope_ = scope;

        RegOpenKeyExW(key, subKeyName, 0, KEY_QUERY_VALUE, &subKey);
        status = RegQueryValueExW(subKey, name_.c_str(), 0, NULL, NULL, &size);
        if (ERROR_SUCCESS != status) {
            // This environment variable doesn't exist. Assign this EnvVar object's
            // value the empty string.
            value_ = L"";
        }
        else {
            // This environment variable already exists. Assign its value to this EnvVar object.
            data = new BYTE[size + 1]();
            RegQueryValueExW(subKey, name_.c_str(), 0, NULL, data, &size);
            value_ = reinterpret_cast<wchar_t const*>(data);
            delete[] data;
        }
        RegCloseKey(key);
    }

    EnvVar(EnvVar const& other) {
        copy(other);
    }

    ~EnvVar() {
        destroy();
    }

    EnvVar& operator=(EnvVar const& other) {
        if (this != &other) {
            destroy();
            copy(other);
        }

        return *this;
    }

    // Removes all matching instance of the specified text from the environment
    // variable's value.
    //
    // text [in]    Text to cut from the variable's value.
    //
    // Return Value: Returns the number of matching instances that were cut.
    unsigned int cut(std::wstring const& text) {
        size_t const sizeMax = std::numeric_limits<size_t>::max();

        unsigned int count = 0;
        HKEY key;
        size_t length;
        size_t pos;
        HKEY subKey;
        wchar_t const* subKeyName;

        if (es_invalid == scope_) {
            return 0;
        }

        switch (scope_) {
            case es_system:
                key = HKEY_LOCAL_MACHINE;
                subKeyName = systemEnvSubKey;
                break;

            case es_user:
                key = HKEY_CURRENT_USER;
                subKeyName = userEnvSubKey;
                break;
        }

        // Replace every instance of text with the empty string.
        pos = value_.find(text);
        length = text.length();
        while (sizeMax != pos) {
            ++count;
            value_.replace(pos, length, L"");
            pos = value_.find(text);
        }

        // Write the new value to the registry.
        RegOpenKeyExW(key, subKeyName, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &subKey);
        RegSetValueExW(subKey,
                       name_.c_str(),
                       0,
                       REG_EXPAND_SZ,
                       reinterpret_cast<BYTE const*>(value_.c_str()),
                       value_.length() * sizeof(wchar_t) + 1);
        RegCloseKey(key);

        // Notify everyone of the change.
        broadcastChange();

        return count;
    }

    // Appends the specified text to the variable's current value.
    //
    // text [in]    Text to append to the variable's value.
    //
    // Return Value: Nothing.
    void paste(std::wstring const& text) {
        HKEY key;
        HKEY subKey;
        wchar_t const* subKeyName;

        if (es_invalid == scope_) {
            return;
        }

        switch (scope_) {
            case es_system:
                key = HKEY_LOCAL_MACHINE;
                subKeyName = systemEnvSubKey;
                break;

            case es_user:
                key = HKEY_CURRENT_USER;
                subKeyName = userEnvSubKey;
                break;
        }

        // Append text to the current value.
        value_ += text;

        // Write the new value to the registry.
        RegOpenKeyExW(key, subKeyName, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &subKey);
        RegSetValueExW(subKey,
                       name_.c_str(),
                       0,
                       REG_EXPAND_SZ,
                       reinterpret_cast<BYTE const*>(value_.c_str()),
                       value_.length() * sizeof(wchar_t) + 1);
        RegCloseKey(key);

        // Notify everyone of the change.
        broadcastChange();
    }

    // Assigns the specified text as the variable's value. Creats the variable
    // if it does not yet exist in the environment.
    //
    // text [in]    Text to assign as the variable's value.
    void set(std::wstring const& text) {
        HKEY key;
        HKEY subKey;
        wchar_t const* subKeyName;

        if (es_invalid == scope_) {
            return;
        }

        switch (scope_) {
            case es_system:
                key = HKEY_LOCAL_MACHINE;
                subKeyName = systemEnvSubKey;
                break;

            case es_user:
                key = HKEY_CURRENT_USER;
                subKeyName = userEnvSubKey;
                break;
        }

        // Assign the new value.
        value_ = text;

        // Write the new value to the registry.
        RegOpenKeyExW(key, subKeyName, 0, KEY_SET_VALUE, &subKey);
        RegSetValueExW(subKey,
                       name_.c_str(),
                       0,
                       REG_EXPAND_SZ,
                       reinterpret_cast<const BYTE*>(value_.c_str()),
                       value_.length() * sizeof(wchar_t) + 1);
        RegCloseKey(key);

        // Notify everyone of the change.
        broadcastChange();
    }

    // Deletes the variable from the environment.
    void unset() {
        HKEY key;
        HKEY subKey;
        wchar_t const* subKeyName;

        if (es_invalid == scope_) {
            return;
        }

        switch (scope_) {
            case es_system:
                key = HKEY_LOCAL_MACHINE;
                subKeyName = systemEnvSubKey;
                break;

            case es_user:
                key = HKEY_CURRENT_USER;
                subKeyName = userEnvSubKey;
                break;
        }

        // Assign the empty string for the EnvVar object's value.
        value_ = L"";

        // Delete the value from the registry.
        RegOpenKeyExW(key, subKeyName, 0, KEY_SET_VALUE, &subKey);
        RegDeleteValueW(subKey, name_.c_str());
        RegCloseKey(key);

        // Notify everyone of the change.
        broadcastChange();
    }

    std::wstring value() const {
        return value_;
    }

   private:
    void broadcastChange() const {
        DWORD_PTR result = 0;
        SendMessageTimeout(HWND_BROADCAST,
                           WM_SETTINGCHANGE,
                           NULL,
                           reinterpret_cast<LPARAM>("Environment"),
                           SMTO_NORMAL,
                           kBroadcastTimeout,
                           &result);
    }

    void copy(EnvVar const& other) {
        name_ = other.name_;
        scope_ = other.scope_;
        value_ = other.value_;
    }

    void destroy() {
    }

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
    static unsigned int EnvCut(EnvScope scope, wchar_t const* name, wchar_t const* value) {
        EnvVar var(scope, name);
        return var.cut(value);
    }

    // Appends the specified value to the name environment variable's value.
    //
    // scope [in]    Environment scope (user environment or system environment).
    //
    // name  [in]    Name of the variable to edit.
    //
    // value [in]    Value to paste to at the end of the variable's value.
    //
    // Return Value: Nothing.
    static void EnvPaste(EnvScope scope, wchar_t const* name, wchar_t const* value) {
        EnvVar var(scope, name);
        var.paste(value);
    }

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
    static void EnvSet(EnvScope scope, wchar_t const* name, wchar_t const* value) {
        EnvVar var(scope, name);
        var.set(value);
    }

    // Deletes the named environment variable.
    //
    // scope [in]    Environment scope (user environment or system environment).
    //
    // name  [in]    Name of the variable to delete.
    //
    // Return Value: Nothing.
    static void EnvUnset(EnvScope scope, wchar_t const* name) {
        EnvVar var(scope, name);
        var.unset();
    }

    // Retrieves the value currently assigned to the named variable.
    //
    // scope [in]    Environment scope (user environment or system environment).
    //
    // name  [in]    Name of the variable whose value to retrieve.
    //
    // Return Value: Returns a const string containing the variable's value.
    static std::wstring QueryValue(EnvScope scope, wchar_t const* name) {
        EnvVar var(scope, name);
        return var.value();
    }

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
    static void PathAdd(EnvScope scope, wchar_t const* path) {
        size_t const sizeMax = std::numeric_limits<size_t>::max();

        size_t length = std::wstring(path).length();
        size_t pos;
        std::wstring value;
        EnvVar var(scope, L"Path");

        value = var.value();
        pos = value.find(path, 0);
        while (sizeMax != pos) {
            if (((0 == pos) ||
                 (L';' == value[pos - 1])) &&
                ((pos + length == value.length()) ||
                 (L';' == value[pos + length]))) {
                // Found the path in the "Path" environment variable already.
                return;
            }
            pos = value.find(path, pos + 1);
        }

        if (0 == var.value().length()) {
            // Nothing is in the Path environment variable yet.
            var.set(path);
        }
        else {
            var.paste(std::wstring(L";") + path);
        }
    }

    // Removes all matching instances of the specified path from the Path
    // environment variable.
    //
    // scope [in]    Environment scope (user path or system path).
    //
    // path  [in]    Path to remove from the Path environment variable.
    //
    // Return value: Returns the number of matching instances that were removed.
    static unsigned int PathRemove(EnvScope scope, wchar_t const* path) {
        size_t const sizeMax = std::numeric_limits<size_t>::max();

        unsigned int count = 0;
        size_t length = std::wstring(path).length();
        size_t pos;
        std::wstring value;
        EnvVar var(scope, L"Path");

        value = var.value();
        pos = value.find(path);
        while (sizeMax != pos) {
            if (((0 == pos) ||
                 (L';' == value[pos - 1])) &&
                ((pos + length == value.length()) ||
                 (L';' == value[pos + length]))) {
                // Found a match in the path environment variable.
                ++count;
                if (0 == pos) {
                    // This is the first directory in the path, so there is no
                    // preceding semicolon to remove.
                    if (length == value.length()) {
                        // There is no trailing semicolon, either because this is
                        // the only directory in the path.
                        value = L"";
                    }
                    else {
                        // Instead of removing the preceding semicolon (which
                        // isn't there), remove the following semicolon so that the
                        // new path value doesn't begin with a semicolon.
                        value.replace(pos, length + 1, L"");
                    }
                }
                else {
                    // Remove the preceding semicolon along with the path string.
                    value.replace(pos - 1, length + 1, L"");
                }
                pos = value.find(path, pos);
            }
            else {
                pos = value.find(path, pos + 1);
            }
        }

        // Set the new path environment variable.
        var.set(value);

        return count;
    }
};
}  // namespace jhc
#endif
#endif