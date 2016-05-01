# Copyright (c) 2011-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of BRayns

set(BRAYNSPARAMETERS_SOURCES
  ParametersManager.cpp
  AbstractParameters.cpp
  RenderingParameters.cpp
  GeometryParameters.cpp
  ApplicationParameters.cpp
  SceneParameters.cpp
)

set(BRAYNSPARAMETERS_PUBLIC_HEADERS
  ParametersManager.h
  AbstractParameters.h
  RenderingParameters.h
  GeometryParameters.h
  ApplicationParameters.h
  SceneParameters.h
)

set(BRAYNSPARAMETERS_LINK_LIBRARIES
    PUBLIC vmmlib boost_program_options
)

common_library(braynsParameters)
