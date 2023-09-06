# Copyright (c) 2015-2023, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

include(FetchContent)
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
set(CMAKE_INSTALL_LIBDIR lib)

find_package(Git REQUIRED)

# =============================================================================
# GLM
# =============================================================================
if((BRAYNS_CIRCUITEXPLORER_ENABLED OR BRAYNS_DTI_ENABLED) AND NOT glm_FOUND AND NOT TARGET glm)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glm")
        message(STATUS "Downloading GLM from submodules")
        FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG 0.9.9.8
            GIT_SHALLOW ON
            GIT_SUBMODULES_RECURSE OFF
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glm
        )
        FetchContent_Populate(glm)
    endif()

    add_library(glm INTERFACE)
    target_include_directories(glm SYSTEM INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/glm>
        $<INSTALL_INTERFACE:include>)

    install(TARGETS glm
        EXPORT Brayns-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
    )

    install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/glm/glm
        DESTINATION include)
endif()

# =============================================================================
# libsonata
# =============================================================================
if(BRAYNS_CIRCUITEXPLORER_ENABLED AND NOT sonata_FOUND AND NOT TARGET sonata::sonata_shared)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libsonata")
        message(STATUS "Downloading libsonata from submodules")
        FetchContent_Declare(
            libsonata
            GIT_REPOSITORY https://github.com/BlueBrain/libsonata.git
            GIT_TAG v0.1.22
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
            GIT_TAG v3.3.5
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
    set(MORPHIO_VERSION_STRING "3.3.5")
    set(MorphIO_VERSION_MAJOR 3)
    set(MorphIO_VERSION_MINOR 3)
    set(MorphIO_VERSION_PATCH 5)

    # Morphio
    add_subdirectory(MorphIO/src)
    add_library(morphiointer INTERFACE)
    target_include_directories(morphiointer INTERFACE ${CMAKE_CURRENT_LIST_DIR}/MorphIO/include)
    target_link_libraries(morphiointer INTERFACE morphio_shared)
    add_library(MorphIO::morphio ALIAS morphiointer)
endif()

# =============================================================================
# MVDTool
# =============================================================================
if(BRAYNS_CIRCUITEXPLORER_ENABLED AND NOT MVDTool_FOUND AND NOT TARGET MVDTool)
    find_package(Boost 1.41 QUIET REQUIRED COMPONENTS system)

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/MVDTool")
        message(STATUS "Downloading MVDTool from submodules")
        FetchContent_Declare(
            MVDTool
            GIT_REPOSITORY https://github.com/BlueBrain/MVDTool.git
            GIT_TAG v2.4.4
            GIT_SHALLOW ON

            # GIT_SUBMODULES # Empty string from 3.16 onwards will not clone submodules
            GIT_SUBMODULES_RECURSE OFF
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/MVDTool
        )
        FetchContent_Populate(MVDTool)
    endif()

    add_library(MVDTool INTERFACE)

    target_include_directories(MVDTool SYSTEM INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/MVDTool/include>
        $<INSTALL_INTERFACE:include>)
    target_include_directories(MVDTool SYSTEM INTERFACE ${Boost_INCLUDE_DIR})
    target_link_libraries(MVDTool INTERFACE HighFive)
    target_compile_definitions(MVDTool INTERFACE -DH5_USE_BOOST)
endif()

# =============================================================================
# Brion
# =============================================================================
if((BRAYNS_CIRCUITEXPLORER_ENABLED OR BRAYNS_DTI_ENABLED) AND NOT Brion_FOUND)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Brion")
        message(STATUS "Downloading Brion from submodules")
        FetchContent_Declare(
            Brion
            GIT_REPOSITORY https://github.com/BlueBrain/Brion.git
            GIT_TAG 3.3.14
            GIT_SHALLOW ON
            GIT_SUBMODULES_RECURSE OFF
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Brion
        )
        FetchContent_Populate(Brion)
    endif()

    add_subdirectory(Brion)

    # Remove Brion warnings
    set_target_properties(Brion PROPERTIES
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Brion,INTERFACE_INCLUDE_DIRECTORIES>
    )
endif()
