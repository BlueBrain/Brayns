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


class TestNeuronalTracer(SimpleTestCase):

    def test_trace_anterograde(self) -> None:
        id = self._load_model()
        tracer = brayns.NeuronalTracer(
            source_cells=[0, 1, 2],
            target_cells=[3, 4, 5],
            source_color=brayns.Color4.red,
            connected_color=brayns.Color4.green,
            non_connected_color=brayns.Color4.blue,
        )
        tracer.trace_anterograde(self.instance, id)

    def _load_model(self) -> int:
        loader = brayns.BbpLoader()
        models = loader.load_models(self.instance, self.bbp_circuit)
        return models[0].id
