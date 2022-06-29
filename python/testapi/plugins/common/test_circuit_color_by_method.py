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


class TestCircuitColorByMethod(SimpleTestCase):

    def test_get_available_methods(self) -> None:
        id = self._load_circuit()
        methods = brayns.CircuitColorByMethod.get_available_methods(
            self.instance,
            id
        )
        ref = [
            brayns.ColorMethod.LAYER,
            brayns.ColorMethod.MTYPE,
            brayns.ColorMethod.ETYPE,
            brayns.ColorMethod.MORPHOLOGY,
            brayns.ColorMethod.MORPHOLOGY_SECTION
        ]
        self.assertEqual(methods, ref)

    def test_get_available_values(self) -> None:
        id = self._load_circuit()
        values = brayns.CircuitColorByMethod.get_available_values(
            self.instance,
            id,
            brayns.ColorMethod.LAYER
        )
        ref = [str(i) for i in range(6)]
        self.assertEqual(values, ref)

    def test_apply(self) -> None:
        id = self._load_circuit()
        color = brayns.CircuitColorByMethod(brayns.ColorMethod.ETYPE, {
            '0': brayns.Color4.green,
            '1': brayns.Color4.blue,
        })
        color.apply(self.instance, id)

    def _load_circuit(self) -> int:
        loader = brayns.BbpLoader(
            radius_multiplier=10
        )
        models = loader.load(self.instance, self.circuit)
        return models[0].id
