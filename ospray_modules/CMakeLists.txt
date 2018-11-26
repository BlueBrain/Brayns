# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#                     Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

# Optix module
option(BRAYNS_OPTIX_ENABLED "Activate OptiX module" OFF)
if(BRAYNS_OPTIX_ENABLED)
  add_subdirectory(optix)
  if(TARGET ospray_module_optix)
    add_dependencies(braynsOSPRayEngine ospray_module_optix)
  endif()
endif()

# OpenDeck module
option(BRAYNS_OPENDECK_ENABLED "Activate opendeck plugin and module" OFF)
if(BRAYNS_OPENDECK_ENABLED)
  add_subdirectory(opendeck)
  if(TARGET ospray_module_opendeck)
    add_dependencies(braynsOSPRayEngine ospray_module_opendeck)
  endif()
endif()

# Stereoscopy module
option(BRAYNS_STEREOSCOPY_ENABLED "Activate Stereoscopy module" OFF)
if(BRAYNS_STEREOSCOPY_ENABLED)
  add_subdirectory(stereoscopy)
  if(TARGET ospray_module_stereoscopy)
    add_dependencies(braynsOSPRayEngine ospray_module_stereoscopy)
  endif()
endif()
