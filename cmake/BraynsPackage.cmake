# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

install(
    EXPORT BraynsTargets
    FILE BraynsTargets.cmake
    DESTINATION cmake
    NAMESPACE brayns::
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    cmake/BraynsConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/BraynsConfig.cmake
    INSTALL_DESTINATION cmake
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/BraynsConfigVersion.cmake
    VERSION ${BRAYNS_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/BraynsConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/BraynsConfigVersion.cmake
    DESTINATION cmake
)
