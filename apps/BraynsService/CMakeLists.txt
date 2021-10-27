# Copyright (c) 2015-2021, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSSERVICE_SOURCES main.cpp)
set(BRAYNSSERVICE_LINK_LIBRARIES PUBLIC brayns)
common_application(braynsService)
