# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSMANIPULATORS_SOURCES
  AbstractManipulator.cpp
  FlyingModeManipulator.cpp
  InspectCenterManipulator.cpp
)

set(BRAYNSMANIPULATORS_PUBLIC_HEADERS
  AbstractManipulator.h
  FlyingModeManipulator.h
  InspectCenterManipulator.h
)

set(BRAYNSCOMMON_INCLUDE_NAME brayns/manipulators)
set(BRAYNSMANIPULATORS_LINK_LIBRARIES
  PUBLIC braynsCommon
  PRIVATE braynsEngine
)

common_library(braynsManipulators)
