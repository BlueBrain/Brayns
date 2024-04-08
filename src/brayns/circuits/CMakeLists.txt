# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

brayns_library(circuits)

set_target_properties(circuits PROPERTIES OUTPUT_NAME braynsCircuits)

target_link_libraries(
    circuits
    PUBLIC
    brayns::core
    PRIVATE
    OpenMP::OpenMP_CXX
    sonata::sonata_shared
    MorphIO::morphio
    HighFive
)

# libsonata uses nonstd::optional which, if available, becomes std::optional
# however, libsonata is compiled enforcing c++14, so their type is always nonstd::optional
# then, symbol lookup errors happen
target_compile_definitions(circuits PRIVATE optional_CONFIG_SELECT_OPTIONAL=1)
target_compile_definitions(circuits PRIVATE variant_CONFIG_SELECT_VARIANT=1)
