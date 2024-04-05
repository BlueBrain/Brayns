# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

brayns_library(circuit)

set_target_properties(circuit PROPERTIES OUTPUT_NAME braynsCircuit)

target_link_libraries(
    circuit
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
target_compile_definitions(circuit PRIVATE optional_CONFIG_SELECT_OPTIONAL=1)
target_compile_definitions(circuit PRIVATE variant_CONFIG_SELECT_VARIANT=1)
