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

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from brayns.network import Instance
from brayns.utils import Vector2, Vector3


@dataclass
class PickResult:
    """Information about the model found at an inspected screen position.

    Metadata depend on the kind of model found at given position and are
    specific to the primitive (subpart) hitted.

    :param position: World position matching screen coordinates.
    :type position: Vector3
    :param model_id: ID of them model at screen position.
    :type model_id: int
    :param metadata: Information about the model primitive at given position.
    :type metadata: Any
    """

    position: Vector3
    model_id: int
    metadata: Any


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
    params = {"position": list(position)}
    result = instance.request("inspect", params)
    return _deserialize_pick_result(result)


def _deserialize_pick_result(message: dict[str, Any]) -> PickResult | None:
    hit = message["hit"]
    if not hit:
        return None
    return PickResult(
        position=Vector3(*message["position"]),
        model_id=message["model_id"],
        metadata=message["metadata"],
    )
