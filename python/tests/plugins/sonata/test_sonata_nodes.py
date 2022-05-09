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

from brayns.plugins.sonata.sonata_nodes import SonataNodes


class TestSonataNodes(unittest.TestCase):

    def test_all(self) -> None:
        test = SonataNodes.all()
        self.assertEqual(test.density, 1)
        self.assertIsNone(test.names)
        self.assertIsNone(test.ids)

    def test_from_density(self) -> None:
        density = 0.3
        test = SonataNodes.from_density(density)
        self.assertEqual(test.density, density)
        self.assertIsNone(test.names)
        self.assertIsNone(test.ids)

    def test_from_names(self) -> None:
        names = ['test1', 'test2']
        density = 0.3
        test = SonataNodes.from_names(names, density)
        self.assertEqual(test.density, density)
        self.assertEqual(test.names, names)
        self.assertIsNone(test.ids)

    def test_from_ids(self) -> None:
        ids = [1, 2, 3]
        test = SonataNodes.from_ids(ids)
        self.assertIsNone(test.density)
        self.assertIsNone(test.names)
        self.assertEqual(test.ids, ids)

    def test_serialize(self) -> None:
        test = SonataNodes.from_ids([1, 2, 3])
        ref = {
            'node_ids': [1, 2, 3]
        }
        self.assertEqual(test.serialize(), ref)


if __name__ == '__main__':
    unittest.main()
