# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSIO_SOURCES
  simulation/CADiffusionSimulationHandler.cpp
  simulation/SpikeSimulationHandler.cpp
  algorithms/MetaballsGenerator.cpp
  XYZBLoader.cpp
  TransferFunctionLoader.cpp
  ProteinLoader.cpp
  TextureLoader.cpp
  SceneLoader.cpp
)

set(BRAYNSIO_PUBLIC_HEADERS
  simulation/CADiffusionSimulationHandler.h
  simulation/SpikeSimulationHandler.h
  algorithms/MetaballsGenerator.h
  XYZBLoader.h
  TransferFunctionLoader.h
  ProteinLoader.h
  TextureLoader.h
  SceneLoader.h
)

set(BRAYNSIO_LINK_LIBRARIES
  PRIVATE braynsParameters braynsCommon vmmlib
    ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
)

if(ASSIMP_FOUND)
  list(APPEND BRAYNSIO_SOURCES MeshLoader.cpp MolecularSystemReader.cpp)
  list(APPEND BRAYNSIO_PUBLIC_HEADERS MeshLoader.h MolecularSystemReader.h)
  list(APPEND BRAYNSIO_LINK_LIBRARIES PRIVATE assimp)
endif()

if(Brion_FOUND)
  list(APPEND BRAYNSIO_SOURCES
      MorphologyLoader.cpp
      NESTLoader.cpp
      simulation/CircuitSimulationHandler.cpp)
  list(APPEND BRAYNSIO_HEADERS
      MorphologyLoader.h
      NESTLoader.h
      simulation/CircuitSimulationHandler.h)
  list(APPEND BRAYNSIO_LINK_LIBRARIES
      PUBLIC Servus
      PRIVATE Brion Brain ${HDF5_LIBRARIES})
endif()

if(MAGICK++_FOUND)
  list(APPEND BRAYNSIO_LINK_LIBRARIES PRIVATE ${Magick++_LIBRARIES})
endif()

common_library(braynsIO)
