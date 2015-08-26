# Copyright (c) HBP 2014-2015 cyrille.favreau@epfl.ch
# All rights reserved. Do not distribute without further notice.
#
# Once done this will define:
#
#  ASSIMP_FOUND - system has Assimp
#  ASSIMP_INCLUDE_DIRS - the Assimp include directory
#  ASSIMP_LIBRARIES - Link these to use Assimp

set(ASSIMP_ROOT $ENV{ASSIMP_ROOT})

find_path(ASSIMP_INCLUDE_DIRS
  NAMES assimp/scene.h
  HINTS ${ASSIMP_ROOT}/include
  /usr/include
)

find_library(ASSIMP_LIBRARIES
  NAMES assimp
  HINTS ${ASSIMP_ROOT}/lib
  /usr/local/lib/
  /usr/lib/
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ASSIMP DEFAULT_MSG ASSIMP_INCLUDE_DIRS ASSIMP_LIBRARIES)
