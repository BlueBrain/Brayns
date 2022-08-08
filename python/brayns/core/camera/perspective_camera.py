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

from brayns.core.camera.camera import Camera
from brayns.core.view.fovy import Fovy

T = TypeVar('T', bound='PerspectiveCamera')


@dataclass
class PerspectiveCamera(Camera):

    fovy: Fovy = Fovy(45, degrees=True)
    aperture_radius: float = 0.0
    focus_distance: float = 1.0

    @classmethod
    @property
    def name(cls) -> str:
        return 'perspective'

    @classmethod
    def deserialize(cls: type[T], message: dict[str, Any]) -> T:
        return cls(
            fovy=Fovy(message['fovy'], degrees=True),
            aperture_radius=message['aperture_radius'],
            focus_distance=message['focus_distance'],
        )

    def serialize(self) -> dict[str, Any]:
        return {
            'fovy': self.fovy.degrees,
            'aperture_radius': self.aperture_radius,
            'focus_distance': self.focus_distance,
        }
