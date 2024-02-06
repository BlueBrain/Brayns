# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
from tests.mock_instance import MockInstance


class TestRenderer(unittest.TestCase):
    def test_get_renderer_name(self) -> None:
        name = "test"
        instance = MockInstance(name)
        test = brayns.get_renderer_name(instance)
        self.assertEqual(test, name)
        self.assertEqual(instance.method, "get-renderer-type")
        self.assertIsNone(instance.params)

    def test_get_renderer(self) -> None:
        ref = brayns.ProductionRenderer()
        instance = MockInstance(ref.get_properties())
        test = brayns.get_renderer(instance, brayns.ProductionRenderer)
        self.assertEqual(test, ref)
        self.assertEqual(instance.method, "get-renderer-production")
        self.assertIsNone(instance.params)

    def test_set_renderer(self) -> None:
        instance = MockInstance()
        renderer = brayns.ProductionRenderer()
        brayns.set_renderer(instance, renderer)
        self.assertEqual(instance.method, "set-renderer-production")
        self.assertEqual(instance.params, renderer.get_properties())

    def test_interactive(self) -> None:
        renderer = brayns.InteractiveRenderer(
            samples_per_pixel=3,
            max_ray_bounces=2,
            background_color=brayns.Color4(0, 0, 1, 1),
            enable_shadows=False,
            ambient_occlusion_samples=1,
        )
        message = {
            "samples_per_pixel": 3,
            "max_ray_bounces": 2,
            "background_color": [0, 0, 1, 1],
            "enable_shadows": False,
            "ao_samples": 1,
        }
        self.assertEqual(renderer.name, "interactive")
        self.assertEqual(renderer.get_properties(), message)
        test = brayns.InteractiveRenderer()
        test.update_properties(message)
        self.assertEqual(test, renderer)

    def test_production(self) -> None:
        renderer = brayns.ProductionRenderer(
            samples_per_pixel=3,
            max_ray_bounces=2,
            background_color=brayns.Color4(0, 0, 1, 1),
        )
        message = {
            "samples_per_pixel": 3,
            "max_ray_bounces": 2,
            "background_color": [0, 0, 1, 1],
        }
        self.assertEqual(renderer.name, "production")
        self.assertEqual(renderer.get_properties(), message)
        test = brayns.ProductionRenderer()
        test.update_properties(message)
        self.assertEqual(test, renderer)
