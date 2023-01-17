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

import brayns

from .geometry_test_case import GeometryTestCase


class TestAddPlanes(GeometryTestCase):
    @property
    def geometries(self) -> list[brayns.Geometry]:
        return [
            brayns.Plane(
                brayns.PlaneEquation(1, 1, 1, 0),
            ).with_color(brayns.Color4.red),
            brayns.Plane(
                brayns.PlaneEquation(0, 0, 1, 0.25),
            ).with_color(brayns.Color4.blue),
        ]

    def get_default_camera(self) -> brayns.Camera:
        return brayns.Camera(
            view=brayns.View(
                position=brayns.Axis.z,
                target=brayns.Vector3.zero,
            )
        )

    def test_all(self) -> None:
        self.run_tests(self.geometries)
