# Copyright 2015-2024 Blue Brain Project/EPFL
# Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
#                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

add_library(braynsPluginAPI INTERFACE)

target_include_directories(braynsPluginAPI INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(braynsPluginAPI INTERFACE
    braynsParameters braynsEngine braynsIO)
