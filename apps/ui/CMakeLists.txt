# Copyright (c) 2015-2019, EPFL/Blue Brain Project
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSUI_SOURCES
  Application.cpp
)

set(BRAYNSUI_PUBLIC_HEADERS
  Application.h
)

set(BRAYNSUI_LINK_LIBRARIES
    PUBLIC brayns braynsParameters glm
    PRIVATE braynsManipulators
    ${OPENGL_gl_LIBRARY}
    glfw
    ${GLEW_LIBRARIES}
    imgui
)

set(BRAYNSUI_OMIT_EXPORT ON)
common_library(braynsUI)

if(APPLE)
  target_compile_options(braynsUI PRIVATE -Wno-deprecated-declarations)
  target_link_libraries(braynsUI PRIVATE "-framework OpenGL")
endif()

target_include_directories(braynsUI PRIVATE ${PROJECT_SOURCE_DIR}/deps/glfw/include)
target_include_directories(braynsUI PRIVATE ${PROJECT_SOURCE_DIR}/deps)
