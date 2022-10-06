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

from .vector3 import Vector3


def componentwise_max(values: list[Vector3]) -> Vector3:
    """Return maximum of each component among values.

    If values is empty, zero is returned.

    :param values: List of vectors.
    :type values: list[Vector3]
    :return: Max value for each component.
    :rtype: Vector3
    """
    if not values:
        return Vector3.zero
    return Vector3(
        max(value.x for value in values),
        max(value.y for value in values),
        max(value.z for value in values),
    )
