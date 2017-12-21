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
  SceneParameters.cpp
  StreamParameters.cpp
  VolumeParameters.cpp
)

set(BRAYNSPARAMETERS_PUBLIC_HEADERS
  ParametersManager.h
  AbstractParameters.h
  AnimationParameters.h
  RenderingParameters.h
  GeometryParameters.h
  ApplicationParameters.h
  SceneParameters.h
  StreamParameters.h
  VolumeParameters.h
)

set(BRAYNSPARAMETERS_LINK_LIBRARIES
    PUBLIC vmmlib ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

common_library(braynsParameters)
