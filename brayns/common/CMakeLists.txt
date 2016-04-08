# Copyright (c) 2011-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of BRayns

set(BRAYNSCOMMON_SOURCES
  ui/Viewport.cpp
  ui/BaseWindow.cpp
  ui/manipulators/AbstractManipulator.cpp
  ui/manipulators/InspectCenterManipulator.cpp
  ui/manipulators/FlyingModeManipulator.cpp
  parameters/ParametersManager.cpp
  parameters/AbstractParameters.cpp
  parameters/RenderingParameters.cpp
  parameters/GeometryParameters.cpp
  parameters/ApplicationParameters.cpp
  parameters/SceneParameters.cpp
  camera/Camera.cpp
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
  ui/Viewport.h
  ui/BaseWindow.h
  ui/manipulators/AbstractManipulator.h
  ui/manipulators/InspectCenterManipulator.h
  ui/manipulators/FlyingModeManipulator.h
  parameters/ParametersManager.h
  parameters/AbstractParameters.h
  parameters/RenderingParameters.h
  parameters/GeometryParameters.h
  parameters/ApplicationParameters.h
  parameters/SceneParameters.h
  camera/Camera.h
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
    PUBLIC boost_filesystem vmmlib boost_program_options
    PRIVATE ${GLUT_LIBRARIES} Brion
)

if(TARGET ZeroBuf)
  list(APPEND BRAYNSCOMMON_LINK_LIBRARIES ZeroBuf ZeroBufRender)
endif()

common_library(braynsCommon)
