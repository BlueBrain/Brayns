# Copyright (c) HBP 2014-2015 cyrille.favreau@epfl.ch
# All rights reserved. Do not distribute without further notice.

if(THIS_IS_MIC)
  return()
endif()

set(BRAYNSVIEWER_SOURCES
  BraynsViewer.cpp
  main.cpp
)

set(BRAYNSVIEWER_HEADERS
  BraynsViewer.h
)

set(BRAYNSVIEWER_LINK_LIBRARIES
  brayns
  braynsCommon
  braynsPlugins
  braynsOSPRayPlugin
  ${OSPRAY_DEPENDENT_LIBRARIES}
  ${OSPRAY_LIBRARIES}
  ${GLUT_LIBRARIES}
)

if(ASSIMP_FOUND)
  list(APPEND BRAYNSVIEWER_LINK_LIBRARIES
    ${ASSIMP_LIBRARIES}
  )
endif()

common_application(braynsViewer)
