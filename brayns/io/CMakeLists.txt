# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

file(GLOB_RECURSE BRAYNSIO_SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)
file(GLOB_RECURSE BRAYNSIO_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.h)

add_library(braynsIO SHARED ${BRAYNSIO_SOURCES} ${BRAYNSIO_HEADERS})

target_include_directories(braynsIO PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsIO PRIVATE
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
