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

import unittest
from typing import Any

import brayns
from tests.mock_instance import MockInstance


class TestPick(unittest.TestCase):
    def test_pick(self) -> None:
        result = self.mock_message(hit=True)
        instance = MockInstance(result)
        position = brayns.Vector2(0.5, 0.6)
        test = brayns.pick(instance, position)
        self.assertEqual(test, self.mock_result())
        self.assertEqual(instance.params, {"position": [0.5, 0.6]})

    def test_pick_not_hit(self) -> None:
        result = self.mock_message(hit=False)
        instance = MockInstance(result)
        position = brayns.Vector2(0.5, 0.6)
        test = brayns.pick(instance, position)
        self.assertIsNone(test)
        self.assertEqual(instance.params, {"position": [0.5, 0.6]})

    def mock_message(self, hit: bool) -> dict[str, Any]:
        return {
            "hit": hit,
            "position": [1, 2, 3],
            "model_id": 3,
            "metadata": {"test": 1},
        }

    def mock_result(self) -> brayns.PickResult:
        return brayns.PickResult(
            position=brayns.Vector3(1, 2, 3),
            model_id=3,
            metadata={"test": 1},
        )
