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
from testapi.loading import add_sphere, load_circuit
from testapi.render import render_and_validate
from testapi.simple_test_case import SimpleTestCase


class TestColoring(SimpleTestCase):
    def test_color_model(self) -> None:
        model = load_circuit(self)
        brayns.color_model(
            self.instance,
            model.id,
            method=brayns.CircuitColorMethod.ID,
            colors={
                "0-500": brayns.Color4.red,
                "501-1000": brayns.Color4.green,
            },
        )
        render_and_validate(self, "color_model")

    def test_set_model_color(self) -> None:
        model = add_sphere(self)
        brayns.set_model_color(self.instance, model.id, brayns.Color4.blue)
        render_and_validate(self, "set_model_color")

    def test_get_color_methods(self) -> None:
        model = load_circuit(self)
        methods = brayns.get_color_methods(self.instance, model.id)
        ref = {
            brayns.ColorMethod.SOLID,
            brayns.CircuitColorMethod.ID,
            brayns.CircuitColorMethod.ETYPE,
            brayns.CircuitColorMethod.LAYER,
            brayns.CircuitColorMethod.MORPHOLOGY,
            brayns.CircuitColorMethod.MTYPE,
        }
        self.assertEqual(len(methods), len(ref))
        self.assertSetEqual(set(methods), ref)

    def test_get_color_values(self) -> None:
        model = load_circuit(self)
        method = brayns.CircuitColorMethod.LAYER
        values = brayns.get_color_values(self.instance, model.id, method)
        ref = [str(i) for i in range(6)]
        self.assertEqual(values, ref)
