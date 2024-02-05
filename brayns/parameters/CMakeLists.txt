# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSPARAMETERS_SOURCES
  ParametersManager.cpp
  AbstractParameters.cpp
  AnimationParameters.cpp
  RenderingParameters.cpp
  ApplicationParameters.cpp
  VolumeParameters.cpp
  NetworkParameters.cpp
)

set(BRAYNSPARAMETERS_PUBLIC_HEADERS
  ParametersManager.h
  AbstractParameters.h
  AnimationParameters.h
  RenderingParameters.h
  ApplicationParameters.h
  VolumeParameters.h
  NetworkParameters.h
)

add_library(braynsParameters SHARED ${BRAYNSPARAMETERS_SOURCES} ${BRAYNSPARAMETERS_PUBLIC_HEADERS})

set(BRAYNSPARAMETER_INCLUDE_DIRS_LIST
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/include
)
target_include_directories(braynsParameters PUBLIC
  "$<BUILD_INTERFACE:${BRAYNSPARAMETER_INCLUDE_DIRS_LIST}>"
  "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsParameters PUBLIC spdlog::spdlog)
target_link_libraries(braynsParameters PRIVATE glm ${Boost_PROGRAM_OPTIONS_LIBRARY} braynsUtils)

target_compile_options(braynsParameters PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsParameters
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsParameters
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
