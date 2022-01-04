# Copyright (c) 2015-2021, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSIO_SOURCES
    LoaderRegistry.cpp
    loaders/MeshLoader.cpp
    loaders/ProteinLoader.cpp
    loaders/VolumeLoader.cpp
    loaders/XYZBLoader.cpp
    loaders/mesh/parsers/ObjMeshParser.cpp
    loaders/mesh/parsers/PlyMeshParser.cpp
    loaders/mesh/parsers/helpers/StringHelper.cpp
    loaders/mesh/parsers/helpers/StreamHelper.cpp
)

set(BRAYNSIO_PUBLIC_HEADERS
    Loader.h
    LoaderRegistry.h
    loaders/MeshLoader.h
    loaders/ProteinLoader.h
    loaders/VolumeLoader.h
    loaders/XYZBLoader.h
)

add_library(braynsIO SHARED ${BRAYNSIO_SOURCES} ${BRAYNSIO_PUBLIC_HEADERS})

target_include_directories(braynsIO PUBLIC
   "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
   "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsIO PRIVATE
    braynsParameters
    braynsCommon
    braynsEngine
)

target_compile_options(braynsIO PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsIO
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsIO
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
