# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
# Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
#                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

# Brion is needed for 3 plugin
if(BRAYNS_CIRCUITEXPLORER_ENABLED OR BRAYNS_CIRCUITINFO_ENABLED OR BRAYNS_DTI_ENABLED)
    find_package(HDF5 REQUIRED)
    find_package(Brion QUIET)
endif()

# CircutiExplorer
if(BRAYNS_CIRCUITEXPLORER_ENABLED)
    find_package(Boost QUIET REQUIRED COMPONENTS system)
    find_package(HDF5 REQUIRED)
    find_package(HighFive QUIET)
    find_package(sonata QUIET)
    find_package(MVDTool QUIET)
    find_package(MorphIO QUIET)
endif()

add_subdirectory(deps)

if(BRAYNS_MULTIVIEW_ENABLED)
    message(FATAL_ERROR "Multiview plugin needs to be updated before compilation")
    add_subdirectory(Multiview)
endif()

if(BRAYNS_OPENDECK_ENABLED)
    add_subdirectory(OpenDeck)
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
