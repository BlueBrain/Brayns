# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSCOMMON_SOURCES
  engine/Engine.cpp
  input/KeyboardHandler.cpp
  volume/VolumeHandler.cpp
  transferFunction/TransferFunction.cpp
  simulation/AbstractSimulationHandler.cpp
  simulation/CircuitSimulationHandler.cpp
  simulation/SpikeSimulationHandler.cpp
  camera/AbstractManipulator.cpp
  camera/Camera.cpp
  camera/FlyingModeManipulator.cpp
  camera/InspectCenterManipulator.cpp
  scene/Scene.cpp
  geometry/Primitive.cpp
  geometry/Geometry.cpp
  geometry/Sphere.cpp
  geometry/Cylinder.cpp
  geometry/Cone.cpp
  geometry/TrianglesMesh.cpp
  material/Material.cpp
  material/Texture2D.cpp
  renderer/Renderer.cpp
  renderer/FrameBuffer.cpp
  light/Light.cpp
  light/PointLight.cpp
  light/DirectionalLight.cpp
)

set(BRAYNSCOMMON_PUBLIC_HEADERS
  exceptions.h
  log.h
  engine/Engine.h
  input/KeyboardHandler.h
  volume/VolumeHandler.h
  simulation/AbstractSimulationHandler.h
  simulation/CircuitSimulationHandler.h
  simulation/SpikeSimulationHandler.h
  transferFunction/TransferFunction.h
  camera/AbstractManipulator.h
  camera/Camera.h
  camera/FlyingModeManipulator.h
  camera/InspectCenterManipulator.h
  scene/Scene.h
  geometry/Primitive.h
  geometry/Geometry.h
  geometry/Sphere.h
  geometry/Cylinder.h
  geometry/Cone.h
  geometry/TrianglesMesh.h
  material/Material.h
  material/Texture2D.h
  renderer/Renderer.h
  renderer/FrameBuffer.h
  light/Light.h
  light/PointLight.h
  light/DirectionalLight.h
)

set(BRAYNSCOMMON_LINK_LIBRARIES
    PUBLIC braynsParameters Servus boost_filesystem vmmlib boost_system
)

if(TARGET Lexis AND TARGET ZeroBuf)
  add_subdirectory(vocabulary)
  list(APPEND BRAYNSCOMMON_LINK_LIBRARIES Lexis ZeroBuf BraynsZeroBufRender)
endif()

common_library(braynsCommon)
