# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# Responsible Authors: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
# Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

file(GLOB_RECURSE BRAYNSENGINE_SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)
file(GLOB_RECURSE BRAYNSENGINE_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.h)

add_library(braynsEngine SHARED ${BRAYNSENGINE_SOURCES} ${BRAYNSENGINE_HEADERS})

target_include_directories(braynsEngine PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsEngine PUBLIC
    braynsJson
    braynsParameters
    ospray::ospray
    ospray::ospray_sdk
    ospray::ospray_module_cpu
)

target_compile_options(braynsEngine PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsEngine
    PROPERTIES
    VERSION ${BRAYNS_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsEngine
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
