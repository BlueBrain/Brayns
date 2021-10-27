# Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

set(BRAYNSUTILS_INCLUDE_NAME brayns/utils)
set(BRAYNSUTILS_LINK_LIBRARIES
  PUBLIC ${LibJpegTurbo_LIBRARIES}
  PRIVATE  ${FREEIMAGE_LIBRARIES}
)

common_library(braynsUtils)

# Needed to allow DynamicLib class to find the engine and plugins libraries
set_target_properties(braynsUtils PROPERTIES BUILD_RPATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

target_include_directories(braynsUtils SYSTEM PUBLIC ${FREEIMAGE_INCLUDE_DIRS})
