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

  # Add respective ROOT directories
  LIST(APPEND EMBREE_INCLUDE_DIRECTORIES
		${EMBREE_ROOT}/include 
		${OSPRAY_ROOT}/include
		${OSPRAY_ROOT}/include/ospray
		${OSPRAY_ROOT}/include/ospray/SDK)

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

IF (EMBREE_MAX_ISA STREQUAL "NONE")
  SET(EMBREE_ISA_SUPPORTS_SSE4   ${EMBREE_ISA_SSE42})
  SET(EMBREE_ISA_SUPPORTS_AVX    ${EMBREE_ISA_AVX})
  SET(EMBREE_ISA_SUPPORTS_AVX2   ${EMBREE_ISA_AVX2})
  SET(EMBREE_ISA_SUPPORTS_AVX512 ${EMBREE_ISA_AVX512KNL})
ELSE()
  SET(EMBREE_ISA_SUPPORTS_SSE4   FALSE)
  SET(EMBREE_ISA_SUPPORTS_AVX    FALSE)
  SET(EMBREE_ISA_SUPPORTS_AVX2   FALSE)
  SET(EMBREE_ISA_SUPPORTS_AVX512 FALSE)
  IF (EMBREE_MAX_ISA MATCHES "SSE4\\.[12]$")
    SET(EMBREE_ISA_SUPPORTS_SSE4 TRUE)
  ELSEIF (EMBREE_MAX_ISA STREQUAL "AVX")
    SET(EMBREE_ISA_SUPPORTS_SSE4 TRUE)
    SET(EMBREE_ISA_SUPPORTS_AVX  TRUE)
  ELSEIF (EMBREE_MAX_ISA STREQUAL "AVX2")
    SET(EMBREE_ISA_SUPPORTS_SSE4 TRUE)
    SET(EMBREE_ISA_SUPPORTS_AVX  TRUE)
    SET(EMBREE_ISA_SUPPORTS_AVX2 TRUE)
  ELSEIF (EMBREE_MAX_ISA STREQUAL "AVX512KNL")
    SET(EMBREE_ISA_SUPPORTS_SSE4   TRUE)
    SET(EMBREE_ISA_SUPPORTS_AVX    TRUE)
    SET(EMBREE_ISA_SUPPORTS_AVX2   TRUE)
    SET(EMBREE_ISA_SUPPORTS_AVX512 TRUE)
  ENDIF()
ENDIF()

## Macro configure ISA targets for ispc ##
MACRO(CONFIGURE_ISPC_ISA)

  SET(OSPRAY_BUILD_ISA "ALL" CACHE STRING
      "Target ISA (SSE4, AVX, AVX2, AVX512, or ALL)")
  STRING(TOUPPER ${OSPRAY_BUILD_ISA} OSPRAY_BUILD_ISA)

  SET(OSPRAY_SUPPORTED_ISAS SSE4)
  IF(EMBREE_ISA_SUPPORTS_AVX)
    SET(OSPRAY_SUPPORTED_ISAS ${OSPRAY_SUPPORTED_ISAS} AVX)
  ENDIF()
  IF(EMBREE_ISA_SUPPORTS_AVX2)
    SET(OSPRAY_SUPPORTED_ISAS ${OSPRAY_SUPPORTED_ISAS} AVX2)
  ENDIF()
  IF(EMBREE_ISA_SUPPORTS_AVX512)
    SET(OSPRAY_SUPPORTED_ISAS ${OSPRAY_SUPPORTED_ISAS} AVX512)
  ENDIF() 
         
  SET_PROPERTY(CACHE OSPRAY_BUILD_ISA PROPERTY STRINGS
               ALL ${OSPRAY_SUPPORTED_ISAS})
        
  UNSET(OSPRAY_ISPC_TARGET_LIST)
             
  IF (OSPRAY_BUILD_ISA STREQUAL "ALL")

    SET(OSPRAY_ISPC_TARGET_LIST ${OSPRAY_ISPC_TARGET_LIST} sse4) 
    MESSAGE(STATUS "OSPRay SSE4 ISA target enabled.") 
    IF(EMBREE_ISA_SUPPORTS_AVX)
      SET(OSPRAY_ISPC_TARGET_LIST ${OSPRAY_ISPC_TARGET_LIST} avx)
      MESSAGE(STATUS "OSPRay AVX ISA target enabled.")
    ENDIF()
    IF(EMBREE_ISA_SUPPORTS_AVX2)
      SET(OSPRAY_ISPC_TARGET_LIST ${OSPRAY_ISPC_TARGET_LIST} avx2)
      MESSAGE(STATUS "OSPRay AVX2 ISA target enabled.")
    ENDIF()
    IF(EMBREE_ISA_SUPPORTS_AVX512)
      SET(OSPRAY_ISPC_TARGET_LIST ${OSPRAY_ISPC_TARGET_LIST} avx512knl-i32x16)
      MESSAGE(STATUS "OSPRay AVX512 ISA target enabled.")
    ENDIF()

  ELSEIF (OSPRAY_BUILD_ISA STREQUAL "AVX512")

    IF(NOT EMBREE_ISA_SUPPORTS_AVX512)
      MESSAGE(FATAL_ERROR "Your Embree build does not support AVX512KNL!")
    ENDIF()
    SET(OSPRAY_ISPC_TARGET_LIST avx512knl-i32x16)

  ELSEIF (OSPRAY_BUILD_ISA STREQUAL "AVX2")

    IF(NOT EMBREE_ISA_SUPPORTS_AVX2)
      MESSAGE(FATAL_ERROR "Your Embree build does not support AVX2!")
    ENDIF()
    SET(OSPRAY_ISPC_TARGET_LIST avx2)

  ELSEIF (OSPRAY_BUILD_ISA STREQUAL "AVX")

    IF(NOT EMBREE_ISA_SUPPORTS_AVX)
      MESSAGE(FATAL_ERROR "Your Embree build does not support AVX!")
    ENDIF()
    SET(OSPRAY_ISPC_TARGET_LIST avx)

  ELSEIF (OSPRAY_BUILD_ISA STREQUAL "SSE4")
    SET(OSPRAY_ISPC_TARGET_LIST sse4)
  ELSE ()
    MESSAGE(ERROR "Invalid OSPRAY_BUILD_ISA value. "
                  "Please select one of SSE4, AVX, AVX2, AVX512, or ALL.")
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
  ispc/camera/ClippedPerspectiveCamera.ispc
  ispc/render/utils/AbstractRenderer.ispc
  ispc/render/utils/RandomGenerator.ispc
  ispc/render/utils/SkyBox.ispc
  ispc/geometry/ExtendedCylinders.ispc
  ispc/geometry/ExtendedCones.ispc
  ispc/geometry/ExtendedSpheres.ispc
  ispc/render/ExtendedOBJMaterial.ispc
  ispc/render/ExtendedOBJRenderer.ispc
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
  ispc/render/ExtendedOBJRenderer.cpp
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
  ispc/render/ExtendedOBJRenderer.h
  ispc/render/ProximityRenderer.h
  ispc/render/SimulationRenderer.h
  ispc/render/ParticleRenderer.h
  OSPRayEngine.h
  OSPRayScene.h
  OSPRayRenderer.h
  OSPRayFrameBuffer.h
  OSPRayCamera.h
)

set(BRAYNSOSPRAYENGINEPLUGIN_LINK_LIBRARIES
    PUBLIC vmmlib braynsCommon braynsIO ${OSPRAY_LIBRARIES}
)

include(ispc)

CONFIGURE_ISPC_ISA()
option(BRAYNS_HARDWARE_RANDOMIZER_ENABLED "Enables hardware randomizer if supported by target CPU" OFF)
if(BRAYNS_HARDWARE_RANDOMIZER_ENABLED)
    set(ISPC_EXTRA_ARGS -DBRAYNS_ISPC_USE_HARDWARE_RANDOMIZER=ON)
endif()

INCLUDE_DIRECTORIES_ISPC(${OSPRAY_INCLUDE_DIRS})
set(OSPRAY_EMBREE_SOURCE_DIR ${OSPRAY_ROOT}/include/ospray/embree)
OSPRAY_ISPC_COMPILE(${BRAYNSOSPRAYPLUGIN_ISPC_SOURCES})
list(APPEND BRAYNSOSPRAYENGINEPLUGIN_SOURCES ${ISPC_OBJECTS})

common_library(braynsOSPRayEnginePlugin)
