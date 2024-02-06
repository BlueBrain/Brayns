# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

file(GLOB_RECURSE BRAYNSPARAMETERS_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.h)
file(GLOB_RECURSE BRAYNSPARAMETERS_SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)

add_library(braynsParameters SHARED ${BRAYNSPARAMETERS_SOURCES} ${BRAYNSPARAMETERS_HEADERS})

target_include_directories(braynsParameters PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(braynsParameters PUBLIC braynsUtils)

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
