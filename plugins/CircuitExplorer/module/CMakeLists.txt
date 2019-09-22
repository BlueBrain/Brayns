# Copyright (c) 2018, Cyrille Favreau
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@gmail.com>
#
# This file is part of https://github.com/favreau/Brayns-UC-CircuitExplorer

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
set(MODULE_NAME ospray_module_braynsCircuitExplorer)
project(${MODULE_NAME} VERSION 1.0.0)
set(${MODULE_NAME}_VERSION_ABI 1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# OSPRay
list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
find_package(ospray REQUIRED SYSTEM)
include(ispc)

set(MODULE_SOURCES
    CircuitExplorer.cpp
    ispc/camera/DOFPerspectiveCamera.cpp
    ispc/render/CircuitExplorerMaterial.cpp
    ispc/render/CircuitExplorerBasicRenderer.cpp
    ispc/render/CircuitExplorerAdvancedRenderer.cpp
    ispc/render/VoxelizedSimulationRenderer.cpp
    ispc/render/ProximityDetectionRenderer.cpp
    ispc/render/CellGrowthRenderer.cpp
    ispc/render/utils/CircuitExplorerAbstractRenderer.cpp
    ispc/render/utils/CircuitExplorerSimulationRenderer.cpp
    )

set(MODULE_ISPC_SOURCES
    ispc/camera/utils.ispc
    ispc/camera/DOFPerspectiveCamera.ispc
    ispc/render/CircuitExplorerMaterial.ispc
    ispc/render/CircuitExplorerBasicRenderer.ispc
    ispc/render/CircuitExplorerAdvancedRenderer.ispc
    ispc/render/VoxelizedSimulationRenderer.ispc
    ispc/render/ProximityDetectionRenderer.ispc
    ispc/render/CellGrowthRenderer.ispc
    ispc/render/utils/CircuitExplorerSimulationRenderer.ispc
    ispc/render/utils/RandomGenerator.ispc
    ispc/render/utils/CircuitExplorerSkyBox.ispc
    )

# Compile ispc code
list(APPEND ALL_ISPC_INCLUDES ${BRAYNS_RESEARCH_MODULES_DIR})
list(APPEND ALL_ISPC_INCLUDES ${OSPRAY_INCLUDE_DIRS})
include_directories_ispc(${ALL_ISPC_INCLUDES})
ospray_ispc_compile(${MODULE_ISPC_SOURCES})
list(APPEND MODULE_SOURCES ${ISPC_OBJECTS})

# Compile C++ code
include_directories(
    ${BRAYNS_RESEARCH_MODULES_DIR}
    ${OSPRAY_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(MODULE_LINK_LIBRARIES
    PUBLIC ${OSPRAY_LIBRARIES})
add_library(${MODULE_NAME}
    SHARED ${MODULE_SOURCES})
target_link_libraries(
    ${MODULE_NAME}
    ${MODULE_LINK_LIBRARIES}
    braynsOSPRayEngine glm)

# Installation
install(TARGETS ${MODULE_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
