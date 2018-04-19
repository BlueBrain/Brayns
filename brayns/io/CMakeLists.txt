# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSIO_SOURCES
  algorithms/MetaballsGenerator.cpp
  ImageManager.cpp
  MeshLoader.cpp
  MolecularSystemReader.cpp
  ProteinLoader.cpp
  SceneLoader.cpp
  simulation/CADiffusionSimulationHandler.cpp
  simulation/SpikeSimulationHandler.cpp
  TransferFunctionLoader.cpp
  XYZBLoader.cpp
)

set(BRAYNSIO_PUBLIC_HEADERS
  algorithms/MetaballsGenerator.h
  ImageManager.h
  MeshLoader.h
  MolecularSystemReader.h
  ProgressReporter.h
  ProteinLoader.h
  SceneLoader.h
  simulation/CADiffusionSimulationHandler.h
  simulation/SpikeSimulationHandler.h
  TransferFunctionLoader.h
  XYZBLoader.h
)

set(BRAYNSIO_LINK_LIBRARIES
  PRIVATE braynsParameters braynsCommon vmmlib
    ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${ASSIMP_LIBRARIES} ${assimp_LIBRARIES}
)

if(BRAYNS_BRION_ENABLED)
  list(APPEND BRAYNSIO_SOURCES
      ConnectivityLoader.cpp
      MorphologyLoader.cpp
      NESTLoader.cpp
      simulation/CircuitSimulationHandler.cpp)
  list(APPEND BRAYNSIO_HEADERS
      ConnectivityLoader.h
      MorphologyLoader.h
      NESTLoader.h
      simulation/CircuitSimulationHandler.h)
  list(APPEND BRAYNSIO_LINK_LIBRARIES
      PRIVATE Brion Brain ${HDF5_LIBRARIES})
endif()

if(BRAYNS_IMAGEMAGICK_ENABLED)
  list(APPEND BRAYNSIO_LINK_LIBRARIES PRIVATE ${Magick++_LIBRARIES})
endif()

set(BRAYNSIO_INCLUDE_NAME brayns/io)
common_library(braynsIO)
