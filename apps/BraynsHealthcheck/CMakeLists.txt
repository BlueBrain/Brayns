# Copyright (c) 2015-2023, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

add_executable(braynsHealthcheck main.cpp)

target_link_libraries(braynsHealthcheck PRIVATE
    Poco::Foundation
    Poco::Net
    Poco::NetSSL
)

install(TARGETS braynsHealthcheck
    EXPORT braynsHealthcheck-targets
    RUNTIME DESTINATION bin
)

install(EXPORT braynsHealthcheck-targets
    DESTINATION share/CMake
)
