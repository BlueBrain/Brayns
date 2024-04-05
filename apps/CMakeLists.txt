# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
# Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
# Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

add_executable(braynsService braynsService.cpp)

target_link_libraries(braynsService PRIVATE brayns::core)

if(BRAYNS_ENABLE_CIRCUITS)
    target_link_libraries(braynsService PRIVATE brayns::circuits)
    target_compile_definitions(braynsService PRIVATE "BRAYNS_CIRCUIT_EXPLORER_ENABLED")
endif()

if(BRAYNS_ENABLE_ATLAS)
    target_link_libraries(braynsService PRIVATE brayns::atlas)
    target_compile_definitions(braynsService PRIVATE "BRAYNS_ATLAS_EXPLORER_ENABLED")
endif()

install(
    TARGETS braynsService
    EXPORT BraynsTargets
    RUNTIME DESTINATION bin
)
