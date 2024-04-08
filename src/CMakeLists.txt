# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

include(BraynsLibrary)

add_subdirectory(brayns/core)

if(BRAYNS_ENABLE_ATLAS)
    add_subdirectory(brayns/atlas)
endif()

if(BRAYNS_ENABLE_CIRCUITS)
    add_subdirectory(brayns/circuits)
endif()
