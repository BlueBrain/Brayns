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


class TestApplication(SimpleTestCase):
    def test_get_application(self) -> None:
        application = brayns.get_application(self.instance)
        self.assertEqual(application.plugins, brayns.Plugin.all)
        self.assertIsInstance(application.resolution, brayns.Resolution)

    def test_set_resolution_and_get(self) -> None:
        brayns.set_resolution(self.instance, brayns.Resolution.ultra_hd)
        test = brayns.get_application(self.instance)
        self.assertEqual(test.resolution, brayns.Resolution.ultra_hd)

    def test_set_resolution_and_render(self) -> None:
        brayns.set_resolution(self.instance, brayns.Resolution.full_hd)
        add_sphere(self)
        settings = RenderSettings(resolution=None, use_snapshot=False)
        render_and_validate(self, "set_resolution", settings)

    def test_stop(self) -> None:
        brayns.stop(self.instance)
        with self.assertRaises(brayns.ConnectionClosedError):
            brayns.get_application(self.instance)
