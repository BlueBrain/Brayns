# Copyright (c) 2015-2021, EPFL/Blue Brain Project
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

add_library(braynsUI STATIC Application.cpp Application.h)

target_link_libraries(braynsUI PUBLIC
    brayns
    glm)

target_link_libraries(braynsUI PRIVATE
    braynsManipulators
    ${OPENGL_gl_LIBRARY}
    glfw
    GLEW #${GLEW_LIBRARIES}
    imgui)

#target_include_directories(braynsUI PRIVATE ${PROJECT_SOURCE_DIR}/deps/glfw/include)
#target_include_directories(braynsUI PRIVATE ${PROJECT_SOURCE_DIR}/deps)

install(TARGETS braynsUI
    EXPORT braynsUI-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsUI-targets
    DESTINATION share/CMake
)
