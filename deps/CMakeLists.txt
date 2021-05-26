# Copyright (c) 2015-2019, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Async++ as a shared library" FORCE)
add_subdirectory(async++)
if(APPLE)
  # std::exception derivatives are not catched otherwise, e.g. in unit tests for
  # testing loading errors; see https://stackoverflow.com/questions/21737201
  target_compile_options(Async++ PRIVATE -frtti)
endif()
list(APPEND LCOV_EXCLUDE '${PROJECT_SOURCE_DIR}/deps/async++/include/async++/*'
                         '${PROJECT_SOURCE_DIR}/deps/async++/src/*')

if(FreeImage_FOUND)
  add_subdirectory(perceptualdiff)
  list(APPEND LCOV_EXCLUDE '${PROJECT_SOURCE_DIR}/deps/perceptualdiff/*')
endif()

if(libuv_FOUND)
  if(NOT EXISTS "${PROJECT_SOURCE_DIR}/deps/uvw")
    message(FATAL_ERROR "uvw missing, run: git submodule update --init")
  endif()
  list(APPEND LCOV_EXCLUDE '${PROJECT_SOURCE_DIR}/deps/uvw/*')
endif()

# Both Poco and GLM create "uninstall" targets. But GLM check wether the
# target exists before or not, whereas POCO doesnt give a damn.
set(ENABLE_REDIS OFF CACHE BOOL "Enable Redis")
set(ENABLE_ENCODINGS OFF CACHE BOOL "Enable Encodings")
set(ENABLE_MONGODB OFF CACHE BOOL "Enable MongoDB")
set(ENABLE_DATA_SQLITE OFF CACHE BOOL "Enable Data SQlite")
set(ENABLE_ZIP OFF CACHE BOOL "Enable Zip")
set(ENABLE_PAGECOMPILER OFF CACHE BOOL "Enable PageCompiler")
set(ENABLE_PAGECOMPILER_FILE2PAGE OFF CACHE BOOL "Enable File2Page")
set(BUILD_SHARED_LIBS ON)
add_subdirectory(poco)

if(NOT TARGET glm)
    message(STATUS "Gathering GLM from submodules")
    option(GLM_QUIET "No CMake Message" ON)
    option(GLM_TEST_ENABLE "Build unit tests" OFF)
    option(GLM_TEST_ENABLE_CXX_14 "Enable C++ 14" ON)
    set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
    set(CMAKE_INSTALL_LIBDIR lib)
    add_subdirectory(glm)

    # WAR for https://github.com/g-truc/glm/issues/854
    if(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
      target_compile_options(glm INTERFACE -Wno-error=class-memaccess)
    endif()
endif()

if(BRAYNS_VIEWER_ENABLED)
  # GLFW
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  add_subdirectory(glfw)

  # IMGUI
  add_library(imgui STATIC
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_glfw.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_opengl2.cpp
    )
  target_compile_definitions(imgui PRIVATE -DIMGUI_IMPL_OPENGL_LOADER_GLEW)
  target_link_libraries(imgui glfw ${OPENGL_gl_LIBRARY})
  target_include_directories(imgui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
endif()

# PBRT V3
if(BRAYNS_PBRT_ENABLED)
    SET(PBRT_FLOAT_AS_DOUBLE ON CACHE BOOL "Use 64-bit floats")
    add_subdirectory(pbrt-v3)
    # Disable compilation warnings
    target_compile_options(pbrt PUBLIC -w)
endif()

# PBRT V2
if(BRAYNS_PBRTV2_ENABLED)
    add_subdirectory(pbrt-v2)
    # Disable compilation warnings
    target_compile_options(pbrtlibv2 PUBLIC -w)
endif()

# Brion if needed
if(BRAYNS_CIRCUITVIEWER_ENABLED OR BRAYNS_CIRCUITEXPLORER_ENABLED OR BRAYNS_CIRCUITINFO_ENABLED)
    common_find_package(Brion)
    if((NOT Brion_FOUND AND NOT BRION_FOUND) AND NOT TARGET Brion)
        message(STATUS "Brion not found, gathering from submodules")
        add_subdirectory(Brion)
    else()
        message(STATUS "Found Brion")
    endif()
endif()
