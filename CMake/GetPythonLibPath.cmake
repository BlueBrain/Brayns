# Copyright (c) 2015-2022, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(PYTHON_LIBRARY_SUFFIX)
execute_process(COMMAND
  ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/python_lib_path.py
  OUTPUT_VARIABLE PYTHON_LIBRARY_SUFFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
