# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSSERVICE_SOURCES main.cpp)

set(BRAYNSSERVICE_LINK_LIBRARIES
  PUBLIC brayns braynsCommon braynsIO braynsParameters ${libuv_LIBRARIES}
)

common_application(braynsService)
