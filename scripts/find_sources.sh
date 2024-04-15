# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

find \
    apps src tests \
    \( -name "*.h" -or -name "*.cpp" \) \
    -not -path "src/brayns/core/deps/*.h" \
    -not -path "tests/deps/*.h"
