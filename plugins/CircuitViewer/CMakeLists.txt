# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(BraynsCircuitViewer VERSION 1.0.0)
set(BraynsCircuitViewer_VERSION_ABI 1)

include(Common)

common_find_package(Brion REQUIRED)
common_find_package(ospray 1.8 SYSTEM)
# For unit tests
common_find_package(BBPTestData)
common_find_package_post()

set(BRAYNSCIRCUITVIEWER_HEADERS
  CircuitViewer.h
  io/CircuitLoader.h
  io/ModelData.h
  io/MorphologyLoader.h
  io/SimulationHandler.h
)

set(BRAYNSCIRCUITVIEWER_SOURCES
  CircuitViewer.cpp
  io/CircuitLoader.cpp
  io/MorphologyLoader.cpp
  io/SimulationHandler.cpp
  io/utils.h
)

set(BRAYNSCIRCUITVIEWER_LINK_LIBRARIES PRIVATE braynsCommon braynsEngine
  braynsParameters braynsPluginAPI Brain ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY})

if(OSPRAY_FOUND)
  list(APPEND BRAYNSCIRCUITVIEWER_HEADERS
    renderer/AdvancedSimulationRenderer.h
    renderer/BasicSimulationRenderer.h
    renderer/SimulationMaterial.h
    renderer/SimulationRenderer.h
  )
  list(APPEND BRAYNSCIRCUITVIEWER_SOURCES
    renderer/AdvancedSimulationRenderer.cpp
    renderer/BasicSimulationRenderer.cpp
    renderer/SimulationMaterial.cpp
    renderer/SimulationRenderer.cpp
    renderer/SimulationRenderer.ih
  )
  set(BRAYNSCIRCUITVIEWER_ISPC_SOURCES
    renderer/AdvancedSimulationRenderer.ispc
    renderer/BasicSimulationRenderer.ispc
    renderer/SimulationMaterial.ispc
    renderer/SimulationRenderer.ispc
  )
  list(APPEND BRAYNSCIRCUITVIEWER_SOURCES ${BRAYNSCIRCUITVIEWER_ISPC_SOURCES})

  # reuse ispc setup and macros from ospray
  list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(OSPRAY_DEBUG_BUILD ON)
  endif()
  include(ispc)

  # Compile ispc code
  include_directories_ispc(${PROJECT_SOURCE_DIR}/../../ ${OSPRAY_INCLUDE_DIRS})
  ospray_ispc_compile(${BRAYNSCIRCUITVIEWER_ISPC_SOURCES})
  list(APPEND BRAYNSCIRCUITVIEWER_SOURCES ${ISPC_OBJECTS})

  list(APPEND BRAYNSCIRCUITVIEWER_LINK_LIBRARIES braynsOSPRayEngine ${OSPRAY_LIBRARIES})
endif()

set(BRAYNSCIRCUITVIEWER_OMIT_LIBRARY_HEADER ON)
set(BRAYNSCIRCUITVIEWER_OMIT_VERSION_HEADERS ON)
set(BRAYNSCIRCUITVIEWER_OMIT_EXPORT ON)
common_library(braynsCircuitViewer)

add_subdirectory(tests)

if (TARGET Brayns-all)
  add_dependencies(Brayns-all BraynsCircuitViewer-all)
  add_dependencies(Brayns-install BraynsCircuitViewer-install)
endif()
