# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Authors:
# Cyrille Favreau <cyrille.favreau@epfl.ch>
# Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

if(BRAYNS_CIRCUITEXPLORER_ENABLED)
    find_package(HDF5 REQUIRED)
    find_package(HighFive QUIET)
    find_package(sonata QUIET)
    find_package(MorphIO QUIET)
endif()

if(BRAYNS_ATLASEXPLORER_ENABLED)
    find_package(BZip2 REQUIRED)
endif()

add_subdirectory(deps)

if(BRAYNS_CIRCUITEXPLORER_ENABLED)
    add_subdirectory(CircuitExplorer)
endif()

if(BRAYNS_ATLASEXPLORER_ENABLED)
    add_subdirectory(AtlasExplorer)
endif()
