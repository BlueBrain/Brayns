# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSCOMMON_SOURCES
  engine/Engine.cpp
  simulation/AbstractSimulationHandler.cpp
  input/KeyboardHandler.cpp
  volume/VolumeHandler.cpp
  transferFunction/TransferFunction.cpp
  camera/AbstractManipulator.cpp
  camera/Camera.cpp
  camera/FlyingModeManipulator.cpp
  camera/InspectCenterManipulator.cpp
  scene/Scene.cpp
  material/Material.cpp
  material/Texture2D.cpp
  Progress.cpp
  renderer/Renderer.cpp
  renderer/FrameBuffer.cpp
  light/Light.cpp
  light/PointLight.cpp
  light/DirectionalLight.cpp
  utils/Utils.cpp
)

set(BRAYNSCOMMON_PUBLIC_HEADERS
  simulation/AbstractSimulationHandler.h
  camera/AbstractManipulator.h
  camera/Camera.h
  camera/FlyingModeManipulator.h
  camera/InspectCenterManipulator.h
  engine/Engine.h
  exceptions.h
  geometry/Cone.h
  geometry/Cylinder.h
  geometry/Sphere.h
  geometry/TrianglesMesh.h
  input/KeyboardHandler.h
  light/DirectionalLight.h
  light/Light.h
  light/PointLight.h
  log.h
  material/Material.h
  material/Texture2D.h
  Progress.h
  renderer/FrameBuffer.h
  renderer/Renderer.h
  scene/Scene.h
  transferFunction/TransferFunction.h
  types.h
  volume/VolumeHandler.h
  utils/Utils.h
)

set(BRAYNSCOMMON_LINK_LIBRARIES
    PUBLIC braynsParameters vmmlib
      ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
)

common_library(braynsCommon)
