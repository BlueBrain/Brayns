# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
# Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
#                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

add_executable(braynsBenchmark main.cpp)

target_link_libraries(braynsBenchmark PUBLIC brayns braynsIO)

install(TARGETS braynsBenchmark
    EXPORT braynsBenchmark-targets
    RUNTIME DESTINATION bin
)

install(EXPORT braynsBenchmark-targets
    DESTINATION share/CMake
)
