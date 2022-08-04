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

from collections.abc import Callable
from typing import Any, TypeVar

from brayns.core.geometry.geometry import Geometry
from brayns.core.model.deserialize_model import deserialize_model
from brayns.core.model.model import Model
from brayns.instance.instance import Instance

T = TypeVar('T', bound=Geometry)


def add_geometries(
    instance: Instance,
    method: str,
    geometries: list[T],
    serializer: Callable[[T], dict[str, Any]]
) -> Model:
    params = _serialize_geometries(geometries, serializer)
    result = instance.request(method, params)
    return deserialize_model(result)


def _serialize_geometries(geometries: list[T], serializer: Callable[[T], dict[str, Any]]) -> list[dict[str, Any]]:
    return [
        {
            'geometry': serializer(geometry),
            'color': list(geometry.color),
        }
        for geometry in geometries
    ]
