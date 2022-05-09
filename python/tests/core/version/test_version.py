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

from brayns.core.version.version import Version
from tests.instance.mock_instance import MockInstance


class TestVersion(unittest.TestCase):

    def setUp(self) -> None:
        self._version = Version(
            major=0,
            minor=1,
            patch=2,
            revision='3'
        )
        self._message = {
            'major': 0,
            'minor': 1,
            'patch': 2,
            'revision': '3'
        }

    def test_from_instance(self) -> None:
        instance = MockInstance(self._message)
        test = Version.from_instance(instance)
        self.assertEqual(test, self._version)

    def test_deserialize(self) -> None:
        test = Version.deserialize(self._message)
        self.assertEqual(test, self._version)

    def test_release(self) -> None:
        test = self._version.release
        ref = (0, 1, 2)
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
