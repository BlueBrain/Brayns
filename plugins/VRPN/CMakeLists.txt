# Copyright (c) 2015-2021, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

find_package(VRPN REQUIRED)

add_library(braynsVRPN SHARED VRPNPlugin.cpp VRPNPlugin.h)

target_link_libraries(braynsVRPN PRIVATE
    braynsCommon braynsPluginAPI braynsEngine braynsNetwork ${VRPN_LIBRARIES}
)

set_target_properties(braynsVRPN
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsVRPN
    EXPORT braynsVRPN-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsVRPN-targets
    DESTINATION share/braynsVRPN/CMake
)
