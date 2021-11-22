# Copyright (c) 2015-2021, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSIO_SOURCES
    MeshLoader.cpp
    ProteinLoader.cpp
    VolumeLoader.cpp
    XYZBLoader.cpp
    assimpImporters/ObjFileImporter.cpp
    assimpImporters/ObjFileParser.cpp
    assimpImporters/ObjFileMtlImporter.cpp
)

set(BRAYNSIO_PUBLIC_HEADERS
    MeshLoader.h
    ProteinLoader.h
    VolumeLoader.h
    XYZBLoader.h
)

if(assimp_VERSION VERSION_GREATER_EQUAL 4.1.0)
    list(APPEND BRAYNSIO_SOURCES assimpImporters/PlyLoader.cpp assimpImporters/PlyParser.cpp)
    set_source_files_properties(assimpImporters/PlyLoader.cpp PROPERTIES COMPILE_FLAGS -Wno-shadow)
endif()

if(assimp_VERSION VERSION_LESS 4)
    list(APPEND BRAYNSIO_SOURCES assimp/DefaultIOStream.cpp)
endif()


add_library(braynsIO SHARED ${BRAYNSIO_SOURCES} ${BRAYNSIO_PUBLIC_HEADERS})

target_include_directories(braynsIO PUBLIC
   "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
   "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsIO PRIVATE
    braynsParameters braynsCommon braynsEngine ${ASSIMP_LIBRARIES} ${assimp_LIBRARIES}
)

if(assimp_VERSION VERSION_GREATER_EQUAL 4.1.0)
    target_compile_definitions(braynsIO PRIVATE USE_CUSTOM_PLY_IMPORTER)
endif()

if(assimp_VERSION VERSION_GREATER_EQUAL 5.0.0)
    target_compile_definitions(braynsIO PRIVATE ASSIMP_VERSION_5_0_1)
endif()

if(assimp_VERSION VERSION_LESS 4)
    target_include_directories(braynsIO PRIVATE .)
    target_compile_definitions(braynsIO PRIVATE ASSIMP_VERSION_3)
elseif(assimp_VERSION VERSION_GREATER_EQUAL 5.0.0)
    target_compile_definitions(braynsIO PRIVATE ASSIMP_VERSION_5)
endif()

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
