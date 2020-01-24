# Copyright (c) 2019, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSIBL_SOURCES main.cpp)
set(BRAYNSIBL_SHADERS
  brdf.fs
  brdf.vs
  cubemap.vs
  equirectangular_to_cubemap.fs
  irradiance_convolution.fs
  prefilter.fs)
set(BRAYNSIBL_LINK_LIBRARIES PRIVATE
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  ${FREEIMAGE_LIBRARIES}
  ${OPENGL_gl_LIBRARY}
  ${GLEW_LIBRARIES}
  glfw
  glm
)

common_application(braynsIBL NOHELP) 
target_include_directories(braynsIBL PRIVATE ${PROJECT_SOURCE_DIR} ${CMAKE_BINARY_DIR})
