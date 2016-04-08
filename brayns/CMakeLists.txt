# Copyright (c) 2011-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of BRayns

add_subdirectory(common)

set(BRAYNS_PUBLIC_HEADERS Brayns.h)

set(BRAYNS_SOURCES Brayns.cpp)

set(BRAYNS_LINK_LIBRARIES PUBLIC braynsCommon braynsPlugins)

common_library(brayns)
