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
