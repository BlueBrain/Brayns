# Copyright (c) HBP 2014-2015 cyrille.favreau@epfl.ch
# All rights reserved. Do not distribute without further notice.

if(THIS_IS_MIC)
  return()
endif()

set(BRAYNSSERVICE_SOURCES
  BraynsService.cpp
  main.cpp
)

set(BRAYNSSERVICE_HEADERS
  BraynsService.h
)

set(BRAYNSSERVICE_LINK_LIBRARIES
  braynsCommon
  braynsKernels
  ${OSPRAY_DEPENDENT_LIBRARIES}
  ${OSPRAY_LIBRARIES}
)

if(ASSIMP_FOUND)
  list(APPEND BRAYNSSERVICE_LINK_LIBRARIES
    ${ASSIMP_LIBRARIES}
  )
endif()

common_application(braynsService)
