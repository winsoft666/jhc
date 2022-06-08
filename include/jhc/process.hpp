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

#ifndef JHC_PROCESS_HPP__
#define JHC_PROCESS_HPP__

#include "jhc/arch.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#ifdef JHC_WIN
#include <Shlwapi.h>
#include <TlHelp32.h>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>
#include <cstring>
#include <stdexcept>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")
#else
#include <sys/wait.h>
#include <algorithm>
#include <bitset>
#include <cstdlib>
#include <fcntl.h>
#include <poll.h>
#include <set>
#include <signal.h>
#include <stdexcept>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <iosfwd>
#include <limits.h>
#endif
#include "jhc/macros.hpp"
#include "jhc/scoped_object.hpp"

namespace jhc {
/// Platform independent class for creating processes.
/// Note on Windows: it seems not possible to specify which pipes to redirect.
/// Thus, at the moment, if read_stdout==nullptr, read_stderr==nullptr and
/// open_stdin==false, the stdout, stderr and stdin are sent to the parent
/// process instead.
//
class Process {
   public:
    /// Additional parameters to Process constructors.
    struct Config {
        /// Buffer size for reading stdout and stderr. Default is 131072 (128 kB).
        std::size_t buffer_size;

        /// Set to true to inherit file descriptors from parent process. Default is
        /// false. On Windows: has no effect unless read_stdout==nullptr,
        /// read_stderr==nullptr and open_stdin==false.
        bool inherit_file_descriptors;

        Config() :
            buffer_size(131072),
            inherit_file_descriptors(false) {
        }
    };
#ifdef JHC_WIN
    typedef unsigned long id_type;  // Process id type
    typedef void* fd_type;          // File descriptor type
    typedef std::wstring string_type;
#else
    typedef pid_t id_type;
    typedef int fd_type;
    typedef std::string string_type;
#endif
    typedef std::unordered_map<string_type, string_type> environment_type;

   private:
    class Data {
       public:
        Data() noexcept;
        id_type id;
#ifdef JHC_WIN
        void* handle;
#else
        int exit_status{-1};
#endif
    };

   public:
    /// Starts a process.
    Process(const std::vector<string_type>& arguments,
            const string_type& current_folder = string_type(),
            std::function<void(const char* bytes, size_t n)> read_stdout = nullptr,
            std::function<void(const char* bytes, size_t n)> read_stderr = nullptr,
            bool open_stdin = false,
            const Config& config = {}) noexcept;

    /// Starts a process.
    Process(const string_type& command,
            const string_type& current_folder = string_type(),
            std::function<void(const char* bytes, size_t n)> read_stdout = nullptr,
            std::function<void(const char* bytes, size_t n)> read_stderr = nullptr,
            bool open_stdin = false,
            const Config& config = {}) noexcept;

    /// Starts a process with specified environment.
    Process(const std::vector<string_type>& arguments,
            const string_type& current_folder,
            const environment_type& environment,
            std::function<void(const char* bytes, size_t n)> read_stdout = nullptr,
            std::function<void(const char* bytes, size_t n)> read_stderr = nullptr,
            bool open_stdin = false,
            const Config& config = {}) noexcept;

    /// Starts a process with specified environment.
    Process(const string_type& command,
            const string_type& current_folder,
            const environment_type& environment,
            std::function<void(const char* bytes, size_t n)> read_stdout = nullptr,
            std::function<void(const char* bytes, size_t n)> read_stderr = nullptr,
            bool open_stdin = false,
            const Config& config = {}) noexcept;

#ifndef JHC_WIN
    /// Starts a process with the environment of the calling process.
    /// Supported on Unix-like systems only.
    Process(const std::function<void()>& function,
            std::function<void(const char* bytes, size_t n)> read_stdout = nullptr,
            std::function<void(const char* bytes, size_t n)> read_stderr = nullptr,
            bool open_stdin = false,
            const Config& config = {}) noexcept;
#endif
    ~Process() noexcept;

    /// Get the process id of the started process.
    id_type getId() const noexcept;

    bool successed() const noexcept;

    /// Wait until process is finished, and return exit status.
    int getExitStatus() noexcept;

    /// If process is finished, returns true and sets the exit status. Returns
    /// false otherwise.
    bool tryGetExitStatus(int& exit_status) noexcept;

    /// Write to stdin.
    bool write(const char* bytes, size_t n);

    /// Write to stdin. Convenience function using write(const char *, size_t).
    bool write(const std::string& data);

    /// Close stdin. If the process takes parameters from stdin, use this to
    /// notify that all parameters have been sent.
    void closeStdin() noexcept;

    /// Kill the process. force=true is only supported on Unix-like systems.
    bool kill(bool force = false) noexcept;

    /// Kill the process and it's child processes. force=true is only supported on Unix-like systems.
    void killProcessTree(bool force = false) noexcept;

    /// Kill a given process id. Use KillProcessTree(bool force) instead if
    /// possible. force=true is only supported on Unix-like systems.
    static void KillProcessTree(id_type id, bool force = false) noexcept;

    /// Kill a given process id.
    /// force=true is only supported on Unix-like systems.
    static bool Kill(id_type id, bool force = false) noexcept;

   private:
    Data data_;
    bool closed_;
    std::mutex close_mutex_;
    std::function<void(const char* bytes, size_t n)> read_stdout_;
    std::function<void(const char* bytes, size_t n)> read_stderr_;
#ifndef JHC_WIN
    std::thread stdout_stderr_thread_;
#else
    std::thread stdout_thread_;
    std::thread stderr_thread_;
    // Based on the discussion thread:
    // https://www.reddit.com/r/cpp/comments/3vpjqg/a_new_platform_independent_process_library_for_c11/cxq1wsj
    static std::mutex create_process_mutex;
#endif
    bool open_stdin_;
    std::mutex stdin_mutex_;

    Config config_;

    std::unique_ptr<fd_type> stdout_fd_;
    std::unique_ptr<fd_type> stderr_fd_;
    std::unique_ptr<fd_type> stdin_fd_;

    id_type open(const std::vector<string_type>& arguments,
                 const string_type& current_folder,
                 const environment_type* environment = nullptr) noexcept;
    id_type open(const string_type& command,
                 const string_type& current_folder,
                 const environment_type* environment = nullptr) noexcept;
#ifndef JHC_WIN
    id_type open(const std::function<void()>& function) noexcept;
#endif
    void async_read() noexcept;
    void close_fds() noexcept;

    JHC_DISALLOW_COPY_MOVE(Process);
};

#ifdef JHC_WIN
std::mutex Process::create_process_mutex;
#endif

/////////////////////////////////////////////////////////////////
/// Implement
/////////////////////////////////////////////////////////////////

inline Process::Process(const std::vector<string_type>& arguments,
                        const string_type& current_folder,
                        std::function<void(const char* bytes, size_t n)> read_stdout,
                        std::function<void(const char* bytes, size_t n)> read_stderr,
                        bool open_stdin,
                        const Config& config) noexcept
    :
    closed_(true), read_stdout_(std::move(read_stdout)), read_stderr_(std::move(read_stderr)), open_stdin_(open_stdin), config_(config) {
    open(arguments, current_folder);
    async_read();
}

inline Process::Process(const string_type& command,
                        const string_type& current_folder,
                        std::function<void(const char* bytes, size_t n)> read_stdout,
                        std::function<void(const char* bytes, size_t n)> read_stderr,
                        bool open_stdin,
                        const Config& config) noexcept
    :
    closed_(true), read_stdout_(std::move(read_stdout)), read_stderr_(std::move(read_stderr)), open_stdin_(open_stdin), config_(config) {
    open(command, current_folder);
    async_read();
}

inline Process::Process(const std::vector<string_type>& arguments,
                        const string_type& current_folder,
                        const environment_type& environment,
                        std::function<void(const char* bytes, size_t n)> read_stdout,
                        std::function<void(const char* bytes, size_t n)> read_stderr,
                        bool open_stdin,
                        const Config& config) noexcept
    :
    closed_(true), read_stdout_(std::move(read_stdout)), read_stderr_(std::move(read_stderr)), open_stdin_(open_stdin), config_(config) {
    open(arguments, current_folder, &environment);
    async_read();
}

inline Process::Process(const string_type& command,
                        const string_type& current_folder,
                        const environment_type& environment,
                        std::function<void(const char* bytes, size_t n)> read_stdout,
                        std::function<void(const char* bytes, size_t n)> read_stderr,
                        bool open_stdin,
                        const Config& config) noexcept
    :
    closed_(true), read_stdout_(std::move(read_stdout)), read_stderr_(std::move(read_stderr)), open_stdin_(open_stdin), config_(config) {
    open(command, current_folder, &environment);
    async_read();
}

inline Process::~Process() noexcept {
    close_fds();
}

inline Process::id_type Process::getId() const noexcept {
    return data_.id;
}

inline bool Process::write(const std::string& data) {
    return write(data.c_str(), data.size());
}

#ifdef JHC_WIN
inline Process::Data::Data() noexcept :
    id(0), handle(NULL) {}

inline bool Process::successed() const noexcept {
    return (data_.id > 0);
}

inline Process::id_type Process::open(const std::vector<string_type>& arguments,
                                      const string_type& current_folder,
                                      const environment_type* environment) noexcept {
    string_type command;
    for (auto& argument : arguments)
        command += (command.empty() ? L"" : L" ") + argument;
    return open(command, current_folder, environment);
}

// Based on the example at
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682499(v=vs.85).aspx.
inline Process::id_type Process::open(const string_type& command,
                                      const string_type& current_folder,
                                      const environment_type* environment) noexcept {
    if (open_stdin_)
        stdin_fd_ = std::unique_ptr<fd_type>(new fd_type(NULL));
    if (read_stdout_)
        stdout_fd_ = std::unique_ptr<fd_type>(new fd_type(NULL));
    if (read_stderr_)
        stderr_fd_ = std::unique_ptr<fd_type>(new fd_type(NULL));

    ScopedHandle stdin_rd_p(false);
    ScopedHandle stdin_wr_p(false);
    ScopedHandle stdout_rd_p(false);
    ScopedHandle stdout_wr_p(false);
    ScopedHandle stderr_rd_p(false);
    ScopedHandle stderr_wr_p(false);

    SECURITY_ATTRIBUTES security_attributes;

    security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    security_attributes.bInheritHandle = TRUE;
    security_attributes.lpSecurityDescriptor = nullptr;

    std::lock_guard<std::mutex> lock(Process::create_process_mutex);
    if (stdin_fd_) {
        if (!CreatePipe(&stdin_rd_p, &stdin_wr_p, &security_attributes, 0) ||
            !SetHandleInformation(stdin_wr_p, HANDLE_FLAG_INHERIT, 0))
            return 0;
    }
    if (stdout_fd_) {
        if (!CreatePipe(&stdout_rd_p, &stdout_wr_p, &security_attributes, 0) ||
            !SetHandleInformation(stdout_rd_p, HANDLE_FLAG_INHERIT, 0)) {
            return 0;
        }
    }
    if (stderr_fd_) {
        if (!CreatePipe(&stderr_rd_p, &stderr_wr_p, &security_attributes, 0) ||
            !SetHandleInformation(stderr_rd_p, HANDLE_FLAG_INHERIT, 0)) {
            return 0;
        }
    }

    PROCESS_INFORMATION process_info;
    STARTUPINFO startup_info;

    ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));

    ZeroMemory(&startup_info, sizeof(STARTUPINFO));
    startup_info.cb = sizeof(STARTUPINFO);
    startup_info.hStdInput = stdin_rd_p;
    startup_info.hStdOutput = stdout_wr_p;
    startup_info.hStdError = stderr_wr_p;
    if (stdin_fd_ || stdout_fd_ || stderr_fd_)
        startup_info.dwFlags |= STARTF_USESTDHANDLES;

    auto process_command = command;
#ifdef MSYS_PROCESS_USE_SH
    size_t pos = 0;
    while ((pos = process_command.find('\\', pos)) != string_type::npos) {
        process_command.replace(pos, 1, "\\\\\\\\");
        pos += 4;
    }
    pos = 0;
    while ((pos = process_command.find('\"', pos)) != string_type::npos) {
        process_command.replace(pos, 1, "\\\"");
        pos += 2;
    }
    process_command.insert(0, "sh -c \"");
    process_command += "\"";
#endif

    string_type environment_str;
    if (environment) {
        for (const auto& e : *environment)
            environment_str += e.first + L'=' + e.second + L'\0';
        environment_str += L'\0';
    }

    const BOOL bSuccess = CreateProcessW(
        nullptr, process_command.empty() ? nullptr : &process_command[0], nullptr, nullptr,
        stdin_fd_ || stdout_fd_ || stderr_fd_ ||
            config_.inherit_file_descriptors,  // Cannot be false when stdout,
                                               // stderr or stdin is used
        stdin_fd_ || stdout_fd_ || stderr_fd_ ? CREATE_NO_WINDOW
                                              : 0,  // CREATE_NO_WINDOW cannot be used when stdout or
                                                    // stderr is redirected to parent process
        environment_str.empty() ? nullptr : &environment_str[0],
        current_folder.empty() ? nullptr : current_folder.c_str(), &startup_info, &process_info);

    if (!bSuccess)
        return 0;
    else
        CloseHandle(process_info.hThread);

    if (stdin_fd_)
        *stdin_fd_ = stdin_wr_p.detach();
    if (stdout_fd_)
        *stdout_fd_ = stdout_rd_p.detach();
    if (stderr_fd_)
        *stderr_fd_ = stderr_rd_p.detach();

    closed_ = false;
    data_.id = process_info.dwProcessId;
    data_.handle = process_info.hProcess;

    return process_info.dwProcessId;
}

inline void Process::async_read() noexcept {
    if (data_.id == 0)
        return;

    if (stdout_fd_) {
        stdout_thread_ = std::thread([this]() {
            DWORD n;
            std::unique_ptr<char[]> buffer(new char[config_.buffer_size]);
            for (;;) {
                const BOOL bSuccess = ReadFile(*stdout_fd_, static_cast<CHAR*>(buffer.get()),
                                               static_cast<DWORD>(config_.buffer_size), &n, nullptr);
                if (!bSuccess || n == 0)
                    break;
                read_stdout_(buffer.get(), static_cast<size_t>(n));
            }
        });
    }
    if (stderr_fd_) {
        stderr_thread_ = std::thread([this]() {
            DWORD n;
            std::unique_ptr<char[]> buffer(new char[config_.buffer_size]);
            for (;;) {
                const BOOL bSuccess = ReadFile(*stderr_fd_, static_cast<CHAR*>(buffer.get()),
                                               static_cast<DWORD>(config_.buffer_size), &n, nullptr);
                if (!bSuccess || n == 0)
                    break;
                read_stderr_(buffer.get(), static_cast<size_t>(n));
            }
        });
    }
}

inline int Process::getExitStatus() noexcept {
    if (data_.id == 0)
        return -1;

    DWORD exit_status;
    WaitForSingleObject(data_.handle, INFINITE);
    if (!GetExitCodeProcess(data_.handle, &exit_status))
        exit_status = -1;
    {
        std::lock_guard<std::mutex> lock(close_mutex_);
        CloseHandle(data_.handle);
        closed_ = true;
    }
    close_fds();

    return static_cast<int>(exit_status);
}

inline bool Process::tryGetExitStatus(int& exit_status) noexcept {
    if (data_.id == 0)
        return false;

    DWORD wait_status = WaitForSingleObject(data_.handle, 0);

    if (wait_status == WAIT_TIMEOUT)
        return false;

    DWORD exit_status_win;
    if (!GetExitCodeProcess(data_.handle, &exit_status_win))
        exit_status_win = -1;
    {
        std::lock_guard<std::mutex> lock(close_mutex_);
        CloseHandle(data_.handle);
        closed_ = true;
    }
    close_fds();

    exit_status = static_cast<int>(exit_status_win);
    return true;
}

inline void Process::close_fds() noexcept {
    if (stdout_thread_.joinable())
        stdout_thread_.join();
    if (stderr_thread_.joinable())
        stderr_thread_.join();

    if (stdin_fd_)
        closeStdin();
    if (stdout_fd_) {
        if (*stdout_fd_ != NULL)
            CloseHandle(*stdout_fd_);
        stdout_fd_.reset();
    }
    if (stderr_fd_) {
        if (*stderr_fd_ != NULL)
            CloseHandle(*stderr_fd_);
        stderr_fd_.reset();
    }
}

inline bool Process::write(const char* bytes, size_t n) {
    if (!open_stdin_) {
        assert(false && "Can't write to an unopened stdin pipe. Please set open_stdin=true when constructing the process.");
        return false;
    }

    std::lock_guard<std::mutex> lock(stdin_mutex_);
    if (stdin_fd_) {
        DWORD written;
        BOOL bSuccess = WriteFile(*stdin_fd_, bytes, static_cast<DWORD>(n), &written, nullptr);
        if (!bSuccess || written == 0) {
            return false;
        }
        else {
            return true;
        }
    }
    return false;
}

inline void Process::closeStdin() noexcept {
    std::lock_guard<std::mutex> lock(stdin_mutex_);
    if (stdin_fd_) {
        if (*stdin_fd_ != NULL)
            CloseHandle(*stdin_fd_);
        stdin_fd_.reset();
    }
}

inline bool Process::kill(bool /*force*/) noexcept {
    std::lock_guard<std::mutex> lock(close_mutex_);
    if (data_.id > 0 && !closed_) {
        const HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, data_.id);
        if (process_handle)
            return TerminateProcess(process_handle, 2);
    }

    return false;
}

// Based on http://stackoverflow.com/a/1173396
inline void Process::killProcessTree(bool /*force*/) noexcept {
    std::lock_guard<std::mutex> lock(close_mutex_);
    if (data_.id > 0 && !closed_) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot) {
            PROCESSENTRY32 process;
            ZeroMemory(&process, sizeof(process));
            process.dwSize = sizeof(process);
            if (Process32First(snapshot, &process)) {
                do {
                    if (process.th32ParentProcessID == data_.id) {
                        HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, process.th32ProcessID);
                        if (process_handle) {
                            TerminateProcess(process_handle, 2);
                            CloseHandle(process_handle);
                        }
                    }
                } while (Process32Next(snapshot, &process));
            }
            CloseHandle(snapshot);
        }
        TerminateProcess(data_.handle, 2);
    }
}

// Based on http://stackoverflow.com/a/1173396
inline void Process::KillProcessTree(id_type id, bool /*force*/) noexcept {
    if (id == 0)
        return;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot) {
        PROCESSENTRY32 process;
        ZeroMemory(&process, sizeof(process));
        process.dwSize = sizeof(process);
        if (Process32First(snapshot, &process)) {
            do {
                if (process.th32ParentProcessID == id) {
                    const HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, process.th32ProcessID);
                    if (process_handle) {
                        TerminateProcess(process_handle, 2);
                        CloseHandle(process_handle);
                    }
                }
            } while (Process32Next(snapshot, &process));
        }
        CloseHandle(snapshot);
    }

    const HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, id);
    if (process_handle)
        TerminateProcess(process_handle, 2);
}

inline bool Process::Kill(id_type id, bool /*force*/) noexcept {
    if (id == 0)
        return false;

    const HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, id);
    if (!process_handle)
        return false;
    return !!TerminateProcess(process_handle, 2);
}
#else
inline Process::Data::Data() noexcept :
    id(-1) {}

inline bool Process::successed() const noexcept {
    return (data_.id != -1);
}

inline Process::Process(const std::function<void()>& function,
                        std::function<void(const char*, size_t)> read_stdout,
                        std::function<void(const char*, size_t)> read_stderr,
                        bool open_stdin,
                        const Config& config) noexcept
    :
    closed_(true), read_stdout_(std::move(read_stdout)), read_stderr_(std::move(read_stderr)), open_stdin_(open_stdin), config_(config) {
    open(function);
    async_read();
}

inline Process::id_type Process::open(const std::function<void()>& function) noexcept {
    if (open_stdin_)
        stdin_fd_ = std::unique_ptr<fd_type>(new fd_type);
    if (read_stdout_)
        stdout_fd_ = std::unique_ptr<fd_type>(new fd_type);
    if (read_stderr_)
        stderr_fd_ = std::unique_ptr<fd_type>(new fd_type);

    int stdin_p[2], stdout_p[2], stderr_p[2];

    if (stdin_fd_ && pipe(stdin_p) != 0)
        return -1;
    if (stdout_fd_ && pipe(stdout_p) != 0) {
        if (stdin_fd_) {
            close(stdin_p[0]);
            close(stdin_p[1]);
        }
        return -1;
    }
    if (stderr_fd_ && pipe(stderr_p) != 0) {
        if (stdin_fd_) {
            close(stdin_p[0]);
            close(stdin_p[1]);
        }
        if (stdout_fd_) {
            close(stdout_p[0]);
            close(stdout_p[1]);
        }
        return -1;
    }

    id_type pid = fork();

    if (pid < 0) {
        if (stdin_fd_) {
            close(stdin_p[0]);
            close(stdin_p[1]);
        }
        if (stdout_fd_) {
            close(stdout_p[0]);
            close(stdout_p[1]);
        }
        if (stderr_fd_) {
            close(stderr_p[0]);
            close(stderr_p[1]);
        }
        return pid;
    }
    else if (pid == 0) {
        if (stdin_fd_)
            dup2(stdin_p[0], 0);
        if (stdout_fd_)
            dup2(stdout_p[1], 1);
        if (stderr_fd_)
            dup2(stderr_p[1], 2);
        if (stdin_fd_) {
            close(stdin_p[0]);
            close(stdin_p[1]);
        }
        if (stdout_fd_) {
            close(stdout_p[0]);
            close(stdout_p[1]);
        }
        if (stderr_fd_) {
            close(stderr_p[0]);
            close(stderr_p[1]);
        }

        if (!config_.inherit_file_descriptors) {
            // Optimization on some systems: using 8 * 1024 (Debian's default
            // _SC_OPEN_MAX) as fd_max limit
            int fd_max = std::min(8192,
                                  static_cast<int>(sysconf(_SC_OPEN_MAX)));  // Truncation is safe
            if (fd_max < 0)
                fd_max = 8192;
            for (int fd = 3; fd < fd_max; fd++)
                close(fd);
        }

        setpgid(0, 0);
        // TODO: See here on how to emulate tty for colors:
        // http://stackoverflow.com/questions/1401002/trick-an-application-into-thinking-its-stdin-is-interactive-not-a-pipe
        // TODO: One solution is: echo "command;exit"|script -q /dev/null

        if (function)
            function();

        _exit(EXIT_FAILURE);
    }

    if (stdin_fd_)
        close(stdin_p[0]);
    if (stdout_fd_)
        close(stdout_p[1]);
    if (stderr_fd_)
        close(stderr_p[1]);

    if (stdin_fd_)
        *stdin_fd_ = stdin_p[1];
    if (stdout_fd_)
        *stdout_fd_ = stdout_p[0];
    if (stderr_fd_)
        *stderr_fd_ = stderr_p[0];

    closed_ = false;
    data_.id = pid;
    return pid;
}

inline Process::id_type Process::open(const std::vector<string_type>& arguments,
                                      const string_type& current_folder,
                                      const environment_type* environment) noexcept {
    return open([&arguments, &current_folder, &environment] {
        if (arguments.empty())
            exit(127);

        std::vector<const char*> argv_ptrs;
        argv_ptrs.reserve(arguments.size() + 1);
        for (auto& argument : arguments)
            argv_ptrs.emplace_back(argument.c_str());
        argv_ptrs.emplace_back(nullptr);

        if (!current_folder.empty()) {
            if (chdir(current_folder.c_str()) != 0)
                exit(1);
        }

        if (!environment)
            execv(arguments[0].c_str(), const_cast<char* const*>(argv_ptrs.data()));
        else {
            std::vector<std::string> env_strs;
            std::vector<const char*> env_ptrs;
            env_strs.reserve(environment->size());
            env_ptrs.reserve(environment->size() + 1);
            for (const auto& e : *environment) {
                env_strs.emplace_back(e.first + '=' + e.second);
                env_ptrs.emplace_back(env_strs.back().c_str());
            }
            env_ptrs.emplace_back(nullptr);

            execve(arguments[0].c_str(), const_cast<char* const*>(argv_ptrs.data()),
                   const_cast<char* const*>(env_ptrs.data()));
        }
    });
}

inline Process::id_type Process::open(const std::string& command,
                                      const std::string& current_folder,
                                      const environment_type* environment) noexcept {
    return open([&command, &current_folder, &environment] {
        if (!current_folder.empty()) {
            if (chdir(current_folder.c_str()) != 0)
                exit(1);
        }

        if (!environment)
            execl("/bin/sh", "/bin/sh", "-c", command.c_str(), nullptr);
        else {
            std::vector<std::string> env_strs;
            std::vector<const char*> env_ptrs;
            env_strs.reserve(environment->size());
            env_ptrs.reserve(environment->size() + 1);
            for (const auto& e : *environment) {
                env_strs.emplace_back(e.first + '=' + e.second);
                env_ptrs.emplace_back(env_strs.back().c_str());
            }
            env_ptrs.emplace_back(nullptr);
            execle("/bin/sh", "/bin/sh", "-c", command.c_str(), nullptr, env_ptrs.data());
        }
    });
}

inline void Process::async_read() noexcept {
    if (data_.id <= 0 || (!stdout_fd_ && !stderr_fd_))
        return;

    stdout_stderr_thread_ = std::thread([this] {
        std::vector<pollfd> pollfds;
        std::bitset<2> fd_is_stdout;
        if (stdout_fd_) {
            fd_is_stdout.set(pollfds.size());
            pollfds.emplace_back();
            pollfds.back().fd = fcntl(*stdout_fd_, F_SETFL, fcntl(*stdout_fd_, F_GETFL) | O_NONBLOCK) == 0
                                    ? *stdout_fd_
                                    : -1;
            pollfds.back().events = POLLIN;
        }
        if (stderr_fd_) {
            pollfds.emplace_back();
            pollfds.back().fd = fcntl(*stderr_fd_, F_SETFL, fcntl(*stderr_fd_, F_GETFL) | O_NONBLOCK) == 0
                                    ? *stderr_fd_
                                    : -1;
            pollfds.back().events = POLLIN;
        }
        auto buffer = std::unique_ptr<char[]>(new char[config_.buffer_size]);
        bool any_open = !pollfds.empty();
        while (any_open && (poll(pollfds.data(), pollfds.size(), -1) > 0 || errno == EINTR)) {
            any_open = false;
            for (size_t i = 0; i < pollfds.size(); ++i) {
                if (pollfds[i].fd >= 0) {
                    if (pollfds[i].revents & POLLIN) {
                        const ssize_t n = read(pollfds[i].fd, buffer.get(), config_.buffer_size);
                        if (n > 0) {
                            if (fd_is_stdout[i])
                                read_stdout_(buffer.get(), static_cast<size_t>(n));
                            else
                                read_stderr_(buffer.get(), static_cast<size_t>(n));
                        }
                        else if (n < 0 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
                            pollfds[i].fd = -1;
                            continue;
                        }
                    }
                    if (pollfds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                        pollfds[i].fd = -1;
                        continue;
                    }
                    any_open = true;
                }
            }
        }
    });
}

inline int Process::getExitStatus() noexcept {
    if (data_.id <= 0)
        return -1;

    int exit_status;
    id_type p;
    do {
        p = waitpid(data_.id, &exit_status, 0);
    } while (p < 0 && errno == EINTR);

    if (p < 0 && errno == ECHILD) {
        // PID doesn't exist anymore, return previously sampled exit status (or
        // -1)
        return data_.exit_status;
    }
    else {
        // store exit status for future calls
        if (exit_status >= 256)
            exit_status = exit_status >> 8;
        data_.exit_status = exit_status;
    }

    {
        std::lock_guard<std::mutex> lock(close_mutex_);
        closed_ = true;
    }
    close_fds();

    return exit_status;
}

inline bool Process::tryGetExitStatus(int& exit_status) noexcept {
    if (data_.id <= 0)
        return false;

    const id_type p = waitpid(data_.id, &exit_status, WNOHANG);
    if (p < 0 && errno == ECHILD) {
        // PID doesn't exist anymore, set previously sampled exit status (or -1)
        exit_status = data_.exit_status;
        return true;
    }
    else if (p <= 0) {
        // Process still running (p==0) or error
        return false;
    }
    else {
        // store exit status for future calls
        if (exit_status >= 256)
            exit_status = exit_status >> 8;
        data_.exit_status = exit_status;
    }

    {
        std::lock_guard<std::mutex> lock(close_mutex_);
        closed_ = true;
    }
    close_fds();

    return true;
}

inline void Process::close_fds() noexcept {
    if (stdout_stderr_thread_.joinable())
        stdout_stderr_thread_.join();

    if (stdin_fd_)
        closeStdin();
    if (stdout_fd_) {
        if (data_.id > 0)
            close(*stdout_fd_);
        stdout_fd_.reset();
    }
    if (stderr_fd_) {
        if (data_.id > 0)
            close(*stderr_fd_);
        stderr_fd_.reset();
    }
}

inline bool Process::write(const char* bytes, size_t n) {
    if (!open_stdin_) {
        assert(false && "Can't write to an unopened stdin pipe. Please set open_stdin=true when constructing the process.");
        return false;
    }

    std::lock_guard<std::mutex> lock(stdin_mutex_);
    if (stdin_fd_) {
        if (::write(*stdin_fd_, bytes, n) >= 0) {
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}

inline void Process::closeStdin() noexcept {
    std::lock_guard<std::mutex> lock(stdin_mutex_);
    if (stdin_fd_) {
        if (data_.id > 0)
            close(*stdin_fd_);
        stdin_fd_.reset();
    }
}

inline bool Process::kill(bool force) noexcept {
    std::lock_guard<std::mutex> lock(close_mutex_);
    if (data_.id > 0 && !closed_) {
        if (force)
            return ::kill(data_.id, SIGTERM) == 0;
        return ::kill(data_.id, SIGINT) == 0;
    }
}

inline void Process::killProcessTree(bool force) noexcept {
    std::lock_guard<std::mutex> lock(close_mutex_);
    if (data_.id > 0 && !closed_) {
        if (force)
            ::kill(-data_.id, SIGTERM);
        else
            ::kill(-data_.id, SIGINT);
    }
}

inline void Process::KillProcessTree(id_type id, bool force) noexcept {
    if (id <= 0)
        return;

    if (force)
        ::kill(-id, SIGTERM);
    else
        ::kill(-id, SIGINT);
}

inline bool Process::Kill(id_type id, bool force) noexcept {
    if (force)
        return ::kill(id, SIGTERM) == 0;
    return ::kill(id, SIGINT) == 0;
}
#endif
}  // namespace jhc
#endif  // !JHC_PROCESS_HPP__
