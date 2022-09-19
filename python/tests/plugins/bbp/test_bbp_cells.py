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


class TestBbpCells(unittest.TestCase):

    def test_all(self) -> None:
        cells = brayns.BbpCells.all()
        self.assertEqual(cells, brayns.BbpCells.from_density(1))

    def test_from_density(self) -> None:
        density = 0.5
        cells = brayns.BbpCells.from_density(density)
        self.assertEqual(cells.density, density)
        self.assertIsNone(cells.targets)
        self.assertIsNone(cells.gids)

    def test_from_targets(self) -> None:
        targets = ['1', '2', '3']
        cells = brayns.BbpCells.from_targets(targets)
        self.assertEqual(cells.density, 1.0)
        self.assertEqual(cells.targets, targets)
        self.assertIsNone(cells.gids)

    def test_from_targets_and_density(self) -> None:
        targets = ['1', '2', '3']
        density = 0.5
        cells = brayns.BbpCells.from_targets(targets, density)
        self.assertEqual(cells.density, density)
        self.assertEqual(cells.targets, targets)
        self.assertIsNone(cells.gids)

    def test_from_gids(self) -> None:
        gids = [1, 2, 3]
        cells = brayns.BbpCells.from_gids(gids)
        self.assertIsNone(cells.density)
        self.assertIsNone(cells.targets)
        self.assertEqual(cells.gids, gids)


if __name__ == '__main__':
    unittest.main()
