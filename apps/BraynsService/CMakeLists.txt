# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
# Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

add_executable(braynsService main.cpp)

target_link_libraries(braynsService PRIVATE brayns)

install(TARGETS braynsService
    EXPORT braynsService-targets
    RUNTIME DESTINATION bin
)

install(EXPORT braynsService-targets
    DESTINATION share/CMake
)
