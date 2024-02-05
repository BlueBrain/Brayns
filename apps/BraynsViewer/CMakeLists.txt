# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

add_executable(braynsViewer
    main.cpp
    input/KeyboardHandler.cpp
    manipulators/AbstractManipulator.cpp
    manipulators/FlyingModeManipulator.cpp
    manipulators/InspectCenterManipulator.cpp
    ui/Application.cpp
)

target_link_libraries(braynsViewer PRIVATE
    brayns
    braynsIO
    glfw
    GLEW
    imgui
)

install(TARGETS braynsViewer
    EXPORT braynsViewer-targets
    RUNTIME DESTINATION bin
)

install(EXPORT braynsViewer-targets
    DESTINATION share/CMake
)
