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

from ..vector import Vector3


def lower_bound(values: list[Vector3]) -> Vector3:
    """Take the smallest value for each component and make a new vector.

    Example: lower_bound([1, -2, 3], [-1, 2, -3]) = [-1, -2, -3].

    Return zero if values are empty.

    :param values: Values to find the lower bound.
    :type values: list[Vector3]
    :return: Lowest value for each component among given values.
    :rtype: Vector3
    """
    if not values:
        return Vector3.zero
    return Vector3(
        min(value.x for value in values),
        min(value.y for value in values),
        min(value.z for value in values),
    )
