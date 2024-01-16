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

from typing import cast

import brayns
from testapi.loading import load_sonata_circuit
from testapi.render import prepare_image
from testapi.simple_test_case import SimpleTestCase


class TestPick(SimpleTestCase):
    def test_hit(self) -> None:
        self._prepare_scene()
        test = brayns.pick(self.instance, brayns.Vector2(0.5, 0.5))
        self.assertIsNotNone(test)
        test = cast(brayns.PickResult, test)
        self.assertAlmostEqual(test.position.x, 195.463, delta=0.01)
        self.assertAlmostEqual(test.position.y, 176.531, delta=0.01)
        self.assertAlmostEqual(test.position.z, 104.240, delta=0.01)
        self.assertEqual(test.model_id, 0)
        self.assertEqual(test.metadata, {"neuron_id": 5})

    def test_missed(self) -> None:
        self._prepare_scene()
        test = brayns.pick(self.instance, brayns.Vector2(0, 0))
        self.assertIsNone(test)

    def test_empty_scene(self) -> None:
        test = brayns.pick(self.instance, brayns.Vector2(0.5, 0.5))
        self.assertIsNone(test)

    def _prepare_scene(self) -> None:
        load_sonata_circuit(self, dendrites=True)
        prepare_image(self.instance)
