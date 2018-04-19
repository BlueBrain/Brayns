# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSTASKS_SOURCES
  LoadDataFunctor.cpp
  UploadBinaryTask.cpp
  UploadPathTask.cpp
)

set(BRAYNSTASKS_PUBLIC_HEADERS
  errors.h
  LoadDataFunctor.h
  UploadBinaryTask.h
  UploadPathTask.h
)

set(BRAYNSTASKS_INCLUDE_NAME brayns/tasks)
set(BRAYNSTASKS_LINK_LIBRARIES PUBLIC braynsCommon braynsParameters braynsIO
)

common_library(braynsTasks)
