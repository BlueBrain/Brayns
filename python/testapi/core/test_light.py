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

import brayns
from testapi.loading import add_clip_plane, add_light, add_sphere
from testapi.render import RenderSettings, render_and_validate
from testapi.simple_test_case import SimpleTestCase


class TestLight(SimpleTestCase):
    def test_clear_lights(self) -> None:
        models = [
            add_sphere(self),
            add_clip_plane(self),
            add_light(self),
            add_light(self),
        ]
        brayns.clear_lights(self.instance)
        brayns.get_model(self.instance, models[0].id)
        brayns.get_model(self.instance, models[1].id)
        with self.assertRaises(brayns.JsonRpcError):
            brayns.get_model(self.instance, models[2].id)
        with self.assertRaises(brayns.JsonRpcError):
            brayns.get_model(self.instance, models[3].id)

    def test_ambient_light(self) -> None:
        light = brayns.AmbientLight(
            intensity=5,
            color=brayns.Color3.red,
        )
        self.run_tests(light)

    def test_directional_light(self) -> None:
        light = brayns.DirectionalLight(
            intensity=10,
            color=brayns.Color3.red,
            direction=brayns.Axis.up,
        )
        self.run_tests(light)

    def test_quad_light(self) -> None:
        light = brayns.QuadLight(
            intensity=10,
            color=brayns.Color3.red,
            bottom_left=-2 * brayns.Axis.x,
            edge1=brayns.Axis.up,
            edge2=brayns.Axis.front,
        )
        self.run_tests(light)

    def run_tests(self, light: brayns.Light) -> None:
        model = brayns.add_light(self.instance, light)
        self.assertTrue(model.visible)
        self.assertEqual(model.transform, brayns.Transform.identity)
        add_sphere(self)
        settings = RenderSettings(add_lights=False)
        filename = f"{light.name}_light"
        render_and_validate(self, filename, settings)
