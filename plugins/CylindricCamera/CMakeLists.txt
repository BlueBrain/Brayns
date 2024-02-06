# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

file(GLOB_RECURSE BRAYNSCYLINDRICCAMERA_SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)
file(GLOB_RECURSE BRAYNSCYLINDRICCAMERA_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.h)

add_library(braynsCylindricCamera SHARED ${BRAYNSCYLINDRICCAMERA_SOURCES} ${BRAYNSCYLINDRICCAMERA_HEADERS})

target_include_directories(braynsCylindricCamera PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
    "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsCylindricCamera PRIVATE
    braynsEngine
    braynsPluginAPI
    braynsNetwork
    ospray::ospray
    ospray::ospray_sdk
    ospray::ospray_module_cpu)

target_compile_options(braynsCylindricCamera PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsCylindricCamera
    PROPERTIES
    VERSION ${BRAYNS_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsCylindricCamera
    EXPORT braynsCylindricCamera-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsCylindricCamera-targets
    DESTINATION share/braynsCylindricCamera/CMake
)
