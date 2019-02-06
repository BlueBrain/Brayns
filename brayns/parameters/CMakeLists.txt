# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSPARAMETERS_SOURCES
  ParametersManager.cpp
  AbstractParameters.cpp
  AnimationParameters.cpp
  RenderingParameters.cpp
  GeometryParameters.cpp
  ApplicationParameters.cpp
  VolumeParameters.cpp
)

set(BRAYNSPARAMETERS_PUBLIC_HEADERS
  ParametersManager.h
  AbstractParameters.h
  AnimationParameters.h
  RenderingParameters.h
  GeometryParameters.h
  ApplicationParameters.h
  VolumeParameters.h
)

set(BRAYNSPARAMETERS_LINK_LIBRARIES
    PRIVATE glm ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

set(BRAYNSPARAMETERS_INCLUDE_NAME brayns/parameters)
common_library(braynsParameters)
