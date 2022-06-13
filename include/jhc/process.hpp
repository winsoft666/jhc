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
#include "jhc/config.hpp"
#include "jhc/arch.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include "jhc/macros.hpp"
#include "jhc/singleton_class.hpp"

namespace jhc {
/// Platform independent class for creating processes.
/// Note on Windows: it seems not possible to specify which pipes to redirect.
/// Thus, at the moment, if read_stdout==nullptr, read_stderr==nullptr and
/// open_stdin==false, the stdout, stderr and stdin are sent to the parent
/// process instead.
//
class Process {
   public:
    JHC_DISALLOW_COPY_MOVE(Process);
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

    static std::string GetProcessPath(id_type id) noexcept;

   private:
#ifdef JHC_WIN
    class ProcessMutex : public SingletonClass<ProcessMutex> {
       public:
        // Based on the discussion thread:
        // https://www.reddit.com/r/cpp/comments/3vpjqg/a_new_platform_independent_process_library_for_c11/cxq1wsj
        std::mutex create_process_mutex;
    };
#endif
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
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/process.cc"
#endif

#endif  // !JHC_PROCESS_HPP__
