# Copyright (c) 2015-2022, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

file(GLOB_RECURSE BRAYNSCIRCUITEXPLORER_SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)
file(GLOB_RECURSE BRAYNSCIRCUITEXPLORER_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.h)

# Movie generation with FFmpeg
find_library(AVFORMAT_LIBRARY avformat)
find_library(AVCODEC_LIBRARY avcodec)
find_library(AVUTIL_LIBRARY avutil)
find_library(SWSCALE_LIBRARY swscale)

set(FFMPEG_FOUND ON)

if(NOT AVFORMAT_LIBRARY)
    message(STATUS "libavformat not found")
    set(FFMPEG_FOUND OFF)
endif()

if(NOT AVCODEC_LIBRARY)
    message(STATUS "libavcodec not found")
    set(FFMPEG_FOUND OFF)
endif()

if(NOT AVUTIL_LIBRARY)
    message(STATUS "libavutil not found")
    set(FFMPEG_FOUND OFF)
endif()

if(NOT SWSCALE_LIBRARY)
    message(STATUS "libswscale not found")
    set(FFMPEG_FOUND OFF)
endif()

add_library(braynsCircuitExplorer SHARED ${BRAYNSCIRCUITEXPLORER_SOURCES} ${BRAYNSCIRCUITEXPLORER_HEADERS})

set(CIRCUITEXPLORER_INCLUDEDIR_LIST ${CMAKE_CURRENT_LIST_DIR})

target_include_directories(braynsCircuitExplorer PUBLIC
  "$<BUILD_INTERFACE:${CIRCUITEXPLORER_INCLUDEDIR_LIST}>"
  "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsCircuitExplorer PRIVATE
    braynsCommon 
    braynsPluginAPI 
    braynsNetwork
    sonata::sonata_shared 
    MorphIO::morphio 
    Brion 
    Brain 
    HighFive
    MVDTool
)

if(FFMPEG_FOUND)
    target_compile_definitions(braynsCircuitExplorer PRIVATE BRAYNS_USE_FFMPEG)
    target_link_libraries(braynsCircuitExplorer PRIVATE
        ${AVFORMAT_LIBRARY}
        ${AVCODEC_LIBRARY}
        ${AVUTIL_LIBRARY}
        ${SWSCALE_LIBRARY})
endif()

set_target_properties(braynsCircuitExplorer
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

target_compile_options(braynsCircuitExplorer PRIVATE ${BRAYNS_COMPILE_OPTIONS})
# For Brion uint128_t
target_compile_options(braynsCircuitExplorer PRIVATE -Wno-deprecated-copy)

install(TARGETS braynsCircuitExplorer
    EXPORT braynsCircuitExplorer-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h")

install(EXPORT braynsCircuitExplorer-targets
    DESTINATION share/braynsCircuitExplorer/CMake
)

