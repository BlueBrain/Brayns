# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#                     Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

# OpenDeck module
option(BRAYNS_OPENDECK_ENABLED "Activate opendeck plugin and module" OFF)
if(BRAYNS_OPENDECK_ENABLED)
  add_subdirectory(opendeck)
  if(TARGET ospray_module_opendeck)
    add_dependencies(braynsOSPRayEngine ospray_module_opendeck)
  endif()
endif()
