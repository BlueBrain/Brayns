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
from testapi.loading import add_sphere
from testapi.render import RenderSettings, render_and_validate
from testapi.simple_test_case import SimpleTestCase


class TestRenderer(SimpleTestCase):
    def test_interactive_renderer(self) -> None:
        renderer = brayns.InteractiveRenderer(
            samples_per_pixel=2,
            max_ray_bounces=4,
            background_color=brayns.Color4.red,
            enable_shadows=False,
            ambient_occlusion_samples=3,
        )
        self.run_tests(renderer)

    def test_production_renderer(self) -> None:
        renderer = brayns.ProductionRenderer(
            samples_per_pixel=2,
            max_ray_bounces=4,
            background_color=brayns.Color4.red,
        )
        self.run_tests(renderer)

    def run_tests(self, renderer: brayns.Renderer) -> None:
        add_sphere(self)
        brayns.set_renderer(self.instance, renderer)
        name = brayns.get_renderer_name(self.instance)
        self.assertEqual(name, renderer.name)
        test = brayns.get_renderer(self.instance, type(renderer))
        self.assertEqual(test, renderer)
        settings = RenderSettings(renderer=renderer)
        render_and_validate(self, f"{name}_renderer", settings)
