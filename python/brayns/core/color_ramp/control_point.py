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

from __future__ import annotations

from dataclasses import dataclass


@dataclass
class ControlPoint:
    """Control point used for opacity curves.

    Normalized values are simulation values (usually voltages) normalized
    between the min and max of the color ramp value range.

    :param normalized_value: Simulation value, normalized.
    :type normalized_value: float
    :param opacity: Color opacity at simulation value.
    :type opacity: float
    """

    normalized_value: float
    opacity: float

    @classmethod
    @property
    def start(cls) -> ControlPoint:
        """Implicit first control point of the opacity curve [0, 0].

        :return: Control point.
        :rtype: ControlPoint
        """
        return ControlPoint(0.0, 0.0)

    @classmethod
    @property
    def end(cls) -> ControlPoint:
        """Implicit last control point of the opacity curve [1, 1].

        :return: Control point.
        :rtype: ControlPoint
        """
        return ControlPoint(1.0, 1.0)
