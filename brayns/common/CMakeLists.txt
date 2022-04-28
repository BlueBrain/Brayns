# Copyright (c) 2015-2022, EPFL/Blue Brain Project
# Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
#                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSCOMMON_SOURCES
    BaseObject.cpp
    Bounds.cpp
    Log.cpp
    Timer.cpp
    TransferFunction.cpp
    Transformation.cpp
)

set(BRAYNSCOMMON_PUBLIC_HEADERS
    BaseObject.h
    Blob.h
    Bounds.h
    IDFactory.h
    Log.h
    MathTypes.h
    PixelFormat.h
    Timer.h
    TransferFunction.h
    Transformation.h
)

add_library(braynsCommon SHARED ${BRAYNSCOMMON_PUBLIC_HEADERS} ${BRAYNSCOMMON_SOURCES})

target_include_directories(braynsCommon PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsCommon PUBLIC
  glm braynsUtils braynsJson spdlog::spdlog
)
target_link_libraries(braynsCommon PRIVATE
    braynsParameters ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

target_compile_options(braynsCommon PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsCommon
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsCommon
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
