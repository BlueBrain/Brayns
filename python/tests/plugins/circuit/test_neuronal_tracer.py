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

import unittest

import brayns
from tests.mock_instance import MockInstance


class TestNeuronalTracer(unittest.TestCase):

    def test_trace_anterograde(self) -> None:
        instance = MockInstance()
        tracer = brayns.NeuronalTracer(
            source_cells=[1, 2, 3],
            target_cells=[4, 5, 6],
            source_color=brayns.Color4.red,
            connected_color=brayns.Color4.green,
            non_connected_color=brayns.Color4.blue,
        )
        tracer.trace_anterograde(instance, 1)
        self.assertEqual(instance.method, 'trace-anterograde')
        self.assertEqual(instance.params, {
            'model_id': 1,
            'cell_gids': [1, 2, 3],
            'target_cell_gids': [4, 5, 6],
            'source_cell_color': [1, 0, 0, 1],
            'connected_cells_color': [0, 1, 0, 1],
            'non_connected_cells_color': [0, 0, 1, 1],
        })


if __name__ == '__main__':
    unittest.main()
