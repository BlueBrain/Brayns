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

from brayns.utils import Color4

from .value_range import ValueRange


@dataclass
class ColorRamp:
    """Color ramp to map simulation values to colors.

    Simulation values below value_range.min have colors[0].

    Simulation values above value_range.max have colors[-1].

    Otherwise, the two closest colors in the list are interpolated linearly.

    :param value_range: Simulation value range (usually voltages).
    :type value_range: ValueRange
    :param colors: List of colors mapped to value range.
    :type colors: list[Color4]
    """

    value_range: ValueRange
    colors: list[Color4]
