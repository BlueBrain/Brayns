# Copyright (c) 2015-2023 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: adrien.fleury@epfl.ch
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3.0 as published
# by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import logging
import sys


class Logger(logging.Logger):
    """Brayns default logger.

    Log to stdout with simple formatting (name, level, message).

    Can be used to change only the log level without creating a new logger.

    :param level: Log level from logging module, defaults to WARN.
    :type level: int, optional
    """

    def __init__(self, level: int = logging.WARN) -> None:
        super().__init__("Brayns", level)
        handler = logging.StreamHandler(sys.stdout)
        format = "[%(name)s][%(levelname)s] %(message)s"
        formatter = logging.Formatter(format)
        handler.setFormatter(formatter)
        self.addHandler(handler)
