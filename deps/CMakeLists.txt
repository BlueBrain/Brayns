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

# --------------------------------------------------------------------------------
# sonata (which also brings HighFive)
# --------------------------------------------------------------------------------
if(BRAYNS_CIRCUITEXPLORER_ENABLED)
    common_find_package(sonata)
    common_find_package(HighFive)
    common_find_package(nlohmann_json)
    if(NOT sonata_FOUND AND NOT TARGET sonata::sonata_shared)
        message(STATUS "Gathering sonata from submodules")
        # Sonata links against gcov, but on Apple this library is not included
        # on the library path
        if(APPLE)
            add_library(gcov INTERFACE)
            # Export target
            install(TARGETS gcov EXPORT gcov-targets)
            install(EXPORT gcov-targets DESTINATION share/gcov/CMake)
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")
            set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")
        endif()
        set(SONATA_VERSION "0.1.9")
        add_subdirectory(libsonata)
        add_library(sonatainter INTERFACE)
        target_include_directories(sonatainter INTERFACE ${CMAKE_CURRENT_LIST_DIR}/libsonata/include)
        target_link_libraries(sonatainter INTERFACE sonata_shared)
        add_library(sonata::sonata_shared ALIAS sonatainter)
    endif()
endif()

# Brion if needed
if(BRAYNS_CIRCUITEXPLORER_ENABLED OR BRAYNS_CIRCUITINFO_ENABLED)
    common_find_package(Brion)
    if((NOT Brion_FOUND AND NOT BRION_FOUND) AND NOT TARGET Brion)
        message(STATUS "Brion not found, gathering from submodules")
        add_subdirectory(Brion)
    else()
        message(STATUS "Found Brion")
    endif()
endif()

# --------------------------------------------------------------------------------
# MorphIO
# --------------------------------------------------------------------------------
if(BRAYNS_CIRCUITEXPLORER_ENABLED)
    common_find_package(MorphIO)
    if(NOT MorphIO_FOUND AND NOT TARGET MorphIO::morphio_shared)
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

# spdlog
common_find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
    message(STATUS "Gathering spdlog from submodules")
    
    find_package(Threads REQUIRED)
    
    add_library(spdlog_header_only IMPORTED GLOBAL INTERFACE)
    add_library(spdlog::spdlog_header_only ALIAS spdlog_header_only)

    target_include_directories(
      spdlog_header_only SYSTEM INTERFACE
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/spdlog/include>"
      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
endif()
