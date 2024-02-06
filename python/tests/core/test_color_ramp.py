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


class TestColorRamp(unittest.TestCase):
    def test_get_color_ramp(self) -> None:
        instance = MockInstance(self.mock_color_ramp_message())
        test = brayns.get_color_ramp(instance, 0)
        self.assertEqual(test, self.mock_color_ramp())
        self.assertEqual(instance.method, "get-color-ramp")
        self.assertEqual(instance.params, {"id": 0})

    def test_set_color_ramp(self) -> None:
        instance = MockInstance()
        brayns.set_color_ramp(instance, 0, self.mock_color_ramp())
        self.assertEqual(instance.method, "set-color-ramp")
        self.assertEqual(
            instance.params,
            {
                "id": 0,
                "color_ramp": self.mock_color_ramp_message(),
            },
        )

    def mock_color_ramp(self) -> brayns.ColorRamp:
        return brayns.ColorRamp(
            value_range=brayns.ValueRange(0, 1),
            colors=[
                brayns.Color4.red,
                brayns.Color4.blue,
            ],
        )

    def mock_color_ramp_message(self) -> dict[str, Any]:
        return {
            "range": [0, 1],
            "colors": [
                [1, 0, 0, 1],
                [0, 0, 1, 1],
            ],
        }
