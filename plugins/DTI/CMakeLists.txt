cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(NAME BRAYNSDTI)
set(LIBRARY_NAME braynsDTI)
project(${NAME} VERSION 0.1.0)
set(${NAME}_VERSION_ABI 1)

# C++ 14
set (CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake)

find_package(Brion QUIET)


# Source
set(${NAME}_SOURCES
  io/DTILoader.cpp
  io/DTISimulationHandler.cpp
  DTIPlugin.cpp
)

set(${NAME}_PUBLIC_HEADERS
  io/DTILoader.h
  io/DTISimulationHandler.h
  DTIPlugin.h
)

set(${NAME}_LINK_LIBRARIES
  braynsPluginAPI braynsNetwork glm)

# Build type and options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif(NOT CMAKE_BUILD_TYPE)

# Library
include_directories(${BRAYNS_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR})
add_library(${LIBRARY_NAME} SHARED ${${NAME}_SOURCES})
target_link_libraries(
    ${LIBRARY_NAME} PUBLIC
    braynsCommon
    braynsEngine
    braynsParameters
    ${${NAME}_LINK_LIBRARIES})

target_link_libraries(${LIBRARY_NAME} PRIVATE Brion Brain)

# ================================================================================
# Install binaries
# ================================================================================
INSTALL(TARGETS ${LIBRARY_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

# Install headers
INSTALL( FILES ${${NAME}_PUBLIC_HEADERS} DESTINATION include/${LIBRARY_NAME})
