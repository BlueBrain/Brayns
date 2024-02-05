# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

include(FetchContent)

set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
set(CMAKE_INSTALL_LIBDIR lib)

# =============================================================================
# Async++
# =============================================================================
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build Async++ as a shared library" FORCE)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/async++")
    message(STATUS "Downloading Async++ from submodules")
    FetchContent_Declare(
        async++
        GIT_REPOSITORY https://github.com/Amanieu/asyncplusplus.git
        GIT_TAG        2c852e5c8751350afaa6cbd67a3d921a7469bc71
        GIT_SHALLOW ON
        GIT_SUBMODULES_RECURSE OFF
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/async++
    )
    FetchContent_Populate(async++)
endif()
add_subdirectory(async++)

# =============================================================================
# Poco
# =============================================================================
if(NOT Poco_Found)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/poco")
        message(STATUS "Downloading Poco from submodules")
        FetchContent_Declare(
            poco
            GIT_REPOSITORY https://github.com/pocoproject/poco.git
            GIT_TAG        poco-1.11.1-release
            GIT_SHALLOW ON
            GIT_SUBMODULES_RECURSE ON
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/poco
        )
        FetchContent_Populate(poco)
    endif()

    # Both Poco and GLM create "uninstall" targets. But GLM check wether the
    # target exists before or not, whereas POCO doesnt give a damn.
    set(ENABLE_REDIS OFF CACHE BOOL "Enable Redis")
    set(ENABLE_ENCODINGS OFF CACHE BOOL "Enable Encodings")
    set(ENABLE_APACHECONNECTOR OFF CACHE BOOL "Enable Apache")
    set(ENABLE_MONGODB OFF CACHE BOOL "Enable MongoDB")
    set(ENABLE_DATA_POSTGRESQL OFF CACHE BOOL "Enable PostgreSQL")
    set(ENABLE_DATA_MYSQL OFF CACHE BOOL "Enable MySQL")
    set(ENABLE_DATA_ODBC OFF CACHE BOOL "Enable Data ODBC")
    set(ENABLE_DATA_SQLITE OFF CACHE BOOL "Enable Data SQlite")
    set(ENABLE_ZIP OFF CACHE BOOL "Enable Zip")
    set(ENABLE_PAGECOMPILER OFF CACHE BOOL "Enable PageCompiler")
    set(ENABLE_PAGECOMPILER_FILE2PAGE OFF CACHE BOOL "Enable File2Page")
    set(BUILD_SHARED_LIBS ON)
    add_subdirectory(poco)

    # Remove Poco warnings
    if(NOT CMAKE_BUILD_TYPE MATCHES DEBUG)
        target_compile_options(Foundation PUBLIC -w)
    endif()

    # Remove Poco postfix d in debug
    set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "Set Debug library postfix" FORCE)
    set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "Set Release library postfix" FORCE)
    set(CMAKE_MINSIZEREL_POSTFIX "" CACHE STRING "Set MinSizeRel library postfix" FORCE)
    set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "Set RelWithDebInfo library postfix" FORCE)
endif()

# =============================================================================
# GLM
# =============================================================================
if(NOT glm_FOUND AND NOT TARGET glm)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glm")
        message(STATUS "Downloading GLM from submodules")
        FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG        9749727c2db4742369219e1d452f43e918734b4e
            GIT_SHALLOW ON
            GIT_SUBMODULES_RECURSE OFF
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glm
        )
        FetchContent_Populate(glm)
    endif()
    option(GLM_QUIET "No CMake Message" ON)
    option(GLM_TEST_ENABLE "Build unit tests" OFF)
    option(GLM_TEST_ENABLE_CXX_14 "Enable C++ 14" ON)
    set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
    set(CMAKE_INSTALL_LIBDIR lib)
    add_subdirectory(glm)

    # WAR for https://github.com/g-truc/glm/issues/854
    if(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
      target_compile_options(glm INTERFACE -Wno-pedantic -Wno-error=pedantic -Wno-class-memaccess -Wno-error=class-memaccess)
    endif()
endif()

# =============================================================================
# spdlog
# =============================================================================
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/spdlog")
    message(STATUS "Downloading spdlog from submodules")
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.9.2
        GIT_SHALLOW ON
        GIT_SUBMODULES_RECURSE ON
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/spdlog
    )
    FetchContent_Populate(spdlog)
endif()
set(SPDLOG_BUILD_SHARED ON CACHE BOOL "" FORCE)
set(SPDLOG_MASTER_PROJECT ON)
add_subdirectory(spdlog)

# =============================================================================
# stb
# =============================================================================
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/stb")
    message(STATUS "Downloading stb from submodules")
    FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stb
    )
    FetchContent_Populate(stb)
endif()

add_library(stb INTERFACE)

target_include_directories(stb SYSTEM INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/stb>"
    "$<INSTALL_INTERFACE:include>")
