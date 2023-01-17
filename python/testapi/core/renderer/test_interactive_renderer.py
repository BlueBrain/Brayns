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

from .renderer_test_case import RendererTestCase


class TestInteractiveRenderer(RendererTestCase):
    def test_all(self) -> None:
        renderer = brayns.InteractiveRenderer(
            samples_per_pixel=2,
            max_ray_bounces=4,
            background_color=brayns.Color4.red,
            enable_shadows=False,
            ambient_occlusion_samples=3,
        )
        self.run_tests(renderer)
