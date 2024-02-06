# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

file(GLOB_RECURSE BRAYNSUTILS_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.h)
file(GLOB_RECURSE BRAYNSUTILS_SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)

add_library(braynsUtils SHARED ${BRAYNSUTILS_SOURCES} ${BRAYNSUTILS_HEADERS})

set(BRAYNSUTILS_INCLUDE_DIRS_LIST
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/include
)

target_include_directories(braynsUtils PUBLIC
    "$<BUILD_INTERFACE:${BRAYNSUTILS_INCLUDE_DIRS_LIST}>"
    "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(braynsUtils PUBLIC
    rkcommon::rkcommon
    spdlog::spdlog
)

target_link_libraries(braynsUtils PRIVATE stb tinyexr ${ZLIB_LIBRARIES} ${CMAKE_DL_LIBS})

# Needed to allow DynamicLib class to find the plugins libraries
set_target_properties(braynsUtils PROPERTIES BUILD_RPATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

target_compile_options(braynsUtils PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsUtils
    PROPERTIES
    VERSION ${BRAYNS_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsUtils
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
