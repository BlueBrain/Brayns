# Gather all the conditional
# common_package dependencies of Brayns


if(BRAYNS_NETWORKING_ENABLED OR BRAYNS_DEFLECT_ENABLED)
  common_find_package(Lexis REQUIRED)
endif()

# ZeroEQ and HTTP messaging
if(BRAYNS_NETWORKING_ENABLED)
  common_find_package(LibJpegTurbo REQUIRED)
  common_find_package(ZeroBuf REQUIRED)
  common_find_package(ZeroEQ REQUIRED)
  list(APPEND COMMON_FIND_PACKAGE_DEFINES BRAYNS_USE_NETWORKING)
endif()

# Streaming to display walls
if(BRAYNS_DEFLECT_ENABLED)
  common_find_package(Deflect REQUIRED)
endif()

# Data access
if(BRAYNS_BRION_ENABLED)
  common_find_package(Servus REQUIRED)
  common_find_package(Lunchbox)
  common_find_package(BBPTestData)
  common_find_package(Brion REQUIRED)
  common_find_package(HDF5 SYSTEM REQUIRED COMPONENTS C CXX)
endif()

# OSPRay rendering engine
if(BRAYNS_OSPRAY_ENABLED)
    common_find_package(ospray SYSTEM)
endif()

# OptiX rendering engine
if(BRAYNS_OPTIX_ENABLED)
    common_find_package(OptiX SYSTEM)
    common_find_package(CUDA SYSTEM)
endif()

# Livre rendering engine
if(BRAYNS_LIVRE_ENABLED)
  common_find_package(Livre)
endif()

# Image processing
if(BRAYNS_IMAGEMAGICK_ENABLED)
  common_find_package(Magick++ SYSTEM)
endif()

# Mesh loading
if(BRAYNS_ASSIMP_ENABLED)
  common_find_package(assimp)
endif()
