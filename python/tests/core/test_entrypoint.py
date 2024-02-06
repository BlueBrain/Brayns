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


class TestEntrypoint(unittest.TestCase):
    def test_get_methods(self) -> None:
        methods = ["test1", "test2"]
        instance = MockInstance(methods)
        tests = brayns.get_methods(instance)
        self.assertEqual(instance.method, "registry")
        self.assertIsNone(instance.params)
        self.assertEqual(tests, methods)

    def test_get_entrypoint(self) -> None:
        instance = MockInstance(self.mock_entrypoint_message())
        test = brayns.get_entrypoint(instance, "test")
        self.assertEqual(test, self.mock_entrypoint())
        self.assertEqual(instance.method, "schema")
        self.assertEqual(instance.params, {"endpoint": "test"})

    def mock_entrypoint(self) -> brayns.Entrypoint:
        return brayns.Entrypoint(
            method="test1",
            description="test2",
            plugin="test3",
            asynchronous=True,
            deprecated=True,
            params=brayns.JsonSchema(type=brayns.JsonType.OBJECT),
            result=brayns.JsonSchema(type=brayns.JsonType.ARRAY),
        )

    def mock_entrypoint_message(self) -> dict[str, Any]:
        return {
            "title": "test1",
            "description": "test2",
            "plugin": "test3",
            "async": True,
            "deprecated": True,
            "params": {"type": "object"},
            "returns": {"type": "array"},
        }
