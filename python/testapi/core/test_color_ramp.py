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
from testapi.loading import load_circuit
from testapi.render import RenderSettings, render_and_validate
from testapi.simple_test_case import SimpleTestCase


class TestGetColorRamp(SimpleTestCase):
    def test_get_color_ramp(self) -> None:
        model = load_circuit(self, report=True)
        function = brayns.get_color_ramp(self.instance, model.id)
        self.assertEqual(function.value_range, brayns.ValueRange(-80, -10))
        self.assertEqual(len(function.colors), 128)
        self.assertEqual(function.colors[0], brayns.Color4.black)
        self.assertEqual(function.colors[-1], brayns.Color4.white)

    def test_set_color_ramp(self) -> None:
        model = load_circuit(self, report=True)
        ramp = brayns.ColorRamp(
            brayns.ValueRange(-100, 0),
            colors=[brayns.Color4.red, brayns.Color4.green],
        )
        brayns.set_color_ramp(self.instance, model.id, ramp)
        test = brayns.get_color_ramp(self.instance, model.id)
        self.assertEqual(test, ramp)
        brayns.enable_simulation(self.instance, model.id, True)
        settings = RenderSettings(frame=0)
        render_and_validate(self, "frame_0", settings)
        settings.frame = 99
        render_and_validate(self, "frame_99", settings)
