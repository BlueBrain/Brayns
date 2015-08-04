# Copyright (c) HBP 2014-2015 cyrille.favreau@epfl.ch
# All rights reserved. Do not distribute without further notice.

set(ASSIMP_FOUND FALSE)
set(ASSIMP_NAME assimp)

set(ASSIMP_ROOT $ENV{ASSIMP_ROOT})

find_path(ASSIMP_INCLUDE_DIRS
  NAMES scene.h
  HINTS ${ASSIMP_ROOT}/include
  /usr/include/
)

find_library(ASSIMP_LIBRARIES
  NAMES ${ASSIMP_NAME}
  HINTS ${ASSIMP_ROOT}/lib
  /usr/local/lib/
  /usr/lib/
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(assimp DEFAULT_MSG
                                  ASSIMP_LIBRARIES
                                  ASSIMP_INCLUDE_DIR)
