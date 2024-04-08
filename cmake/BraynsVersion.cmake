# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNS_VERSION_MAJOR 4)
set(BRAYNS_VERSION_MINOR 0)
set(BRAYNS_VERSION_PATCH 0)
set(BRAYNS_VERSION_PRE_RELEASE 1)

set(BRAYNS_VERSION ${BRAYNS_VERSION_MAJOR}.${BRAYNS_VERSION_MINOR}.${BRAYNS_VERSION_PATCH})

function(define_brayns_version_marco TARGET)
    target_compile_definitions(
        ${TARGET}
        PUBLIC
        BRAYNS_VERSION_MAJOR=${BRAYNS_VERSION_MAJOR}
        BRAYNS_VERSION_MINOR=${BRAYNS_VERSION_MINOR}
        BRAYNS_VERSION_PATCH=${BRAYNS_VERSION_PATCH}
        BRAYNS_VERSION_PRE_RELEASE=${BRAYNS_VERSION_PRE_RELEASE}
    )
endfunction()
