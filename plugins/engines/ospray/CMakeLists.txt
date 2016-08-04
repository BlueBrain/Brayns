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
MACRO(CONFIGURE_ISPC)
  # Embree common include directories; others may be added depending on build target.
  # this section could be sooo much cleaner if embree only used
  # fully-qualified include names...
  SET(EMBREE_INCLUDE_DIRECTORIES
    ${OSPRAY_EMBREE_SOURCE_DIR}/
    ${OSPRAY_EMBREE_SOURCE_DIR}/include
    ${OSPRAY_EMBREE_SOURCE_DIR}/common
    ${OSPRAY_EMBREE_SOURCE_DIR}/
    ${OSPRAY_EMBREE_SOURCE_DIR}/kernels
    )

  IF (OSPRAY_TARGET STREQUAL "mic")
    SET(OSPRAY_EXE_SUFFIX ".mic")
    SET(OSPRAY_LIB_SUFFIX "_mic")
    SET(OSPRAY_ISPC_SUFFIX ".cpp")
    SET(OSPRAY_ISPC_TARGET "mic")
    SET(THIS_IS_MIC ON)
    SET(__XEON__ OFF)
    INCLUDE(${PROJECT_SOURCE_DIR}/CMake/icc_xeonphi.cmake)

    # additional Embree include directory
    LIST(APPEND EMBREE_INCLUDE_DIRECTORIES ${OSPRAY_EMBREE_SOURCE_DIR}/kernels/xeonphi)

    SET(OSPRAY_TARGET_MIC ON PARENT_SCOPE)
  ELSE()
    SET(OSPRAY_EXE_SUFFIX "")
    SET(OSPRAY_LIB_SUFFIX "")
    SET(OSPRAY_ISPC_SUFFIX ".o")
    SET(THIS_IS_MIC OFF)
    SET(__XEON__ ON)
    IF (${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
      INCLUDE(${PROJECT_SOURCE_DIR}/CMake/icc.cmake)
    ELSEIF (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
      INCLUDE(${PROJECT_SOURCE_DIR}/CMake/gcc.cmake)
    ELSEIF (${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
      INCLUDE(${PROJECT_SOURCE_DIR}/CMake/clang.cmake)
    ELSEIF (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
      INCLUDE(${PROJECT_SOURCE_DIR}/CMake/clang.cmake)
    ELSEIF (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
      INCLUDE(${PROJECT_SOURCE_DIR}/CMake/msvc.cmake)
    ELSE()
      MESSAGE(FATAL_ERROR "Unsupported compiler specified: '${CMAKE_CXX_COMPILER_ID}'")
    ENDIF()

    # additional Embree include directory
    LIST(APPEND EMBREE_INCLUDE_DIRECTORIES ${OSPRAY_EMBREE_SOURCE_DIR}/kernels/xeon)

    SET(OSPRAY_EMBREE_ENABLE_AVX512 false)
    IF (BRAYNS_BUILD_ISA STREQUAL "ALL")
      # ------------------------------------------------------------------
      # in 'all' mode, we have a list of multiple targets for ispc,
      # and enable all targets for embree (we may still disable some
      # below if the compiler doesn't support them
      # ------------------------------------------------------------------
      SET(OSPRAY_ISPC_TARGET_LIST sse4 avx avx2)
      SET(OSPRAY_EMBREE_ENABLE_SSE  true)
      SET(OSPRAY_EMBREE_ENABLE_AVX  true)
      SET(OSPRAY_EMBREE_ENABLE_AVX2 true)
      IF (OSPRAY_BUILD_ENABLE_KNL)
        SET(OSPRAY_EMBREE_ENABLE_AVX512 true)
        SET(OSPRAY_ISPC_TARGET_LIST sse4 avx avx2 avx512knl-i32x16)
      ENDIF()

    ELSEIF (BRAYNS_BUILD_ISA STREQUAL "AVX512")
      # ------------------------------------------------------------------
      # in 'avx512' mode, we currently build only avx512, in generic
      # mode, but enable all embree targets to fall back to (currently
      # does not work since embree would require a 16-wide trace
      # function which it has in neither of the three targets)
      # ------------------------------------------------------------------
      SET(OSPRAY_ISPC_TARGET_LIST avx512knl-i32x16)
      SET(OSPRAY_EMBREE_ENABLE_SSE  true)
      SET(OSPRAY_EMBREE_ENABLE_AVX  true)
      SET(OSPRAY_EMBREE_ENABLE_AVX2 true)
      SET(OSPRAY_EMBREE_ENABLE_AVX512 true)

    ELSEIF (BRAYNS_BUILD_ISA STREQUAL "AVX2")
      # ------------------------------------------------------------------
      # in 'avx2' mode, we enable ONLY avx2 for ispc, and all targets
      # up to avx2 for embree. note that if the compiler doesn't
      # support AVX we will have a combination where embree uses AVX
      # (the most the compiler can do), while ispc still uses
      # avx. this works because both targets are 8 wide. it does
      # however require the compiler to understand AT LEAST AVX1.
      # ------------------------------------------------------------------
      SET(OSPRAY_ISPC_TARGET_LIST avx2)
      SET(OSPRAY_EMBREE_ENABLE_SSE  true)
      SET(OSPRAY_EMBREE_ENABLE_AVX  true)
      SET(OSPRAY_EMBREE_ENABLE_AVX2 true)

    ELSEIF (BRAYNS_BUILD_ISA STREQUAL "AVX")
      # ------------------------------------------------------------------
      # in 'avx' mode, we enable ONLY avx for ispc, and both sse and
      # avx for embree. note that this works ONLY works if the
      # compiler knows at least AVX
      # ------------------------------------------------------------------
      SET(OSPRAY_ISPC_TARGET_LIST avx)
      SET(OSPRAY_EMBREE_ENABLE_SSE  true)
      SET(OSPRAY_EMBREE_ENABLE_AVX  true)
      SET(OSPRAY_EMBREE_ENABLE_AVX2 false)

    ELSEIF (BRAYNS_BUILD_ISA STREQUAL "SSE")
      # ------------------------------------------------------------------
      # in 'sse' mode, we enable ONLY sse4 for ispc, and only sse for
      # embree
      # ------------------------------------------------------------------
      SET(OSPRAY_ISPC_TARGET_LIST sse4)
      SET(OSPRAY_EMBREE_ENABLE_SSE  true)
      SET(OSPRAY_EMBREE_ENABLE_AVX  false)
      SET(OSPRAY_EMBREE_ENABLE_AVX2 false)
    ELSE ()
      MESSAGE(ERROR "Invalid BRAYNS_BUILD_ISA value. Please select one of SSE, AVX, AVX2, or ALL.")
    ENDIF()

  ENDIF()

  IF (OSPRAY_EMBREE_ENABLE_AVX AND NOT OSPRAY_COMPILER_SUPPORTS_AVX)
    IF (NOT OSPRAY_WARNED_MISSING_AVX)
      MESSAGE("Warning: Need at least version ${GCC_VERSION_REQUIRED_AVX} of gcc for AVX. Disabling AVX.\nTo compile for AVX, please switch to either 'ICC'-compiler, or upgrade your gcc version.")
      SET(OSPRAY_WARNED_MISSING_AVX ON CACHE INTERNAL "Warned about missing AVX support.")
    ENDIF()
    SET(OSPRAY_EMBREE_ENABLE_AVX false)
  ENDIF()

  IF (OSPRAY_EMBREE_ENABLE_AVX2 AND NOT OSPRAY_COMPILER_SUPPORTS_AVX2)
    IF (NOT OSPRAY_WARNED_MISSING_AVX2)
      MESSAGE("Warning: Need at least version ${GCC_VERSION_REQUIRED_AVX2} of gcc for AVX2. Disabling AVX2.\nTo compile for AVX2, please switch to either 'ICC'-compiler, or upgrade your gcc version.")
      SET(OSPRAY_WARNED_MISSING_AVX2 ON CACHE INTERNAL "Warned about missing AVX2 support.")
    ENDIF()
    SET(OSPRAY_EMBREE_ENABLE_AVX2 false)
  ENDIF()

  IF (OSPRAY_EMBREE_ENABLE_AVX512 AND NOT OSPRAY_COMPILER_SUPPORTS_AVX512)
    IF (NOT OSPRAY_WARNED_MISSING_AVX2)
      MESSAGE("Warning: Need at least version ${GCC_VERSION_REQUIRED_AVX512} of gcc for AVX512. Disabling AVX512.\nTo compile for AVX512, please switch to either 'ICC'-compiler, or upgrade your gcc version.")
      SET(OSPRAY_WARNED_MISSING_AVX512 ON CACHE INTERNAL "Warned about missing AVX512 support.")
    ENDIF()
    SET(OSPRAY_EMBREE_ENABLE_AVX512 false)
  ENDIF()

  IF (THIS_IS_MIC)
    # whether to build in MIC/xeon phi support
    SET(OSPRAY_BUILD_COI_DEVICE OFF CACHE BOOL "Build COI Device for OSPRay's MIC support?")
  ENDIF()

  INCLUDE(${PROJECT_SOURCE_DIR}/CMake/ispc.cmake)

  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
  INCLUDE_DIRECTORIES(${EMBREE_INCLUDE_DIRECTORIES})

  INCLUDE_DIRECTORIES_ISPC(${PROJECT_SOURCE_DIR})
  INCLUDE_DIRECTORIES_ISPC(${EMBREE_INCLUDE_DIRECTORIES})

  # for auto-generated cmakeconfig etc
  INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})
  INCLUDE_DIRECTORIES_ISPC(${PROJECT_BINARY_DIR})

ENDMACRO()

set(BRAYNSOSPRAYPLUGIN_ISPC_SOURCES
  render/utils/AbstractRenderer.ispc
  render/utils/RandomGenerator.ispc
  render/utils/SkyBox.ispc
  geometry/ExtendedCylinders.ispc
  geometry/ExtendedCones.ispc
  geometry/ExtendedSpheres.ispc
  camera/StereoCamera.ispc
  render/ExtendedOBJMaterial.ispc
  render/ExtendedOBJRenderer.ispc
  render/ProximityRenderer.ispc
  render/SimulationRenderer.ispc
)

set(BRAYNSOSPRAYPLUGIN_SOURCES
  render/utils/AbstractRenderer.cpp
  OSPRayEngine.cpp
  camera/StereoCamera.cpp
  geometry/ExtendedCones.cpp
  geometry/ExtendedCylinders.cpp
  geometry/ExtendedSpheres.cpp
  render/ExtendedOBJMaterial.cpp
  render/ExtendedOBJRenderer.cpp
  render/ProximityRenderer.cpp
  render/SimulationRenderer.cpp
  render/OSPRayScene.cpp
  render/OSPRayRenderer.cpp
  render/OSPRayFrameBuffer.cpp
  render/OSPRayCamera.cpp
)

set(BRAYNSOSPRAYPLUGIN_PUBLIC_HEADERS
  render/utils/AbstractRenderer.h
  OSPRayEngine.h
  camera/StereoCamera.h
  geometry/ExtendedCones.h
  geometry/ExtendedCylinders.h
  geometry/ExtendedSpheres.h
  render/ExtendedOBJMaterial.h
  render/ExtendedOBJRenderer.h
  render/ProximityRenderer.h
  render/SimulationRenderer.h
  render/OSPRayScene.h
  render/OSPRayRenderer.h
  render/OSPRayFrameBuffer.h
  render/OSPRayCamera.h
)

set(BRAYNSOSPRAYPLUGIN_LINK_LIBRARIES
    PUBLIC vmmlib braynsCommon braynsIO
    PRIVATE ${OSPRAY_LIBRARIES}
)

if(BRAYNS_REST_ENABLED)
  list(APPEND BRAYNSOSPRAYPLUGIN_LINK_LIBRARIES Servus)
endif()

include(ispc)
CONFIGURE_ISPC()
INCLUDE_DIRECTORIES_ISPC(${OSPRAY_INCLUDE_DIRS})
set(OSPRAY_EMBREE_SOURCE_DIR ${OSPRAY_ROOT}/include/ospray/embree)
OSPRAY_ISPC_COMPILE(${BRAYNSOSPRAYPLUGIN_ISPC_SOURCES})
list(APPEND BRAYNSOSPRAYPLUGIN_SOURCES ${ISPC_OBJECTS})

common_library(braynsOSPRayPlugin)
