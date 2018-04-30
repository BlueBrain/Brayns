# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

# Configure the output directories. To allow IMPI to do its magic we
# will put *executables* into the (same) build directory, but tag
# mic-executables with ".mic". *libraries* cannot use the
# ".mic"-suffix trick, so we'll put libraries into separate
# directories (names 'intel64' and 'mic', respectively)

set(BRAYNSOSPRAYPLUGIN_ISPC_SOURCES
  ispc/camera/ClippedPerspectiveCamera.ispc
  ispc/render/utils/AbstractRenderer.ispc
  ispc/render/utils/RandomGenerator.ispc
  ispc/render/utils/SkyBox.ispc
  ispc/geometry/ExtendedCylinders.ispc
  ispc/geometry/ExtendedCones.ispc
  ispc/geometry/ExtendedSpheres.ispc
  ispc/render/ExtendedOBJMaterial.ispc
  ispc/render/BasicRenderer.ispc
  ispc/render/ProximityRenderer.ispc
  ispc/render/SimulationRenderer.ispc
  ispc/render/ParticleRenderer.ispc
)

set(BRAYNSOSPRAYENGINEPLUGIN_SOURCES
  ispc/camera/ClippedPerspectiveCamera.cpp
  ispc/render/utils/AbstractRenderer.cpp
  ispc/geometry/ExtendedCones.cpp
  ispc/geometry/ExtendedCylinders.cpp
  ispc/geometry/ExtendedSpheres.cpp
  ispc/render/ExtendedOBJMaterial.cpp
  ispc/render/BasicRenderer.cpp
  ispc/render/ProximityRenderer.cpp
  ispc/render/SimulationRenderer.cpp
  ispc/render/ParticleRenderer.cpp
  OSPRayEngine.cpp
  OSPRayScene.cpp
  OSPRayRenderer.cpp
  OSPRayFrameBuffer.cpp
  OSPRayCamera.cpp
)

set(BRAYNSOSPRAYENGINEPLUGIN_PUBLIC_HEADERS
  ispc/camera/ClippedPerspectiveCamera.h
  ispc/render/utils/AbstractRenderer.h
  ispc/geometry/ExtendedCones.h
  ispc/geometry/ExtendedCylinders.h
  ispc/geometry/ExtendedSpheres.h
  ispc/render/ExtendedOBJMaterial.h
  ispc/render/BasicRenderer.h
  ispc/render/ProximityRenderer.h
  ispc/render/SimulationRenderer.h
  ispc/render/ParticleRenderer.h
  OSPRayEngine.h
  OSPRayScene.h
  OSPRayRenderer.h
  OSPRayFrameBuffer.h
  OSPRayCamera.h
)

set_source_files_properties(OSPRayScene.cpp
  PROPERTIES COMPILE_FLAGS -Wno-deprecated-declarations)

set(BRAYNSOSPRAYENGINEPLUGIN_LINK_LIBRARIES
    PRIVATE vmmlib braynsCommon braynsParameters braynsIO ${OSPRAY_LIBRARIES}
)

# reuse ispc setup and macros from ospray
list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
include(ispc)

# Enable/disable use of hardware randomizer
option(BRAYNS_HARDWARE_RANDOMIZER_ENABLED "Enables hardware randomizer if supported by target CPU" OFF)
if(BRAYNS_HARDWARE_RANDOMIZER_ENABLED)
  set(ISPC_EXTRA_ARGS -DBRAYNS_ISPC_USE_HARDWARE_RANDOMIZER=ON)
endif()

# Compile ispc code
include_directories_ispc(${PROJECT_SOURCE_DIR} ${OSPRAY_INCLUDE_DIRS})
ospray_ispc_compile(${BRAYNSOSPRAYPLUGIN_ISPC_SOURCES})
list(APPEND BRAYNSOSPRAYENGINEPLUGIN_SOURCES ${ISPC_OBJECTS})

# Compile C++ code
set(BRAYNSOSPRAYENGINEPLUGIN_OMIT_LIBRARY_HEADER ON)
set(BRAYNSOSPRAYENGINEPLUGIN_OMIT_VERSION_HEADERS ON)

common_library(braynsOSPRayEnginePlugin)
