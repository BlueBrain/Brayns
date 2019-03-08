# Copyright (c) 2019, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSRUNNER_SOURCES main.cpp)
set(BRAYNSRUNNER_LINK_LIBRARIES PUBLIC brayns)
common_application(braynsRunner)
