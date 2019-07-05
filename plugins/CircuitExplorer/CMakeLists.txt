cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(BraynsCircuitExplorer VERSION 1.0.0)
set(BraynsCircuitExplorer_VERSION_ABI 1)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake)
  message(FATAL_ERROR "CMake/common missing, run: git submodule update --init --recursive")
endif()

# C++ 14
set (CMAKE_CXX_STANDARD 14)

include(Common)
option(BRAYNS_CIRCUITEXPLORER_USE_DB "Circuit explorer uses DB storage" OFF)
if(BRAYNS_CIRCUITEXPLORER_USE_DB)
    find_package(HDF5 REQUIRED)
    set(BUILD_BINDINGS OFF CACHE BOOL "Do not build python bindings" FORCE)
    add_subdirectory(${PROJECT_SOURCE_DIR}/MorphIO)
else()
    # For some reason, this has to be in the root CMake in order to be
    # automatically downloaded by the -DCLONE_SUBPROJECTS=ON cmake option
    common_find_package(Brion REQUIRED)
endif()
common_find_package_post()

add_subdirectory(plugin)
add_subdirectory(module)
