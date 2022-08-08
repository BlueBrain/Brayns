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
from typing import Any, TypeVar

from brayns.core.bounds.bounds import Bounds
from brayns.core.camera.camera import Camera
from brayns.core.vector.vector3 import Vector3
from brayns.core.view.view import View

T = TypeVar('T', bound='OrthographicCamera')


@dataclass
class OrthographicCamera(Camera):

    height: float = 0.0

    @staticmethod
    def get_front_view(target: Bounds) -> View:
        center = target.center
        distance = target.depth
        position = center + distance * Vector3.forward
        return View(position, center)

    @classmethod
    @property
    def name(cls) -> str:
        return 'orthographic'

    @classmethod
    def deserialize(cls: type[T], message: dict[str, Any]) -> T:
        return cls(
            height=message['height']
        )

    def serialize(self) -> dict[str, Any]:
        return {
            'height': self.height
        }
