# Copyright 2015-2024 Blue Brain Project/EPFL
# Responsible Authors: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSENGINE_SOURCES
  Camera.cpp
  Engine.cpp
  FrameBuffer.cpp
  FrameExporter.cpp
  Light.cpp
  LightManager.cpp
  Material.cpp
  Model.cpp
  Renderer.cpp
  Scene.cpp
  SharedDataVolume.cpp
  Volume.cpp
)

set(BRAYNSENGINE_PUBLIC_HEADERS
  BrickedVolume.h
  Camera.h
  Engine.h
  FrameBuffer.h
  FrameExporter.h
  Light.h
  LightManager.h
  Material.h
  Model.h
  Renderer.h
  Scene.h
  SharedDataVolume.h
  Volume.h
)

add_library(braynsEngine SHARED ${BRAYNSENGINE_SOURCES} ${BRAYNSENGINE_PUBLIC_HEADERS})

target_include_directories(braynsEngine PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsEngine PUBLIC braynsCommon)

target_compile_options(braynsEngine PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsEngine
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsEngine
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
