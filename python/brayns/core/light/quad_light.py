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

from brayns.core.common.vector3 import Vector3
from brayns.core.light.light import Light


@dataclass
class QuadLight(Light):

    bottom_left: Vector3 = Vector3.zero
    edge1: Vector3 = Vector3.right
    edge2: Vector3 = Vector3.up

    @classmethod
    @property
    def name(self) -> str:
        return 'quad'

    @property
    def emission_direction(self) -> Vector3:
        return self.edge1.cross(self.edge2)

    def serialize(self) -> dict:
        return self._to_dict({
            'bottom_left_corner': list(self.bottom_left),
            'vertical_displacement': list(self.edge2),
            'horizontal_displacement': list(self.edge1)
        })
