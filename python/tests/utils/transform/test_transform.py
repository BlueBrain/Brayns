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

from .mock_transform import MockTransform


class TestTransform(unittest.TestCase):

    def test_identity(self) -> None:
        test = brayns.Transform.identity
        self.assertEqual(test.translation, brayns.Vector3.zero)
        self.assertEqual(test.rotation, brayns.Rotation.identity)
        self.assertEqual(test.scale, brayns.Vector3.one)

    def test_to_dict(self) -> None:
        test = MockTransform.transform.to_dict()
        ref = MockTransform.message
        self.assertEqual(test, ref)

    def test_from_dict(self) -> None:
        test = brayns.Transform.from_dict(MockTransform.message)
        ref = MockTransform.transform
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
