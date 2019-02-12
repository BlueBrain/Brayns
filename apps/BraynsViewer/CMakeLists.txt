# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSVIEWER_SOURCES main.cpp)

set(BRAYNSVIEWER_LINK_LIBRARIES
  PUBLIC brayns
  braynsCommon
  braynsIO
  braynsParameters
  braynsUI
  ${OPENGL_gl_LIBRARY}
  glfw
  ${GLEW_LIBRARIES}
  imgui
)

common_application(braynsViewer)

target_include_directories(braynsViewer PRIVATE ${PROJECT_SOURCE_DIR}/deps/glfw/include)
target_include_directories(braynsViewer PRIVATE ${PROJECT_SOURCE_DIR}/deps)
