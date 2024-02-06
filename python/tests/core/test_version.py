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


class TestVersion(unittest.TestCase):
    def test_get_version(self) -> None:
        instance = MockInstance(self.mock_version_message())
        test = brayns.get_version(instance)
        self.assertEqual(test, self.mock_version())

    def test_release(self) -> None:
        version = self.mock_version()
        self.assertEqual(version.release, (0, 1, 2))

    def test_tag(self) -> None:
        version = self.mock_version()
        self.assertEqual(version.tag, "0.1.2")

    def test_check(self) -> None:
        version = self.mock_version()
        version.check(version.tag)
        with self.assertRaises(brayns.VersionError):
            version.check("5.6.7")

    def mock_version(self) -> brayns.Version:
        return brayns.Version(major=0, minor=1, patch=2, revision="3")

    def mock_version_message(self) -> dict[str, Any]:
        return {"major": 0, "minor": 1, "patch": 2, "revision": "3"}
