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

from brayns.plugins.coloring.cell_id import CellId


class TestCellId(unittest.TestCase):

    def test_from_integer(self) -> None:
        test = CellId.from_integer(3)
        ref = '3'
        self.assertEqual(test.value, ref)

    def test_from_integers(self) -> None:
        values = [1, 3, 6]
        test = CellId.from_integers(values)
        ref = '1,3,6'
        self.assertEqual(test.value, ref)

    def test_from_strings(self) -> None:
        values = ['1', '3', '6']
        test = CellId.from_strings(values)
        ref = '1,3,6'
        self.assertEqual(test.value, ref)

    def test_from_range(self) -> None:
        test = CellId.from_range(0, 3)
        ref = '0-3'
        self.assertEqual(test.value, ref)

    def test_or(self) -> None:
        test = CellId.from_integer(3) | CellId.from_range(10, 13) | CellId('5')
        ref = '3,10-13,5'
        self.assertEqual(test.value, ref)


if __name__ == '__main__':
    unittest.main()
