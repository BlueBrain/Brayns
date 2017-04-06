# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSBENCHMARK_SOURCES main.cpp)

set(BRAYNSBENCHMARK_LINK_LIBRARIES
  PUBLIC brayns braynsCommon braynsIO braynsParameters
)

common_application(braynsBenchmark)
