# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

brayns_library(core)

set_target_properties(core PROPERTIES OUTPUT_NAME braynsCore)

target_include_directories(
    core
    SYSTEM
    PRIVATE
    deps
)

target_link_libraries(
    core
    PUBLIC
    OpenMP::OpenMP_CXX
    ospray::ospray
    Poco::Foundation
    Poco::JSON
    Poco::Net
    Poco::NetSSL
    rkcommon::rkcommon
    spdlog::spdlog
    PRIVATE
    ${ZLIB_LIBRARIES}
)
