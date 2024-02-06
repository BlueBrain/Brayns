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
from testapi.loading import load_sonata_circuit
from testapi.render import render_and_validate
from testapi.simple_test_case import SimpleTestCase


class TestGetCircuitIds(SimpleTestCase):
    def test_get_circuit_ids(self) -> None:
        model = load_sonata_circuit(self)
        ids = brayns.get_circuit_ids(self.instance, model.id)
        self.assertEqual(ids, list(range(0, 10)))

    def test_set_circuit_thickness(self) -> None:
        model = self.load_neurons([1])
        brayns.set_circuit_thickness(self.instance, model.id, 5)
        render_and_validate(self, "set_circuit_thickness")

    def load_neurons(self, ids: list[int]) -> brayns.Model:
        loader = brayns.SonataLoader(
            [
                brayns.SonataNodePopulation(
                    name="cerebellum_neurons",
                    nodes=brayns.SonataNodes.from_ids(ids),
                    morphology=brayns.Morphology(load_dendrites=True),
                )
            ]
        )
        models = loader.load_models(self.instance, self.sonata_circuit)
        self.assertEqual(len(models), 1)
        return models[0]
