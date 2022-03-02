############################################################################
#    Copyright (C) <2019-2022>, winsoft666, <winsoft666@outlook.com>.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http:#www.gnu.org/licenses/>.
############################################################################

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")

project(akali_hpp CXX)

set (CMAKE_CXX_STANDARD 14)

set(EXE_NAME test)

add_definitions(-DUNICODE -D_UNICODE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB AKALI_HPP_HEADER_FILES ../include/akali_hpp.h ../include/akali_hpp/*.*)
file(GLOB_RECURSE SPDLOG_FILES ../include/spdlog/*.*)
file(GLOB TEST_SOURCE_FILES ./*.cpp)

source_group("akali_hpp" FILES ${AKALI_HPP_HEADER_FILES})
source_group("akali_hpp\\spdlog" FILES ${SPDLOG_FILES})
source_group("test" FILES ${TEST_SOURCE_FILES})

include_directories(../include)

add_executable(
	${EXE_NAME}
	${AKALI_HPP_HEADER_FILES}
	${SPDLOG_FILES}
	${TEST_SOURCE_FILES}
	)
	
# Win32 Console
if (WIN32 OR _WIN32)
	set_target_properties(${EXE_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
	set_target_properties(${EXE_NAME} PROPERTIES COMPILE_DEFINITIONS "_CONSOLE")
else()
   target_link_libraries(${EXE_NAME} pthread)
endif()
