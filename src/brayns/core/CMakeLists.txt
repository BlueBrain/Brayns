# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

brayns_library(core)

set_target_properties(core PROPERTIES OUTPUT_NAME braynsCore)

target_link_libraries(
    core
    PUBLIC
    OpenMP::OpenMP_CXX
    rkcommon::rkcommon
    spdlog::spdlog
    Poco::Foundation
    Poco::JSON
    Poco::Net
    Poco::NetSSL
    ospray::ospray
    PRIVATE
    stb
    tinyexr
    ${ZLIB_LIBRARIES}
)
