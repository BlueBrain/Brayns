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
from .rotation import Rotation


def euler(x: float, y: float, z: float, degrees: bool = False) -> Rotation:
    """Shortcut to build a rotation from euler angles.

    :param x: X rotation.
    :type x: float
    :param y:  Y rotation.
    :type y: float
    :param z:  Z rotation.
    :type z: float
    :param degrees: Wether given angles are in degrees, defaults to False
    :type degrees: bool, optional
    :return: Rotation corresponding to angles.
    :rtype: Rotation
    """
    angles = Vector3(x, y, z)
    return Rotation.from_euler(angles, degrees)
