# Copyright (c) HBP 2014-2015 cyrille.favreau@epfl.ch
# All rights reserved. Do not distribute without further notice.

set(BRAYNSCOMMON_SOURCES
  ui/BaseWindow.cpp
  ui/manipulators/AbstractManipulator.cpp
  ui/manipulators/InspectCenterManipulator.cpp
  ui/manipulators/FlyingModeManipulator.cpp
  loaders/MorphologyLoader.cpp
  loaders/ProteinLoader.cpp
  loaders/TextureLoader.cpp
  algorithms/MetaballsGenerator.cpp
  parameters/AbstractParameters.cpp
  parameters/RenderingParameters.cpp
  parameters/GeometryParameters.cpp
  parameters/ApplicationParameters.cpp
  extensions/ExtensionController.cpp
)

set(BRAYNSCOMMON_HEADERS
  exceptions.h
  log.h
  ui/BaseWindow.h
  ui/manipulators/AbstractManipulator.h
  ui/manipulators/InspectCenterManipulator.h
  ui/manipulators/FlyingModeManipulator.h
  loaders/MorphologyLoader.h
  loaders/ProteinLoader.h
  loaders/TextureLoader.h
  algorithms/MetaballsGenerator.h
  parameters/AbstractParameters.h
  parameters/RenderingParameters.h
  parameters/GeometryParameters.h
  parameters/ApplicationParameters.h
  extensions/ExtensionController.h
)

set(BRAYNSCOMMON_LINK_LIBRARIES
  ${GLUT_LIBRARIES}
  ${OSPRAY_LIBRARIES}
)

if(ASSIMP_FOUND)
  list(APPEND BRAYNSCOMMON_SOURCES
    loaders/MeshLoader.cpp
  )
  list(APPEND BRAYNSCOMMON_HEADERS
    loaders/MeshLoader.h
  )
  list(APPEND BRAYNSCOMMON_LINK_LIBRARIES ${ASSIMP_LIBRARIES})
endif()

if(DEFLECT_FOUND)
  list(APPEND BRAYNSCOMMON_SOURCES
    extensions/DeflectManager.cpp
  )
  list(APPEND BRAYNSCOMMON_HEADERS
    extensions/DeflectManager.h
  )
  list(APPEND BRAYNSCOMMON_LINK_LIBRARIES Deflect)
endif()

if(RESTBRIDGE_FOUND)
  list(APPEND BRAYNSCOMMON_LINK_LIBRARIES RESTBridge ${LibJpegTurbo_LIBRARIES})
endif()

if(ZEQ_FOUND)
  list(APPEND BRAYNSCOMMON_LINK_LIBRARIES zeq zeqHBP)
endif()

if(BBPSDK_FOUND)
  list(APPEND BRAYNSCOMMON_LINK_LIBRARIES BBPSDK)
endif()

common_library(braynsCommon)
