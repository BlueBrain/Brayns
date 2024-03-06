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
from tests.mock_messages import mock_model, mock_model_message


class TestLight(unittest.TestCase):
    def test_add_light(self) -> None:
        instance = MockInstance(mock_model_message())
        light = brayns.AmbientLight(3, brayns.Color3.red)
        model = brayns.add_light(instance, light)
        self.assertEqual(model, mock_model())
        self.assertEqual(instance.method, "add-light-ambient")
        self.assertEqual(instance.params, light.get_properties())

    def test_clear_lights(self) -> None:
        instance = MockInstance()
        brayns.clear_lights(instance)
        self.assertEqual(instance.method, "clear-lights")
        self.assertIsNone(instance.params)

    def test_ambient(self) -> None:
        self.assertEqual(brayns.AmbientLight.name, "ambient")
        test = brayns.AmbientLight(12, brayns.Color3.red)
        self.assertEqual(
            test.get_properties(),
            {
                "color": [1, 0, 0],
                "intensity": 12,
            },
        )

    def test_directional(self) -> None:
        self.assertEqual(brayns.DirectionalLight.name, "directional")
        test = brayns.DirectionalLight(direction=brayns.Axis.up)
        self.assertEqual(
            test.get_properties(),
            {
                "intensity": 1,
                "color": [1, 1, 1],
                "direction": [0, 1, 0],
            },
        )

    def test_quad(self) -> None:
        self.assertEqual(brayns.QuadLight.name, "quad")
        light = brayns.QuadLight(edge1=brayns.Axis.up, edge2=brayns.Axis.left)
        self.assertEqual(light.emission_direction, brayns.Axis.front)
        test = brayns.QuadLight()
        self.assertEqual(
            test.get_properties(),
            {
                "intensity": 1,
                "color": [1, 1, 1],
                "position": [0, 0, 0],
                "edge1": [1, 0, 0],
                "edge2": [0, 1, 0],
            },
        )

    def test_sphere(self) -> None:
        self.assertEqual(brayns.SphereLight.name, "sphere")
        test = brayns.SphereLight()
        self.assertEqual(
            test.get_properties(),
            {
                "intensity": 1,
                "color": [1, 1, 1],
                "position": [0, 0, 0],
                "radius": 0,
            },
        )
