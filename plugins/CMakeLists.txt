# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

if(BRAYNS_DEFLECT_ENABLED)
  add_subdirectory(Deflect)
endif()

if(BRAYNS_NETWORKING_ENABLED)
  add_subdirectory(Rockets)
endif()

if(BRAYNS_OPENDECK_ENABLED)
  add_subdirectory(OpenDeck)
endif()

option(BRAYNS_VRPN_ENABLED "Activate vrpn plugin" OFF)
if(BRAYNS_VRPN_ENABLED)
  add_subdirectory(VRPN)
endif()

option(BRAYNS_CIRCUITVIEWER_ENABLED "Activate CircuitViewer plugin" ON)
if (BRAYNS_CIRCUITVIEWER_ENABLED)
  add_subdirectory(CircuitViewer)
endif()
