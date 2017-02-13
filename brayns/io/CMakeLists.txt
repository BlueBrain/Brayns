# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSIO_SOURCES
  algorithms/MetaballsGenerator.cpp
  XYZBLoader.cpp
  TransferFunctionLoader.cpp
  MorphologyLoader.cpp
  ProteinLoader.cpp
  NESTLoader.cpp
  TextureLoader.cpp
)

set(BRAYNSIO_PUBLIC_HEADERS
  algorithms/MetaballsGenerator.h
  XYZBLoader.h
  TransferFunctionLoader.h
  MorphologyLoader.h
  ProteinLoader.h
  NESTLoader.h
  TextureLoader.h
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

if(TARGET Brion)
  list(APPEND BRAYNSIO_LINK_LIBRARIES PRIVATE Brion Brain ${HDF5_LIBRARIES})
endif()

if(MAGICK++_FOUND)
  list(APPEND BRAYNSIO_LINK_LIBRARIES PRIVATE ${Magick++_LIBRARIES})
endif()

common_library(braynsIO)
