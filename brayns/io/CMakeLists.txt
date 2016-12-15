# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSIO_SOURCES
  XYZBLoader.cpp
  TransferFunctionLoader.cpp
  MorphologyLoader.cpp
  MolecularSystemReader.cpp
  ProteinLoader.cpp
  NESTLoader.cpp
  TextureLoader.cpp
)

set(BRAYNSIO_PUBLIC_HEADERS
  XYZBLoader.h
  TransferFunctionLoader.h
  MorphologyLoader.h
  MolecularSystemReader.h
  ProteinLoader.h
  NESTLoader.h
  TextureLoader.h
)

set(BRAYNSIO_LINK_LIBRARIES
  PRIVATE braynsParameters braynsCommon Servus vmmlib boost_system boost_filesystem
)

if(ASSIMP_FOUND)
  list(APPEND BRAYNSIO_SOURCES MeshLoader.cpp)
  list(APPEND BRAYNSIO_PUBLIC_HEADERS MeshLoader.h)
  list(APPEND BRAYNSIO_LINK_LIBRARIES PRIVATE assimp)
endif()

if(TARGET Brion)
  list(APPEND BRAYNSIO_LINK_LIBRARIES PRIVATE Brion Brain ${HDF5_LIBRARIES})
endif()

if(MAGICK++_FOUND)
  list(APPEND BRAYNSIO_LINK_LIBRARIES PRIVATE ${Magick++_LIBRARIES})
endif()

common_library(braynsIO)
