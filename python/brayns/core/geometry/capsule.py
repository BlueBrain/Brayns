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
from brayns.core.geometry.geometry import Geometry


@dataclass
class Capsule(Geometry):

    start_point: Vector3
    start_radius: float
    end_point: Vector3
    end_radius: float

    @classmethod
    @property
    def name(cls) -> str:
        return 'capsules'

    @property
    def properties(self) -> dict:
        return {
            'p0': list(self.start_point),
            'r0': self.start_radius,
            'p1': list(self.end_point),
            'r1': self.end_radius
        }
