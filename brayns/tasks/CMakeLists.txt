# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSTASKS_SOURCES
  AddModelFromBlobTask.cpp
  AddModelTask.cpp
  LoadModelFunctor.cpp
)

set(BRAYNSTASKS_PUBLIC_HEADERS
  AddModelFromBlobTask.h
  AddModelTask.h
  LoadModelFunctor.h
  errors.h
)

set(BRAYNSTASKS_INCLUDE_NAME brayns/tasks)
set(BRAYNSTASKS_LINK_LIBRARIES PUBLIC braynsCommon braynsEngine)

common_library(braynsTasks)
