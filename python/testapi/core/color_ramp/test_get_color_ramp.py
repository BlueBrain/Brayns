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

import brayns
from testapi.simple_test_case import SimpleTestCase


class TestGetColorRamp(SimpleTestCase):

    def test_get_color_ramp(self) -> None:
        model = self._load_circuit()
        function = brayns.get_color_ramp(self.instance, model.id)
        self.assertEqual(function.value_range, brayns.ValueRange(-80, -10))
        self.assertEqual(len(function.colors), 128)
        self.assertEqual(function.colors[0], brayns.Color4.black)
        self.assertEqual(function.colors[-1], brayns.Color4.white)

    def _load_circuit(self) -> brayns.Model:
        loader = brayns.BbpLoader(
            report=brayns.BbpReport.compartment('somas')
        )
        models = loader.load_models(self.instance, self.circuit)
        return models[0]
