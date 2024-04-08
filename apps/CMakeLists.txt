# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
# Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
# Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

add_executable(service service.cpp)

set_target_properties(service PROPERTIES OUTPUT_NAME braynsService)

target_link_libraries(service PRIVATE brayns::core)

if(BRAYNS_ENABLE_CIRCUIT)
    target_link_libraries(service PRIVATE brayns::circuit)
    target_compile_definitions(service PRIVATE "BRAYNS_ENABLE_CIRCUIT")
endif()

if(BRAYNS_ENABLE_ATLAS)
    target_link_libraries(service PRIVATE brayns::atlas)
    target_compile_definitions(service PRIVATE "BRAYNS_ENABLE_ATLAS")
endif()

install(
    TARGETS service
    EXPORT BraynsTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
