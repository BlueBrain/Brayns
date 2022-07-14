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


class TestTransferFunction(SimpleTestCase):

    def test_from_model(self) -> None:
        model = self._load_circuit()
        function = brayns.TransferFunction.from_model(self.instance, model.id)
        self.assertEqual(function.value_range, brayns.ValueRange(-80, -10))
        self.assertEqual(len(function.colors), 128)
        self.assertEqual(function.colors[0], brayns.Color4.black)
        self.assertEqual(function.colors[-1], brayns.Color4.white)

    def test_apply(self) -> None:
        model = self._load_circuit()
        function = brayns.TransferFunction(
            brayns.ValueRange(20, 30),
            colors=[
                brayns.Color4.red,
                brayns.Color4.green,
                brayns.Color4.blue
            ]
        )
        function.apply(self.instance, model.id)
        test = brayns.TransferFunction.from_model(self.instance, model.id)
        self.assertEqual(test, function)

    def _load_circuit(self) -> brayns.Model:
        morphology = brayns.MorphologyParameters(
            radius_multiplier=10
        )
        loader = brayns.BbpLoader(
            report=brayns.BbpReport.compartment('somas'),
            morphology=morphology
        )
        models = loader.load(self.instance, self.circuit)
        return models[0]
