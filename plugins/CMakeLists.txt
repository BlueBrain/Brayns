# Copyright (c) HBP 2014-2015 cyrille.favreau@epfl.ch
# All rights reserved. Do not distribute without further notice.

set(BRAYNSPLUGINS_SOURCES
  loaders/MorphologyLoader.cpp
  loaders/ProteinLoader.cpp
  extensions/ExtensionController.cpp
)

set(BRAYNSPLUGINS_HEADERS
  loaders/MorphologyLoader.h
  loaders/ProteinLoader.h
  extensions/ExtensionController.h
)

set(BRAYNSPLUGINS_LINK_LIBRARIES
  ${GLUT_LIBRARIES}
  ${OSPRAY_LIBRARIES}
  ${BBPSDK_LIBRARIES}
)

if(ASSIMP_FOUND)
  list(APPEND BRAYNSPLUGINS_SOURCES
    loaders/MeshLoader.cpp
  )
  list(APPEND BRAYNSPLUGINS_HEADERS
    loaders/MeshLoader.h
  )
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES ${ASSIMP_LIBRARIES})
endif()

if(DEFLECT_FOUND)
  list(APPEND BRAYNSPLUGINS_SOURCES
    extensions/DeflectManager.cpp
  )
  list(APPEND BRAYNSPLUGINS_HEADERS
    extensions/DeflectManager.h
  )
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES Deflect)
endif()

if(RESTBRIDGE_FOUND)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES RESTBridge ${LibJpegTurbo_LIBRARIES})
endif()

if(ZEQ_FOUND)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES zeq zeqHBP)
endif()

if(BBPSDK_FOUND)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES BBPSDK)
endif()

if(IMAGEMAGICK_FOUND)
  list(APPEND BRAYNSPLUGINS_SOURCES
    loaders/TextureLoader.cpp
  )
  list(APPEND BRAYNSPLUGINS_HEADERS
    loaders/TextureLoader.h
  )
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES ${IMAGEMAGICK_LIBRARIES})
endif()


set( BRAYNS_LINK_LIBRARIES
  braynsPlugins
  ${OSPRAY_DEPENDENT_LIBRARIES}
  ${OSPRAY_LIBRARIES}
)

if( ASSIMP_FOUND )
  list(APPEND BRAYNS_LINK_LIBRARIES ${ASSIMP_LIBRARIES})
endif()

add_subdirectory(renderers/ospray)

common_library(braynsPlugins)
