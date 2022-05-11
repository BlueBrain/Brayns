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

from brayns.core.common.color import Color
from brayns.core.renderer.interactive_renderer import InteractiveRenderer


class TestInteractiveRenderer(unittest.TestCase):

    def setUp(self) -> None:
        self._renderer = InteractiveRenderer(
            samples_per_pixel=2,
            max_ray_bounces=12,
            background_color=Color(0, 0, 1, 1),
            enable_shadows=False,
            ambient_occlusion_samples=15
        )
        self._message = {
            'samples_per_pixel': 2,
            'max_ray_bounces': 12,
            'background_color': [0, 0, 1, 1],
            'enable_shadows': False,
            'ao_samples': 15
        }

    def test_default(self) -> None:
        test = InteractiveRenderer.default()
        self.assertEqual(test.samples_per_pixel, 1)
        self.assertEqual(test.max_ray_bounces, 3)
        self.assertEqual(test.enable_shadows, True)
        self.assertEqual(test.ambient_occlusion_samples, 0)

    def test_name(self) -> None:
        test = InteractiveRenderer.name
        ref = 'interactive'
        self.assertEqual(test, ref)

    def test_deserialize(self) -> None:
        test = InteractiveRenderer.deserialize(self._message)
        self.assertEqual(test, self._renderer)

    def test_serialize(self) -> None:
        test = self._renderer.serialize()
        self.assertEqual(test, self._message)


if __name__ == '__main__':
    unittest.main()
