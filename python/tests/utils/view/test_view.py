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

from .mock_view import MockView


class TestView(unittest.TestCase):

    def test_direction(self) -> None:
        test = brayns.View(
            position=brayns.Vector3.zero,
            target=brayns.Vector3.one
        )
        self.assertEqual(test.direction, brayns.Vector3.one.normalized)

    def test_to_dict(self) -> None:
        test = MockView.view.to_dict()
        self.assertEqual(test, MockView.message)

    def test_from_dict(self) -> None:
        test = brayns.View.from_dict(MockView.message)
        self.assertEqual(test, MockView.view)


if __name__ == '__main__':
    unittest.main()
