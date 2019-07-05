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

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wshadow -Wclass-memaccess")
if(CIRCUIT_EXPLORER_USE_DB)
  find_package(PQXX REQUIRED SYSTEM)
endif()

common_find_package(Brayns REQUIRED)
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

if(CIRCUIT_EXPLORER_USE_DB)
  list(APPEND ${NAME}_SOURCES
     io/db/DBConnector.cpp
     io/db/DBVoltageSimulationHandler.cpp
#      io/db/MorphologyLoader.cpp
   )
else()
  list(APPEND ${NAME}_SOURCES
    io/file/VoltageSimulationHandler.cpp
    io/file/CellGrowthHandler.cpp
    io/file/SpikeSimulationHandler.cpp
    io/file/MorphologyCollageLoader.cpp
    io/file/PairSynapsesLoader.cpp
    io/file/AbstractCircuitLoader.cpp
    io/file/MeshCircuitLoader.cpp
    io/file/AdvancedCircuitLoader.cpp
    io/file/SynapseCircuitLoader.cpp
    io/file/BrickLoader.cpp
    io/file/MorphologyLoader.cpp
    io/file/SynapseJSONLoader.cpp
    io/file/Utils.cpp
  )
endif()

set(${NAME}_PUBLIC_HEADERS
  api/CircuitExplorerParams.h
  meshing/MetaballsGenerator.h
  meshing/PointCloudMesher.h
  CircuitExplorerPlugin.h
)

if(CIRCUIT_EXPLORER_USE_DB)
  list(APPEND ${NAME}_PUBLIC_HEADERS
    io/db/DBConnector.h
    io/db/DBVoltageSimulationHandler.h
#    io/db/MorphologyLoader.h
  )
else()
  list(APPEND ${NAME}_PUBLIC_HEADERS
    io/db/DBConnector.h
    io/db/DBVoltageSimulationHandler.h
    io/file/CellGrowthHandler.h
    io/file/VoltageSimulationHandler.h
    io/file/SpikeSimulationHandler.h
    io/file/BrickLoader.h
    io/file/AbstractCircuitLoader.h
    io/file/PairSynapsesLoader.h
    io/file/MeshCircuitLoader.h
    io/file/MorphologyCollageLoader.h
    io/file/AdvancedCircuitLoader.h
    io/file/SynapseCircuitLoader.h
    io/file/MorphologyLoader.h
    io/file/SynapseJSONLoader.h
    io/file/Utils.h
  )
endif()

set(${NAME}_LINK_LIBRARIES
  braynsParameters braynsPluginAPI pqxx
  ${FREEIMAGE_LIBRARIES} ${HDF5_LIBRARIES} )

if(CIRCUIT_EXPLORER_USE_DB)
  list(APPEND ${NAME}_LINK_LIBRARIES
    ${MORPHIO_LIBRARIES}
  )
else()
  list(APPEND ${NAME}_LINK_LIBRARIES
    vmmlib Brion Brain
  )
endif()

if(CGAL_FOUND)
    list(APPEND COMMON_FIND_PACKAGE_DEFINES BRAYNS_USE_CGAL)
    list(APPEND ${NAME}_LINK_LIBRARIES ${CGAL_LIBRARY} gmp)
endif()

set(${NAME}_OMIT_LIBRARY_HEADER ON)
set(${NAME}_OMIT_VERSION_HEADERS ON)
set(${NAME}_OMIT_EXPORT ON)
common_library(${LIBRARY_NAME})
