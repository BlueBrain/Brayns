# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSIO_SOURCES
  ProteinLoader.cpp
  VolumeLoader.cpp
  XYZBLoader.cpp
)

set(BRAYNSIO_PUBLIC_HEADERS
  ProteinLoader.h
  VolumeLoader.h
  XYZBLoader.h
)

set(BRAYNSIO_LINK_LIBRARIES
  PRIVATE braynsParameters braynsCommon braynsEngine
)

if(BRAYNS_LIBARCHIVE_ENABLED)
  list(APPEND BRAYNSIO_SOURCES ArchiveLoader.cpp)
  list(APPEND BRAYNSIO_HEADERS ArchiveLoader.h)
  list(APPEND BRAYNSIO_LINK_LIBRARIES
    PRIVATE ${LibArchive_LIBRARIES})
endif()

if(BRAYNS_ASSIMP_ENABLED)
  list(APPEND BRAYNSIO_SOURCES MeshLoader.cpp)
  list(APPEND BRAYNSIO_PUBLIC_HEADERS MeshLoader.h)
  list(APPEND BRAYNSIO_LINK_LIBRARIES
    PRIVATE ${ASSIMP_LIBRARIES} ${assimp_LIBRARIES})
endif()

set(BRAYNSIO_INCLUDE_NAME brayns/io)
common_library(braynsIO)

if(BRAYNS_ASSIMP_ENABLED)
  target_compile_definitions(braynsIO PUBLIC BRAYNS_USE_ASSIMP)
endif()