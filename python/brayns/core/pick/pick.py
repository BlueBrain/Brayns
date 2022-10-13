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

from brayns.network import Instance
from brayns.utils import Vector2

from .deserialize_pick_result import deserialize_pick_result
from .pick_result import PickResult


def pick(instance: Instance, position: Vector2) -> PickResult | None:
    """Pick a given screen position and inspect it.

    Screen position must be normalized with [0, 0] being bottom-left of
    and [1, 1] top-right.

    If a model is found on given coordinates, return its ID with the world
    position corresponding to the pixel where the model was rendered.

    If no models are found, then None is returned.

    :param instance: Instance to inspect.
    :type instance: Instance
    :param x: Screen position X normalized.
    :type x: float
    :return: Information about model in given XY if any, otherwise None.
    :rtype: PickResult | None
    """
    params = {'position': list(position)}
    result = instance.request('inspect', params)
    return deserialize_pick_result(result)
