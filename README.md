# akali_hpp
A header-only C++ common library that don't need install any third party library.

# Features
- arch.hpp `[Windows]` `[Linux]` `[MacOS]`

    OS type check macros.

    Little/Big endian check function.

- criticalsection.hpp `[Windows]`

    Windows CriticalSection wrapper.

- driver_info.hpp `[Windows]`

    Get windows driver information.

- event.hpp `[Windows]` `[Linux]` `[MacOS]`

    A event implement similay with Windows Event.

- file_info.hpp `[Windows]`

    Get file meta data.

- file_util.hpp `[Windows]` `[Linux]` `[MacOS]`

    Wrap file operation.

- ini.hpp `[Windows]`

    Read/Write ini file.

- macros.hpp `[Windows]` `[Linux]` `[MacOS]`

    Safe release macros.

    Copy/Assign disable macros.

- md5.hpp `[Windows]` `[Linux]` `[MacOS]`

    Calculate file/string md5.

- memory_pool.hpp `[Windows]` `[Linux]` `[MacOS]`

    Memory pool.

- os_ver.hpp `[Windows]`

    Get OS version.

- process_finder.hpp `[Windows]`

    Find process by id, name.

- process_util.hpp `[Windows]` `[Linux]` `[MacOS]`

- registry.hpp `[Windows]`

    Read/Write/Delete windows registry.

- schedule_task.hpp `[Windows]`

- singleton.hpp `[Windows]` `[Linux]` `[MacOS]`

    Class/Process singleton.

- stringencode.hpp `[Windows]` `[Linux]` `[MacOS]`

    Url/Hex encode/decode.

    Unicode <--> ANSI, Unicode <--> UTF8, ANSI <--> UTF8.

- string_helper.hpp `[Windows]` `[Linux]` `[MacOS]`

    String upper/lower case, trim, equal, contains, split, join, replace.

- thread.hpp `[Windows]` `[Linux]` `[MacOS]`

    Wrapper with std::async, support invoke function into thread, set thread name.

- thread_pool.hpp `[Windows]` `[Linux]` `[MacOS]`

    Thread pool.

- timer.hpp `[Windows]` `[Linux]` `[MacOS]`

    Timer.

- time_util.hpp `[Windows]` `[Linux]` `[MacOS]`

    Timestamp/UTC Time getter, time meter.

- trace.hpp `[Windows]` `[Linux]` `[MacOS]`

    Debug output.

- win_env_variable.hpp `[Windows]`

    Windows Environment variable editer.

- win_exception_catch.hpp `[Windows]`

    Windows process exception dumper.

- win_service_base.hpp `[Windows]`

    Windows service base class.

- win_service_installer.hpp `[Windows]`

    Install/Uninstall windows service.

- xorstr.hpp `[Windows]` `[Linux]` `[MacOS]`

    Compile time string encryption.