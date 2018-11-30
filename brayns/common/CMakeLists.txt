# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSCOMMON_SOURCES
  ImageManager.cpp
  PropertyMap.cpp
  camera/AbstractManipulator.cpp
  camera/Camera.cpp
  camera/FlyingModeManipulator.cpp
  camera/InspectCenterManipulator.cpp
  engine/Engine.cpp
  input/KeyboardHandler.cpp
  light/DirectionalLight.cpp
  light/Light.cpp
  light/PointLight.cpp
  loader/LoaderRegistry.cpp
  material/Material.cpp
  material/Texture2D.cpp
  renderer/FrameBuffer.cpp
  renderer/Renderer.cpp
  scene/ClipPlane.cpp
  scene/Model.cpp
  scene/Scene.cpp
  simulation/AbstractSimulationHandler.cpp
  transferFunction/TransferFunction.cpp
  utils/base64/base64.cpp
  utils/DynamicLib.cpp
  utils/imageUtils.cpp
  utils/utils.cpp
  volume/SharedDataVolume.cpp
  volume/Volume.cpp
)

set(BRAYNSCOMMON_PUBLIC_HEADERS
  ActionInterface.h
  BaseObject.h
  ImageManager.h
  Progress.h
  PropertyMap.h
  PropertyObject.h
  Statistics.h
  Timer.h
  Transformation.h
  camera/AbstractManipulator.h
  camera/Camera.h
  camera/FlyingModeManipulator.h
  camera/InspectCenterManipulator.h
  engine/Engine.h
  geometry/CommonDefines.h
  geometry/Cone.h
  geometry/Cylinder.h
  geometry/SDFGeometry.h
  geometry/Sphere.h
  geometry/Streamline.h
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
  renderer/FrameBuffer.h
  renderer/Renderer.h
  scene/ClipPlane.h
  scene/Model.h
  scene/Scene.h
  simulation/AbstractSimulationHandler.h
  tasks/Task.h
  tasks/TaskFunctor.h
  tasks/TaskRuntimeError.h
  transferFunction/TransferFunction.h
  types.h
  utils/enumUtils.h
  utils/utils.h
  volume/BrickedVolume.h
  volume/SharedDataVolume.h
  volume/Volume.h
)

set(BRAYNSCOMMON_HEADERS
  utils/DynamicLib.h
  utils/base64/base64.h
  utils/imageUtils.h
)

set(BRAYNSCOMMON_INCLUDE_NAME brayns/common)
set(BRAYNSCOMMON_LINK_LIBRARIES
  PUBLIC vmmlib Async++
  PRIVATE braynsParameters ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    ${FREEIMAGE_LIBRARIES}
)

common_library(braynsCommon)
target_include_directories(braynsCommon SYSTEM PRIVATE ${FREEIMAGE_INCLUDE_DIRS})
