# Copyright (c) 2015-2024 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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


class TestFramebuffer(SimpleTestCase):
    def test_progressive_framebuffer(self) -> None:
        framebuffer = brayns.ProgressiveFramebuffer(scale=4)
        brayns.set_framebuffer(self.instance, framebuffer)
        add_sphere(self)
        self.render_and_validate("progressive_framebuffer_0")
        settings = RenderSettings.raw()
        render_and_validate(self, "progressive_framebuffer_1", settings)

    def test_static_framebuffer(self) -> None:
        framebuffer = brayns.StaticFramebuffer()
        brayns.set_framebuffer(self.instance, framebuffer)
        add_sphere(self)
        self.render_and_validate("static_framebuffer")

    def render_and_validate(self, ref: str) -> None:
        settings = RenderSettings(
            renderer=brayns.InteractiveRenderer(2), use_snapshot=False
        )
        render_and_validate(self, ref, settings)
