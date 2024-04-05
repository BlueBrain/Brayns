# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

brayns_library(atlas)

set_target_properties(atlas PROPERTIES OUTPUT_NAME braynsAtlas)

target_include_directories(atlas PRIVATE ${ZLIB_INCLUDE_DIRS} ${BZIP2_INCLUDE_DIRS})

target_link_libraries(
    atlas
    PUBLIC
    brayns::core
    PRIVATE
    OpenMP::OpenMP_CXX
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
)
