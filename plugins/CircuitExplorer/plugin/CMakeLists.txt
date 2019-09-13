cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(NAME BRAYNSCIRCUITEXPLORER)
set(LIBRARY_NAME braynsCircuitExplorer)
project(${NAME} VERSION 0.1.0)
set(${NAME}_VERSION_ABI 1)

include(Common)

include_directories(
    SYSTEM
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../MorphIO/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../MorphIO/3rdparty/GSL_LITE/include)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wshadow")

common_find_package(CGAL)
common_find_package(OpenMP)
common_find_package(libpqxx REQUIRED)
common_find_package_post()

set(${NAME}_SOURCES
  api/CircuitExplorerParams.cpp
  meshing/MetaballsGenerator.cpp
  meshing/PointCloudMesher.cpp
  CircuitExplorerPlugin.cpp
)

list(APPEND ${NAME}_SOURCES
  io/VoltageSimulationHandler.cpp
  io/CellGrowthHandler.cpp
  io/SpikeSimulationHandler.cpp
  io/MorphologyCollageLoader.cpp
  io/PairSynapsesLoader.cpp
  io/AbstractCircuitLoader.cpp
  io/MeshCircuitLoader.cpp
  io/AdvancedCircuitLoader.cpp
  io/SynapseCircuitLoader.cpp
  io/BrickLoader.cpp
  io/MorphologyLoader.cpp
  io/SynapseJSONLoader.cpp
  io/Utils.cpp
)

set(${NAME}_PUBLIC_HEADERS
  api/CircuitExplorerParams.h
  meshing/MetaballsGenerator.h
  meshing/PointCloudMesher.h
  CircuitExplorerPlugin.h
)

list(APPEND ${NAME}_PUBLIC_HEADERS
  io/CellGrowthHandler.h
  io/VoltageSimulationHandler.h
  io/SpikeSimulationHandler.h
  io/BrickLoader.h
  io/AbstractCircuitLoader.h
  io/PairSynapsesLoader.h
  io/MeshCircuitLoader.h
  io/MorphologyCollageLoader.h
  io/AdvancedCircuitLoader.h
  io/SynapseCircuitLoader.h
  io/MorphologyLoader.h
  io/SynapseJSONLoader.h
  io/Utils.h
)

set(${NAME}_LINK_LIBRARIES
  braynsParameters braynsPluginAPI pqxx
  ${FREEIMAGE_LIBRARIES} ${HDF5_LIBRARIES} )

list(APPEND ${NAME}_LINK_LIBRARIES
  vmmlib Brion Brain
)

if(CGAL_FOUND)
    list(APPEND COMMON_FIND_PACKAGE_DEFINES BRAYNS_USE_CGAL)
    list(APPEND ${NAME}_LINK_LIBRARIES ${CGAL_LIBRARY} gmp)
endif()

set(${NAME}_OMIT_LIBRARY_HEADER ON)
set(${NAME}_OMIT_VERSION_HEADERS ON)
set(${NAME}_OMIT_EXPORT ON)
common_library(${LIBRARY_NAME})
