# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Authors:
# Cyrille Favreau <cyrille.favreau@epfl.ch>
# Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

# Brion is needed for 3 plugin
if(BRAYNS_CIRCUITEXPLORER_ENABLED OR BRAYNS_DTI_ENABLED)
    find_package(HDF5 REQUIRED)
    find_package(Brion QUIET)
endif()

# CircutiExplorer
if(BRAYNS_CIRCUITEXPLORER_ENABLED)
    find_package(Boost QUIET REQUIRED COMPONENTS system)
    find_package(HighFive QUIET)
    find_package(sonata QUIET)
    find_package(MVDTool QUIET)
    find_package(MorphIO QUIET)
endif()

if(BRAYNS_ATLASEXPLORER_ENABLED)
    find_package(BZip2 REQUIRED)
endif()

add_subdirectory(deps)

if(BRAYNS_CIRCUITEXPLORER_ENABLED)
    add_subdirectory(CircuitExplorer)
endif()

if(BRAYNS_DTI_ENABLED)
    add_subdirectory(DTI)
endif()

if(BRAYNS_ATLASEXPLORER_ENABLED)
    add_subdirectory(AtlasExplorer)
endif()

if(BRAYNS_CYLINDRICCAMERA_ENABLED)
    add_subdirectory(CylindricCamera)
endif()

if(BRAYNS_MOLECULEEXPLORER_ENABLED)
    add_subdirectory(MoleculeExplorer)
endif()
