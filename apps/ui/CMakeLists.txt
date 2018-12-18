# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSUI_SOURCES
  BaseWindow.cpp
)

set(BRAYNSUI_PUBLIC_HEADERS
  BaseWindow.h
)

set(BRAYNSUI_LINK_LIBRARIES
    PUBLIC brayns braynsParameters vmmlib
      ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    PRIVATE braynsManipulators ${GLUT_LIBRARIES} ${GLEW_LIBRARIES}
)

set(BRAYNSUI_OMIT_EXPORT ON)
common_library(braynsUI)

if(APPLE)
  target_compile_options(braynsUI PRIVATE -Wno-deprecated-declarations)
  target_link_libraries(braynsUI PRIVATE "-framework OpenGL")
endif()
