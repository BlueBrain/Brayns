#
# Copyright (c) 2011-2015, EPFL/Blue Brain Project
#                          Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of BRayns
#

# Configure the output directories. To allow IMPI to do its magic we
# will put *executables* into the (same) build directory, but tag
# mic-executables with ".mic". *libraries* cannot use the
# ".mic"-suffix trick, so we'll put libraries into separate
# directories (names 'intel64' and 'mic', respectively)
MACRO(get_ispc_target_list)
  IF(NOT BRAYNS_TARGET STREQUAL "mic")
    IF (BRAYNS_BUILD_ISA STREQUAL "ALL")
      # ------------------------------------------------------------------
      # in 'all' mode, we have a list of multiple targets for ispc,
      # and enable all targets for embree (we may still disable some
      # below if the compiler doesn't support them
      # ------------------------------------------------------------------
      SET(ISPC_TARGET_LIST sse4 avx avx2)

    ELSEIF (BRAYNS_BUILD_ISA STREQUAL "AVX2")
      # ------------------------------------------------------------------
      # in 'avx2' mode, we enable ONLY avx2 for ispc, and all targets
      # up to avx2 for embree. note that if the compiler doesn't
      # support AVX we will have a combination where embree uses AVX
      # (the most the compiler can do), while ispc still uses
      # avx. this works because both targets are 8 wide. it does
      # however require the compiler to understand AT LEAST AVX1.
      # ------------------------------------------------------------------
      SET(ISPC_TARGET_LIST avx2)

    ELSEIF (BRAYNS_BUILD_ISA STREQUAL "AVX")
      # ------------------------------------------------------------------
      # in 'avx' mode, we enable ONLY avx for ispc, and both sse and
      # avx for embree. note that this works ONLY works if the
      # compiler knows at least AVX
      # ------------------------------------------------------------------
      SET(ISPC_TARGET_LIST avx)

    ELSEIF (BRAYNS_BUILD_ISA STREQUAL "SSE")
      # ------------------------------------------------------------------
      # in 'sse' mode, we enable ONLY sse4 for ispc, and only sse for
      # embree
      # ------------------------------------------------------------------
      SET(ISPC_TARGET_LIST sse4)
    ELSE ()
      MESSAGE(ERROR "Invalid BRAYNS_BUILD_ISA value. Please select one of SSE, AVX, AVX2, or ALL.")
    ENDIF()

  ENDIF()

  IF (OSPRAY_MPI)
    ADD_DEFINITIONS(-DOSPRAY_MPI=1)
  ENDIF()
ENDMACRO()

set(BRAYNSOSPRAYPLUGIN_ISPC_SOURCES
  geometry/ExtendedCylinders.ispc
  geometry/ExtendedCones.ispc
  geometry/ExtendedSpheres.ispc
  camera/StereoCamera.ispc
  render/ExtendedOBJMaterial.ispc
  render/ExtendedOBJRenderer.ispc
  render/ProximityRenderer.ispc
)

set(BRAYNSOSPRAYPLUGIN_SOURCES
  camera/StereoCamera.cpp
  geometry/ExtendedCones.cpp
  geometry/ExtendedCylinders.cpp
  geometry/ExtendedSpheres.cpp
  render/ExtendedOBJMaterial.cpp
  render/ExtendedOBJRenderer.cpp
  render/ProximityRenderer.cpp
  render/OSPRayScene.cpp
  render/OSPRayRenderer.cpp
  render/OSPRayFrameBuffer.cpp
  render/OSPRayCamera.cpp
)

set(BRAYNSOSPRAYPLUGIN_HEADERS
  camera/StereoCamera.h
  geometry/ExtendedCones.h
  geometry/ExtendedCylinders.h
  geometry/ExtendedSpheres.h
  render/ExtendedOBJMaterial.h
  render/ExtendedOBJRenderer.h
  render/ProximityRenderer.h
  render/OSPRayScene.h
  render/OSPRayRenderer.h
  render/OSPRayFrameBuffer.h
  render/OSPRayCamera.h
)

set(BRAYNSOSPRAYPLUGIN_LINK_LIBRARIES
    PUBLIC vmmlib
    PRIVATE ${OSPRAY_LIBRARIES}
)

if(ZEROBUF_FOUND)
  list(APPEND BRAYNSOSPRAYPLUGIN_LINK_LIBRARIES ZeroBuf ZeroBufRender)
endif()

set(ISPC_INCLUDE_DIR ${OSPRAY_INCLUDE_DIRS})
include(ispc)
get_ispc_target_list()
ispc_compile(${BRAYNSOSPRAYPLUGIN_ISPC_SOURCES})
list(APPEND BRAYNSOSPRAYPLUGIN_SOURCES ${ISPC_OBJECTS})

common_library(braynsOSPRayPlugin)
