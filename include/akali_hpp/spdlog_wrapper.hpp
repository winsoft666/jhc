/*******************************************************************************
*    Copyright (C) <2022>  <winsoft666@outlook.com>.
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

#ifndef AKALI_SPDLOG_WRAPPER_HPP__
#define AKALI_SPDLOG_WRAPPER_HPP__
#include "akali_hpp/arch.hpp"
#include "spdlog/spdlog.h"
#if defined(_WIN32)
#include "spdlog/sinks/msvc_sink.h"
#endif
#include "spdlog/sinks/rotating_file_sink.h"

#ifdef AKALI_WIN
#include <Shlwapi.h>
#include <ShlObj.h>
#include <strsafe.h>
#include "akali_hpp/process_util.hpp"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Shell32.lib")
#endif

namespace akali_hpp {
class SpdlogWrapper {
   public:
    static bool GlobalRegister(const std::string& appName) {
        if (appName.empty())
            return false;

        try {
            std::vector<spdlog::sink_ptr> sinks;

            std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> file_sink;
#ifdef AKALI_WIN
            char szAppDataFolder[1024] = {0};
            if (!SHGetSpecialFolderPathA(NULL, szAppDataFolder, CSIDL_LOCAL_APPDATA, 0) || strlen(szAppDataFolder) == 0) {
                StringCchCopyA(szAppDataFolder, 1024, ".");
            }

            PathAddBackslashA(szAppDataFolder);
            PathCombineA(szAppDataFolder, szAppDataFolder, appName.c_str());
            PathAddBackslashA(szAppDataFolder);

            const std::string strExePath = ProcessUtil::GetCurrentProcessPath();
            std::string strExeName = PathFindFileNameA(strExePath.c_str());
            if (strExeName.empty()) {
                strExeName = "app";
            }

            // C:\Users\xxx\AppData\Local\<appName>\<strExeName>.log
            std::string logFileBaseName = szAppDataFolder;
            logFileBaseName += strExeName;
            logFileBaseName += ".log";

            file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                logFileBaseName,
                1048576 * 5,  // 5MB
                30            // 30 files
            );
            file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e %z] [%L] [%P-%t] %v");

#else
            // /var/log/<appName>/app.log
            std::string strLogFolder = "/var/log/" + appName;
            std::string strExeName = "app";
            std::string logFileBaseName = strLogFolder + strExeName + ".log";

            file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                logFileBaseName,
                1048576 * 5,  // 5MB
                30            // 30 files
            );
            file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e %z] [%L] [%P-%t] %v");

#endif
            sinks.push_back(file_sink);

#if defined(_WIN32)
            auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
            msvc_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e %z] [%L] [%P-%t] %v");
            sinks.push_back(msvc_sink);
#endif
            auto log = std::make_shared<spdlog::logger>("akali_hpp_spdlog_wrapper", std::begin(sinks), std::end(sinks));
            spdlog::register_logger(log);

            spdlog::set_level(spdlog::level::trace);
            spdlog::flush_every(std::chrono::seconds(1));
            spdlog::flush_on(spdlog::level::warn);
        } catch (const spdlog::spdlog_ex& ex) {
            (void)(ex);
            return false;
        } catch (const std::exception& e) {
            (void)(e);
            return false;
        }

        return true;
    }

    static void GlobalDrop() {
        spdlog::drop_all();
    }

    static void Flush() {
        if (spdlog::get("akali_hpp_spdlog_wrapper")) {
            spdlog::get("akali_hpp_spdlog_wrapper")->flush();
        }
        else {
            assert(!"spdlog must be correctly setup first!");
        }
    }

    template <typename... Args>
    static void Trace(const char* format, const Args&... args) {
        if (spdlog::get("akali_hpp_spdlog_wrapper")) {
            spdlog::get("akali_hpp_spdlog_wrapper")->trace(format, args...);
        }
        else {
            assert(!"spdlog must be correctly setup first!");
        }
    }

    template <typename... Args>
    static void Info(const char* format, const Args&... args) {
        if (spdlog::get("akali_hpp_spdlog_wrapper")) {
            spdlog::get("akali_hpp_spdlog_wrapper")->info(format, args...);
        }
        else {
            assert(!"spdlog must be correctly setup first!");
        }
    }

    template <typename... Args>
    static void Warn(const char* format, const Args&... args) {
        if (spdlog::get("akali_hpp_spdlog_wrapper")) {
            spdlog::get("akali_hpp_spdlog_wrapper")->warn(format, args...);
        }
        else {
            assert(!"spdlog must be correctly setup first!");
        }
    }

    template <typename... Args>
    static void Error(const char* format, const Args&... args) {
        if (spdlog::get("akali_hpp_spdlog_wrapper")) {
            spdlog::get("akali_hpp_spdlog_wrapper")->error(format, args...);
        }
        else {
            assert(!"spdlog must be correctly setup first!");
        }
    }

    template <typename... Args>
    static void Critical(const char* format, const Args&... args) {
        if (spdlog::get("akali_hpp_spdlog_wrapper")) {
            spdlog::get("akali_hpp_spdlog_wrapper")->critical(format, args...);
        }
        else {
            assert(!"spdlog must be correctly setup first!");
        }
    }
};
}  // namespace akali_hpp

#endif  // !AKALI_SPDLOG_WRAPPER_HPP__