# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Authors:
# Cyrille Favreau <cyrille.favreau@epfl.ch>
# Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
# Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
# Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(BraynsVersion)

project(Brayns VERSION ${BRAYNS_VERSION})

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BRAYNS_ENABLE_CIRCUITS "Build circuit support" ON)
option(BRAYNS_ENABLE_ATLAS "Build atlas support" ON)
option(BRAYNS_ENABLE_APPS "Build applications" ON)
option(BRAYNS_ENABLE_TESTS "Build unit tests" ON)
option(BRAYNS_ENABLE_DEPS "Downloads automatically dependencies" ON)

find_package(OpenMP REQUIRED)
find_package(ospray REQUIRED)
find_package(rkcommon REQUIRED)
find_package(Poco QUIET COMPONENTS Foundation JSON Net NetSSL)
find_package(ZLIB REQUIRED)

if(BRAYNS_ENABLE_CIRCUITS)
    find_package(HDF5 REQUIRED)
    find_package(HighFive QUIET)
    find_package(sonata QUIET)
    find_package(MorphIO QUIET)
endif()

if(BRAYNS_ENABLE_ATLAS)
    find_package(BZip2 REQUIRED)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_INSTALL_LIBDIR lib)
set(CMAKE_INSTALL_BINDIR bin)
set(CMAKE_INSTALL_INCLUDEDIR include)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Werror)
endif()

add_subdirectory(src)

if(BRAYNS_ENABLE_APPS)
    add_subdirectory(apps)
endif()

if(BRAYNS_ENABLE_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()

if(BRAYNS_ENABLE_DEPS)
    add_subdirectory(deps)
endif()

install(
    EXPORT BraynsTargets
    FILE BraynsTargets.cmake
    DESTINATION cmake
    NAMESPACE brayns::
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    cmake/BraynsConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/BraynsConfig.cmake
    INSTALL_DESTINATION cmake
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/BraynsConfigVersion.cmake
    VERSION ${BRAYNS_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/BraynsConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/BraynsConfigVersion.cmake
    DESTINATION cmake
)
