# Copyright (c) HBP 2014-2015 ahmet.bilgili@epfl.ch
# All rights reserved. Do not distribute without further notice.

add_subdirectory(common)

set(BRAYNS_PUBLIC_HEADERS
  Brayns.h)

set(BRAYNS_SOURCES
  Brayns.cpp)

set(BRAYNS_HEADERS
  Brayns.h)

set(BRAYNS_LINK_LIBRARIES
  PUBLIC braynsCommon braynsPlugins
  ${OSPRAY_DEPENDENT_LIBRARIES} ${OSPRAY_LIBRARIES}
)

common_library(brayns)
