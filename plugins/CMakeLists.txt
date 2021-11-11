# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
#                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

if(BRAYNS_MULTIVIEW_ENABLED)
    message(FATAL_ERROR "Multiview plugin needs to be updated before compilation")
    add_subdirectory(Multiview)
endif()

if(BRAYNS_OPENDECK_ENABLED)
    add_subdirectory(OpenDeck)
endif()

if(BRAYNS_VRPN_ENABLED)
    add_subdirectory(VRPN)
endif()

if (BRAYNS_CIRCUITEXPLORER_ENABLED)
    add_subdirectory(CircuitExplorer)
endif()

if (BRAYNS_DTI_ENABLED)
    add_subdirectory(DTI)
endif()

if(BRAYNS_CIRCUITINFO_ENABLED)
    add_subdirectory(CircuitInfo)
endif()
