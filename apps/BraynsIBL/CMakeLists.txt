# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

# Create temporary include files from the shaders
set(BRAYNSIBL_SHADERS
  brdf.fs
  brdf.vs
  cubemap.vs
  equirectangular_to_cubemap.fs
  irradiance_convolution.fs
  prefilter.fs)

foreach(SHADER IN LISTS BRAYNSIBL_SHADERS)
    file(READ "${SHADER}" SHADER_CONTENT)
    string(REPLACE "\n" "\"\n\"" SHADER_CONTENT "${SHADER_CONTENT}")
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    string(REPLACE "." "_" SHADER_NAME "${SHADER_NAME}")
    configure_file(ShaderIncludeTemplate.h.in ${CMAKE_BINARY_DIR}/include/apps/BraynsIBL/${SHADER}.h)
endforeach()

add_executable(braynsIBL main.cpp)

target_include_directories(braynsIBL PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/include
)

target_link_libraries(braynsIBL PRIVATE
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${FREEIMAGE_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_opengl_LIBRARY}
    GLEW #${GLEW_LIBRARIES}
    glfw
    glm
)

install(TARGETS braynsIBL
    EXPORT braynsIBL-targets
    RUNTIME DESTINATION bin
)

install(EXPORT braynsIBL-targets
    DESTINATION share/CMake
)
