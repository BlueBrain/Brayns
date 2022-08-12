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

import brayns


class TestProductionRenderer(unittest.TestCase):

    def setUp(self) -> None:
        self._renderer = brayns.ProductionRenderer(
            samples_per_pixel=2,
            max_ray_bounces=12,
            background_color=brayns.Color4(0, 0, 1, 1)
        )
        self._message = {
            'samples_per_pixel': 2,
            'max_ray_bounces': 12,
            'background_color': [0, 0, 1, 1]
        }

    def test_name(self) -> None:
        test = brayns.ProductionRenderer.name
        ref = 'production'
        self.assertEqual(test, ref)

    def test_deserialize(self) -> None:
        test = brayns.ProductionRenderer.deserialize(self._message)
        self.assertEqual(test, self._renderer)

    def test_serialize(self) -> None:
        test = self._renderer.serialize()
        self.assertEqual(test, self._message)

    def test_serialize_with_name(self) -> None:
        renderer = brayns.ProductionRenderer()
        test = renderer.serialize_with_name()
        ref = {
            'name': renderer.name,
            'params': renderer.serialize()
        }
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
