# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSIO_SOURCES
  algorithms/MetaballsGenerator.cpp
  MeshLoader.cpp
  MolecularSystemReader.cpp
  ProteinLoader.cpp
  simulation/CADiffusionSimulationHandler.cpp
  simulation/SpikeSimulationHandler.cpp
  TransferFunctionLoader.cpp
  VolumeLoader.cpp
  XYZBLoader.cpp
)

set(BRAYNSIO_PUBLIC_HEADERS
  algorithms/MetaballsGenerator.h
  MeshLoader.h
  MolecularSystemReader.h
  ProteinLoader.h
  simulation/CADiffusionSimulationHandler.h
  simulation/SpikeSimulationHandler.h
  TransferFunctionLoader.h
  VolumeLoader.h
  XYZBLoader.h
)

set(BRAYNSIO_HEADERS
  circuitLoaderCommon.h
)

set(BRAYNSIO_LINK_LIBRARIES
  PRIVATE braynsParameters braynsCommon vmmlib
    ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${ASSIMP_LIBRARIES} ${assimp_LIBRARIES}
)

if(BRAYNS_BRION_ENABLED)
  list(APPEND BRAYNSIO_SOURCES
    CircuitLoader.cpp
    MorphologyLoader.cpp
    NESTLoader.cpp
    simulation/CircuitSimulationHandler.cpp)
  list(APPEND BRAYNSIO_HEADERS
    CircuitLoader.h
    MorphologyLoader.h
    NESTLoader.h
    simulation/CircuitSimulationHandler.h)
  list(APPEND BRAYNSIO_LINK_LIBRARIES
    PUBLIC Brion PRIVATE Brain ${HDF5_LIBRARIES})
endif()

set(BRAYNSIO_OMIT_LIBRARY_HEADER ON)
set(BRAYNSIO_OMIT_VERSION_HEADERS ON)
set(BRAYNSIO_OMIT_EXPORT ON)
set(BRAYNSIO_INCLUDE_NAME brayns/io)
common_library(braynsIO)
