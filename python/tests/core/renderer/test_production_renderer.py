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

import unittest
from typing import Any

import brayns


class TestProductionRenderer(unittest.TestCase):
    @classmethod
    @property
    def renderer(cls) -> brayns.ProductionRenderer:
        return brayns.ProductionRenderer(
            samples_per_pixel=3,
            max_ray_bounces=2,
            background_color=brayns.Color4(0, 0, 1, 1),
        )

    @classmethod
    @property
    def message(cls) -> dict[str, Any]:
        return {
            "samples_per_pixel": 3,
            "max_ray_bounces": 2,
            "background_color": [0, 0, 1, 1],
        }

    def test_name(self) -> None:
        test = brayns.ProductionRenderer.name
        ref = "production"
        self.assertEqual(test, ref)

    def test_get_properties(self) -> None:
        self.assertEqual(self.renderer.get_properties(), self.message)

    def test_update_properties(self) -> None:
        test = brayns.ProductionRenderer()
        test.update_properties(self.message)
        self.assertEqual(test, self.renderer)
