# Copyright (c) 2015-2019, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

# Async++
message(STATUS "Gathering Async++ from submodules")
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build Async++ as a shared library" FORCE)
add_subdirectory(async++)

# Perceptual Diff
message(STATUS "Gathering PerceptualDiff from submodules")
add_subdirectory(perceptualdiff)

if(NOT Poco_Found)
    message(STATUS "Gathering Poco from submodules")
    # Both Poco and GLM create "uninstall" targets. But GLM check wether the
    # target exists before or not, whereas POCO doesnt give a damn.
    set(ENABLE_REDIS OFF CACHE BOOL "Enable Redis")
    set(ENABLE_ENCODINGS OFF CACHE BOOL "Enable Encodings")
    set(ENABLE_APACHECONNECTOR OFF CACHE BOOL "Enable Apache")
    set(ENABLE_MONGODB OFF CACHE BOOL "Enable MongoDB")
    set(ENABLE_DATA_POSTGRESQL OFF CACHE BOOL "Enable PostgreSQL")
    set(ENABLE_DATA_MYSQL OFF CACHE BOOL "Enable MySQL")
    set(ENABLE_DATA_ODBC OFF CACHE BOOL "Enable Data ODBC")
    set(ENABLE_DATA_SQLITE OFF CACHE BOOL "Enable Data SQlite")
    set(ENABLE_ZIP OFF CACHE BOOL "Enable Zip")
    set(ENABLE_PAGECOMPILER OFF CACHE BOOL "Enable PageCompiler")
    set(ENABLE_PAGECOMPILER_FILE2PAGE OFF CACHE BOOL "Enable File2Page")
    set(BUILD_SHARED_LIBS ON)
    add_subdirectory(poco)

    # Remove Poco warnings
    if(NOT CMAKE_BUILD_TYPE MATCHES DEBUG)
        target_compile_options(Foundation PUBLIC -w)
    endif()

    # Remove Poco postfix d in debug
    set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "Set Debug library postfix" FORCE)
    set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "Set Release library postfix" FORCE)
    set(CMAKE_MINSIZEREL_POSTFIX "" CACHE STRING "Set MinSizeRel library postfix" FORCE)
    set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "Set RelWithDebInfo library postfix" FORCE)
endif()

if(NOT glm_FOUND AND NOT TARGET glm)
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

# spdlog
message(STATUS "Gathering spdlog from submodules")
set(SPDLOG_BUILD_SHARED ON CACHE BOOL "" FORCE)
set(SPDLOG_MASTER_PROJECT ON)
add_subdirectory(spdlog)

if(BRAYNS_VIEWER_ENABLED AND NOT glfw3_FOUND)
  # GLFW
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  add_subdirectory(glfw)
endif()

if(BRAYNS_VIEWER_ENABLED)
  # IMGUI
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
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/imgui>"
      "$<INSTALL_INTERFACE:include>")

  install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/imgui
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

# --------------------------------------------------------------------------------
# sonata (which also brings HighFive)
# --------------------------------------------------------------------------------
if(BRAYNS_CIRCUITEXPLORER_ENABLED)
    find_package(sonata QUIET)
    if(NOT sonata_FOUND AND NOT TARGET sonata::sonata_shared)
        message(STATUS "Gathering libsonata from submodules")

        set(EXTLIB_FROM_SUBMODULES ON CACHE BOOL "" FORCE)
        set(SONATA_TESTS OFF CACHE BOOL "" FORCE)
        add_subdirectory(libsonata)
        add_library(sonata::sonata_shared ALIAS sonata_shared)
    endif()
endif()

# --------------------------------------------------------------------------------
# MorphIO
# --------------------------------------------------------------------------------
if(BRAYNS_CIRCUITEXPLORER_ENABLED)
    find_package(MorphIO QUIET)
    if(NOT MorphIO_FOUND AND NOT TARGET MorphIO::morphio_shared)

        set(BUILD_BINDINGS OFF CACHE BOOL "" FORCE)
        set(MORPHIO_TESTS OFF CACHE BOOL "" FORCE)

        set(MORPHIO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/MorphIO/include/)

        # Deps
        add_subdirectory(MorphIO/3rdparty/GSL_LITE)
        target_include_directories(gsl-lite SYSTEM INTERFACE)
        add_library(lexertl INTERFACE)
        target_include_directories(lexertl SYSTEM INTERFACE MorphIO/3rdparty/lexertl14/include)

        # Retrieve version
        set(MORPHIO_VERSION_STRING "3.3.0")
        set(MorphIO_VERSION_MAJOR 3)
        set(MorphIO_VERSION_MINOR 3)
        set(MorphIO_VERSION_PATCH 0)
        # Morphio
        add_subdirectory(MorphIO/src)
        add_library(morphiointer INTERFACE)
        target_include_directories(morphiointer INTERFACE ${CMAKE_CURRENT_LIST_DIR}/MorphIO/include)
        target_link_libraries(morphiointer INTERFACE morphio_shared)
        add_library(MorphIO::morphio_shared ALIAS morphiointer)
    endif()
endif()

# Brion if needed
if(BRAYNS_CIRCUITEXPLORER_ENABLED OR BRAYNS_CIRCUITINFO_ENABLED)
    find_package(Brion QUIET)
    if(NOT Brion_FOUND AND NOT TARGET Brion)
        message(STATUS "Gathering Brion from submodules")
        add_subdirectory(Brion)
    endif()
endif()
