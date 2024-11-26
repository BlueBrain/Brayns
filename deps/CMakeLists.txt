# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

include(FetchContent)

set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
set(CMAKE_INSTALL_LIBDIR lib)

# =============================================================================
# Poco libraries
# =============================================================================
if(NOT Poco_FOUND)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/poco")
        message(STATUS "Downloading Poco from submodules")
        FetchContent_Declare(
            poco
            GIT_REPOSITORY https://github.com/pocoproject/poco.git
            GIT_TAG poco-1.12.5-release
            GIT_SHALLOW ON
            GIT_SUBMODULES_RECURSE ON
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/poco
        )
        FetchContent_Populate(poco)
    endif()

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
    set_target_properties(Foundation PROPERTIES
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Foundation,INTERFACE_INCLUDE_DIRECTORIES>
    )
    set_target_properties(Net PROPERTIES
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Net,INTERFACE_INCLUDE_DIRECTORIES>
    )

    # Remove Poco postfix d in debug
    set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "Set Debug library postfix" FORCE)
    set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "Set Release library postfix" FORCE)
    set(CMAKE_MINSIZEREL_POSTFIX "" CACHE STRING "Set MinSizeRel library postfix" FORCE)
    set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "Set RelWithDebInfo library postfix" FORCE)
endif()

# =============================================================================
# spdlog
# =============================================================================
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/spdlog")
    message(STATUS "Downloading spdlog from submodules")
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.0
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

# =============================================================================
# tinyexr
# =============================================================================
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tinyexr")
    message(STATUS "Downloading tinyexr from submodules")
    FetchContent_Declare(
        tinyexr
        GIT_REPOSITORY https://github.com/syoyo/tinyexr
        GIT_TAG v1.0.7
        GIT_SHALLOW ON
        GIT_SUBMODULES_RECURSE OFF
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tinyexr
    )
    FetchContent_Populate(tinyexr)
endif()

add_library(tinyexr INTERFACE)

set(BRAYNS_TINYEXR_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyexr
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyexr/deps/miniz)

target_include_directories(tinyexr SYSTEM INTERFACE
    "$<BUILD_INTERFACE:${BRAYNS_TINYEXR_INCLUDES}>"
    "$<INSTALL_INTERFACE:include>")

# =============================================================================
# doctest
# =============================================================================
if(BRAYNS_TESTS_ENABLED)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/doctest")
        message(STATUS "Downloading doctest from submodules")
        FetchContent_Declare(
            doctest
            GIT_REPOSITORY https://github.com/doctest/doctest
            GIT_TAG v2.4.11
            GIT_SHALLOW ON
            GIT_SUBMODULES_RECURSE OFF
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doctest
        )
        FetchContent_Populate(doctest)
    endif()

    add_library(doctest INTERFACE)
    target_include_directories(doctest SYSTEM INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/doctest>"
        "$<INSTALL_INTERFACE:include>")
    target_compile_definitions(doctest INTERFACE DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)
endif()
