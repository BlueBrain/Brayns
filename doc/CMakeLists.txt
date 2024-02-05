# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

if(NOT TARGET doxygen)
    add_custom_target(doxygen)
    set_target_properties(doxygen PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD ON)
endif()
if(NOT TARGET doxycopy)
    add_custom_target(doxycopy)
    set_target_properties(doxycopy PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD ON)
endif()
add_dependencies(doxycopy doxygen)
