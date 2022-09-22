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

from dataclasses import dataclass, field

from brayns.utils import Bounds, Vector3, View

from .perspective_projection import PerspectiveProjection
from .projection import Projection


@dataclass
class Camera:
    """Camera used to render.

    A camera is composed of a ``View`` and a ``Projection``.

    The view defines the camera position and orientation while the projection
    defines how the model will be projected from 3D to 2D.

    To focus the camera to a given target (bounds) use ``look_at`` to set the
    default full-screen front view.
    """

    view: View = field(default_factory=View)
    projection: Projection = field(default_factory=PerspectiveProjection)

    @property
    def name(self) -> str:
        return self.projection.name

    @property
    def position(self) -> Vector3:
        return self.view.position

    @position.setter
    def position(self, value: Vector3) -> None:
        self.view.position = value

    @property
    def target(self) -> Vector3:
        return self.view.target

    @target.setter
    def target(self, value: Vector3) -> None:
        self.view.target = value

    @property
    def up(self) -> Vector3:
        return self.view.up

    @up.setter
    def up(self, value: Vector3) -> None:
        self.view.up = value

    @property
    def direction(self) -> Vector3:
        return self.view.direction

    def look_at(self, target: Bounds) -> None:
        self.projection.set_target(target)
        self.view = self.projection.get_front_view(target)
