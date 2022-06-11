#ifdef JHC_NOT_HEADER_ONLY
#include "../win_env_variable.hpp"
#endif

#ifdef JHC_WIN
#include <limits>
#include <assert.h>
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
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

EnvVar::EnvVar(EnvScope scope, std::wstring const& name) :
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

EnvVar::EnvVar(EnvVar const& other) {
    copy(other);
}

EnvVar::~EnvVar() {
    destroy();
}

EnvVar& EnvVar::operator=(EnvVar const& other) {
    if (this != &other) {
        destroy();
        copy(other);
    }

    return *this;
}

unsigned int EnvVar::cut(std::wstring const& text) {
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

void EnvVar::paste(std::wstring const& text) {
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

void EnvVar::set(std::wstring const& text) {
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
void EnvVar::unset() {
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

std::wstring EnvVar::value() const {
    return value_;
}

void EnvVar::broadcastChange() const {
    DWORD_PTR result = 0;
    SendMessageTimeout(HWND_BROADCAST,
                       WM_SETTINGCHANGE,
                       NULL,
                       reinterpret_cast<LPARAM>("Environment"),
                       SMTO_NORMAL,
                       kBroadcastTimeout,
                       &result);
}

void EnvVar::copy(EnvVar const& other) {
    name_ = other.name_;
    scope_ = other.scope_;
    value_ = other.value_;
}

void EnvVar::destroy() {
}

unsigned int EnvHelper::EnvCut(EnvScope scope, wchar_t const* name, wchar_t const* value) {
    EnvVar var(scope, name);
    return var.cut(value);
}

void EnvHelper::EnvPaste(EnvScope scope, wchar_t const* name, wchar_t const* value) {
    EnvVar var(scope, name);
    var.paste(value);
}

void EnvHelper::EnvSet(EnvScope scope, wchar_t const* name, wchar_t const* value) {
    EnvVar var(scope, name);
    var.set(value);
}

void EnvHelper::EnvUnset(EnvScope scope, wchar_t const* name) {
    EnvVar var(scope, name);
    var.unset();
}

std::wstring EnvHelper::QueryValue(EnvScope scope, wchar_t const* name) {
    EnvVar var(scope, name);
    return var.value();
}

void EnvHelper::PathAdd(EnvScope scope, wchar_t const* path) {
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

unsigned int EnvHelper::PathRemove(EnvScope scope, wchar_t const* path) {
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
}  // namespace jhc
#endif