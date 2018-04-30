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
  renderer/Renderer.cpp
  renderer/FrameBuffer.cpp
  light/Light.cpp
  light/PointLight.cpp
  light/DirectionalLight.cpp
  loader/LoaderRegistry.cpp
  utils/Utils.cpp
)

set(BRAYNSCOMMON_PUBLIC_HEADERS
  ActionInterface.h
  BaseObject.h
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
  loader/Loader.h
  loader/LoaderRegistry.h
  log.h
  material/Material.h
  material/Texture2D.h
  mathTypes.h
  Progress.h
  renderer/FrameBuffer.h
  renderer/Renderer.h
  scene/Scene.h
  simulation/AbstractSimulationHandler.h
  Statistics.h
  Timer.h
  transferFunction/TransferFunction.h
  tasks/Task.h
  tasks/TaskFunctor.h
  tasks/TaskRuntimeError.h
  types.h
  utils/Utils.h
  volume/VolumeHandler.h
)

set(BRAYNSCOMMON_INCLUDE_NAME brayns/common)
set(BRAYNSCOMMON_LINK_LIBRARIES
  PUBLIC vmmlib Async++
  PRIVATE braynsParameters ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
)

common_library(braynsCommon)
