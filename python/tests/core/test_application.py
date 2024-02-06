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


class TestApplication(unittest.TestCase):
    def test_get_application(self) -> None:
        instance = MockInstance(self.mock_application_message())
        test = brayns.get_application(instance)
        self.assertEqual(test, self.mock_application())

    def test_set_resolution(self) -> None:
        instance = MockInstance()
        brayns.set_resolution(instance, brayns.Resolution(100, 200))
        self.assertEqual(instance.method, "set-application-parameters")
        self.assertEqual(instance.params, {"viewport": [100, 200]})

    def test_stop(self) -> None:
        instance = MockInstance()
        brayns.stop(instance)
        self.assertEqual(instance.method, "quit")
        self.assertIsNone(instance.params)

    def mock_application(self) -> brayns.Application:
        return brayns.Application(
            plugins=["test1", "test2"],
            resolution=brayns.Resolution.full_hd,
        )

    def mock_application_message(self) -> dict[str, Any]:
        return {
            "plugins": ["test1", "test2"],
            "viewport": [1920, 1080],
        }
