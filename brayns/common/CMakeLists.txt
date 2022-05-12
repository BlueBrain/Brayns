# Copyright (c) 2015-2022, EPFL/Blue Brain Project
# Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
#                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSCOMMON_SOURCES
    BaseObject.cpp
    Bounds.cpp
    Log.cpp
    TransferFunction.cpp
    Transformation.cpp
    parameters/AbstractParameters.cpp
    parameters/AnimationParameters.cpp
    parameters/ApplicationParameters.cpp
    parameters/NetworkParameters.cpp
    parameters/ParametersManager.cpp
)

set(BRAYNSCOMMON_PUBLIC_HEADERS
    BaseObject.h
    Blob.h
    Bounds.h
    Log.h
    MathTypes.h
    PixelFormat.h
    TransferFunction.h
    Transformation.h
    parameters/AbstractParameters.h
    parameters/AnimationParameters.h
    parameters/ApplicationParameters.h
    parameters/NetworkParameters.h
    parameters/ParametersManager.h
)

add_library(braynsCommon SHARED ${BRAYNSCOMMON_PUBLIC_HEADERS} ${BRAYNSCOMMON_SOURCES})

set(BRAYNSCOMMON_INCLUDE_DIRS_LIST
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/include
)

target_include_directories(braynsCommon PUBLIC
    "$<BUILD_INTERFACE:${BRAYNSCOMMON_INCLUDE_DIRS_LIST}>"
    "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsCommon PUBLIC
    glm
    braynsUtils
    braynsJson
    spdlog::spdlog
)
target_link_libraries(braynsCommon PRIVATE
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
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
