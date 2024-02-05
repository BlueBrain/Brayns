# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

include(FetchContent)

set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
set(CMAKE_INSTALL_LIBDIR lib)

if((BRAYNS_VIEWER_ENABLED OR BRAYNS_IBL_ENABLED) AND NOT glfw3_FOUND)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glfw")
        message(STATUS "Downloading GLFW3 from submodules")
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG        3.3.5
            GIT_SHALLOW ON
            GIT_SUBMODULES_RECURSE ON
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw
        )
        FetchContent_Populate(glfw)
    endif()

    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(glfw)
endif()

if(BRAYNS_VIEWER_ENABLED)
    # IMGUI
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
        message(STATUS "Downloading ImGui from submodules")
        FetchContent_Declare(
            imgui
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_TAG        d38d7c6628bebd02692cfdd6fa76b4d992a35b75
            GIT_SHALLOW ON
            GIT_SUBMODULES_RECURSE ON
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui
        )
        FetchContent_Populate(imgui)
    endif()

    add_library(imgui STATIC
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_glfw.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_opengl2.cpp
    )
    target_compile_definitions(imgui PRIVATE -DIMGUI_IMPL_OPENGL_LOADER_GLEW)
    target_link_libraries(imgui glfw ${OPENGL_gl_LIBRARY} ${OPENGL_opengl_LIBRARY})
    target_include_directories(imgui PUBLIC
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui>"
      "$<INSTALL_INTERFACE:include>")

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/imgui
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      FILES_MATCHING PATTERN "*.h")

    install(TARGETS imgui
      EXPORT imgui-targets
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib
    )

    install(EXPORT imgui-targets
      DESTINATION share/CMake
    )

endif()
