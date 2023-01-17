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
from testapi.simple_test_case import SimpleTestCase


class TestSetColorRamp(SimpleTestCase):
    def test_set_color_ramp(self) -> None:
        model = self.load_circuit(report=True)
        ramp = brayns.ColorRamp(
            brayns.ValueRange(-100, 0),
            colors=[brayns.Color4.red, brayns.Color4.green],
        )
        brayns.set_color_ramp(self.instance, model.id, ramp)
        self._check_get(model, ramp)
        brayns.enable_simulation(self.instance, model.id, True)
        self._check_render()

    def _check_get(self, model: brayns.Model, ramp: brayns.ColorRamp) -> None:
        test = brayns.get_color_ramp(self.instance, model.id)
        self.assertEqual(test, ramp)

    def _check_render(self) -> None:
        ref = self.folder / "frame_0.png"
        self.quick_validation(ref, 0)
        ref = self.folder / "frame_99.png"
        self.quick_validation(ref, 99)
