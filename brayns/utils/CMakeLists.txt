# Copyright (c) 2015-2021, EPFL/Blue Brain Project
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSUTILS_HEADERS
  base64/base64.h
  DynamicLib.h
  EnumUtils.h
  Filesystem.h
  FunctorInfo.h
  ImageUtils.h
  StringUtils.h
)

set(BRAYNSUTILS_SOURCES
  base64/base64.cpp
  DynamicLib.cpp
  ImageUtils.cpp
  StringUtils.cpp
)

add_library(braynsUtils SHARED ${BRAYNSUTILS_SOURCES} ${BRAYNSUTILS_HEADERS})

target_include_directories(braynsUtils PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

target_include_directories(braynsUtils SYSTEM PUBLIC ${FREEIMAGE_INCLUDE_DIRS})

target_link_libraries(braynsUtils PUBLIC ${LibJpegTurbo_LIBRARIES})
target_link_libraries(braynsUtils PRIVATE ${FREEIMAGE_LIBRARIES})

# Needed to allow DynamicLib class to find the engine and plugins libraries
set_target_properties(braynsUtils PROPERTIES BUILD_RPATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

target_compile_options(braynsUtils PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsUtils
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsUtils
    EXPORT brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
