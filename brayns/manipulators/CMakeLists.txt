# Copyright (c) 2015-2021, EPFL/Blue Brain Project
# Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
#                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSMANIPULATORS_SOURCES
  AbstractManipulator.cpp
  FlyingModeManipulator.cpp
  InspectCenterManipulator.cpp
)

set(BRAYNSMANIPULATORS_PUBLIC_HEADERS
  AbstractManipulator.h
  FlyingModeManipulator.h
  InspectCenterManipulator.h
)

add_library(braynsManipulators SHARED ${BRAYNSMANIPULATORS_SOURCES} ${BRAYNSMANIPULATORS_PUBLIC_HEADERS})

target_include_directories(braynsManipulators PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(braynsManipulators PUBLIC braynsCommon)
target_link_libraries(braynsManipulators PRIVATE braynsEngine)

target_compile_options(braynsManipulators PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsManipulators
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsManipulators
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
