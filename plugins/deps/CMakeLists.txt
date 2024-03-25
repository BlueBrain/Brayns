# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

include(FetchContent)
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
set(CMAKE_INSTALL_LIBDIR lib)

find_package(Git REQUIRED)

# =============================================================================
# libsonata
# =============================================================================
if(BRAYNS_CIRCUITEXPLORER_ENABLED AND NOT sonata_FOUND AND NOT TARGET sonata::sonata_shared)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libsonata")
        message(STATUS "Downloading libsonata from submodules")
        FetchContent_Declare(
            libsonata
            GIT_REPOSITORY https://github.com/BlueBrain/libsonata.git
            GIT_TAG v0.1.24
            GIT_SHALLOW ON
            GIT_SUBMODULES_RECURSE ON
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libsonata
        )
        FetchContent_Populate(libsonata)
    endif()

    set(EXTLIB_FROM_SUBMODULES ON CACHE BOOL "" FORCE)
    set(SONATA_TESTS OFF CACHE BOOL "" FORCE)
    SET(SONATA_CXX_WARNINGS OFF CACHE BOOL "" FORCE)
    set(CMAKE_CXX_STANDARD 14 CACHE STRING "" FORCE)

    add_subdirectory(libsonata)
    add_library(sonata::sonata_shared ALIAS sonata_shared)
endif()

# =============================================================================
# MorphIO
# =============================================================================
if(BRAYNS_CIRCUITEXPLORER_ENABLED AND NOT MorphIO_FOUND AND NOT TARGET MorphIO::morphio)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/MorphIO")
        message(STATUS "Downloading MorphIO from submodules")
        FetchContent_Declare(
            MorphIO
            GIT_REPOSITORY https://github.com/BlueBrain/MorphIO.git
            GIT_TAG v3.3.7
            GIT_SHALLOW ON
            GIT_SUBMODULES_RECURSE ON
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/MorphIO
        )
        FetchContent_Populate(MorphIO)
    endif()

    set(BUILD_BINDINGS OFF CACHE BOOL "" FORCE)
    set(MORPHIO_TESTS OFF CACHE BOOL "" FORCE)

    set(MORPHIO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/MorphIO/include/)

    # Deps
    add_subdirectory(MorphIO/3rdparty/GSL_LITE)
    target_include_directories(gsl-lite SYSTEM INTERFACE)
    add_library(lexertl INTERFACE)
    target_include_directories(lexertl SYSTEM INTERFACE MorphIO/3rdparty/lexertl14/include)

    add_library(ghc_filesystem INTERFACE)
    target_include_directories(ghc_filesystem SYSTEM INTERFACE MorphIO/3rdparty/ghc_filesystem/include)

    # Retrieve version
    set(MORPHIO_VERSION_STRING "3.3.7")
    set(MorphIO_VERSION_MAJOR 3)
    set(MorphIO_VERSION_MINOR 3)
    set(MorphIO_VERSION_PATCH 7)

    # Morphio
    add_subdirectory(MorphIO/src)
    add_library(morphiointer INTERFACE)
    target_include_directories(morphiointer INTERFACE ${CMAKE_CURRENT_LIST_DIR}/MorphIO/include)
    target_link_libraries(morphiointer INTERFACE morphio_shared)
    add_library(MorphIO::morphio ALIAS morphiointer)
endif()
