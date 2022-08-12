# Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

from dataclasses import dataclass

from brayns.utils import Error


@dataclass
class MovieError(Error):
    """Exception raised if an error occurs when making a movie.

    :param reason: Description of what happened.
    :type reason: str
    :param code: Error code (FFMPEG output code).
    :type code: int
    :param logs: FFMPEG logs if any.
    :type logs: str
    """

    reason: str
    code: int = 0
    logs: str = ''

    def __str__(self) -> str:
        return self.reason
