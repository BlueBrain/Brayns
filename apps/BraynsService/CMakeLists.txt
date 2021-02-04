# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSSERVICE_SOURCES main.cpp)

set(BRAYNSSERVICE_LINK_LIBRARIES PUBLIC brayns ${libuv_LIBRARIES})

if(BRAYNS_NETWORKING_ENABLED)
  list(APPEND BRAYNSSERVICE_LINK_LIBRARIES PRIVATE braynsRockets)
endif()

common_application(braynsService)
