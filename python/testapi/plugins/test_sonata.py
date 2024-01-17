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
from testapi.render import RenderSettings, render_and_validate
from testapi.simple_test_case import SimpleTestCase


class TestSonata(SimpleTestCase):
    def test_load_models(self) -> None:
        loader = brayns.SonataLoader(
            [
                brayns.SonataNodePopulation(
                    name="cerebellum_neurons",
                    nodes=brayns.SonataNodes.from_density(0.5),
                    report=brayns.SonataReport.compartment("test"),
                    morphology=brayns.Morphology(load_dendrites=True, load_axon=True),
                )
            ]
        )
        models = loader.load_models(self.instance, self.sonata_circuit)
        self.assertEqual(len(models), 1)
        ramp = brayns.ColorRamp(
            value_range=brayns.ValueRange(0, 3),
            colors=[brayns.Color4.red, brayns.Color4.blue],
        )
        brayns.set_color_ramp(self.instance, models[0].id, ramp)
        settings = RenderSettings(frame=1)
        render_and_validate(self, "sonata_circuit", settings)
