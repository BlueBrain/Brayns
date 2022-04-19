# Copyright (c) 2015-2022, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSDTI_SOURCES
  components/DTIComponent.cpp
  components/SpikeReportComponent.cpp
  io/DTILoader.cpp
  DTIPlugin.cpp
)

set(BRAYNSDTI_HEADERS
  components/DTIComponent.h
  components/SpikeReportComponent.h
  io/DTILoader.h
  io/DTILoaderParameters.h
  DTIPlugin.h
)

add_library(braynsDTI SHARED ${BRAYNSDTI_SOURCES} ${BRAYNSDTI_HEADERS})

target_include_directories(braynsDTI PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
  "$<INSTALL_INTERFACE:include>")
target_link_libraries(braynsDTI PRIVATE Brion Brain braynsPluginAPI braynsEngine braynsNetwork)

set_target_properties(braynsDTI
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsDTI
    EXPORT braynsDTI-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsDTI-targets
    DESTINATION share/braynsDTI/CMake
)
