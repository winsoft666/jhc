#################################################################################
#    C++ Common Library
#    ---------------------------------------------------------------------------
#    Copyright (C) 2022 JiangXueqiao <winsoft666@outlook.com>.
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
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#################################################################################

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")

project(jhc CXX)

configure_file("${PROJECT_SOURCE_DIR}/include/jhc/config.hpp.in" "${PROJECT_SOURCE_DIR}/include/jhc/config.hpp")

option(JHC_NOT_HEADER_ONLY "Whether definition and implementation are decoupled or not" ON)
option(BUILD_TESTS "Build tests project" ON)
option(USE_STATIC_CRT "Set to ON to build with static CRT on Windows (/MT)." OFF)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Debug Output
message(STATUS "JHC_NOT_HEADER_ONLY=${JHC_NOT_HEADER_ONLY}")
message(STATUS "USE_STATIC_CRT=${USE_STATIC_CRT}")
message(STATUS "CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "VCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET}")

# Unicode Support
add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set (CMAKE_CXX_STANDARD 11)
    
if (JHC_NOT_HEADER_ONLY)
    set(DEBUG_LIBNAME_SUFFIX "-d" CACHE STRING "Optional suffix to append to the library name for a debug build")
    mark_as_advanced(DEBUG_LIBNAME_SUFFIX)

    include_directories(./include)

    set(LIB_NAME jhc-static)

    if (MSVC AND USE_STATIC_CRT)
        set(CompilerFlags
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            )
        foreach(CompilerFlag ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
        endforeach()
    endif()

    file(GLOB HEADER_FILES 			./include/jhc_all.hpp ./include/jhc/*.hpp)
    file(GLOB SOURCE_FILES 			./include/jhc/impl/*.cc)

    source_group("Header Files" 			FILES ${HEADER_FILES})
    source_group("Source Files" 		    FILES ${SOURCE_FILES})
    
    add_library(
      ${LIB_NAME}
      STATIC
      ${SOURCE_FILES}
      ${HEADER_FILES})

    set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    
    # set output name prefix/suffix
    set_target_properties(${LIB_NAME} PROPERTIES 
      OUTPUT_NAME ${LIB_NAME}
      DEBUG_OUTPUT_NAME ${LIB_NAME}${DEBUG_LIBNAME_SUFFIX})
      
    if(WIN32)
      set_target_properties(${LIB_NAME} PROPERTIES COMPILE_FLAGS "/wd4251")

      target_link_libraries(${LIB_NAME} PUBLIC 
        Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib Psapi.lib Shlwapi.lib)
    endif()

    # Set output directory
    set_target_properties(${LIB_NAME}
      PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )

    target_include_directories(${LIB_NAME} INTERFACE $<INSTALL_INTERFACE:include>)

    install(TARGETS ${LIB_NAME}
      EXPORT ${LIB_NAME}-target
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib)

    install(FILES ./include/jhc_all.hpp DESTINATION include)
    install(FILES ${HEADER_FILES} DESTINATION include/jhc)
endif()

if(BUILD_TESTS)
    add_subdirectory(test)
    enable_testing()
endif()