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

import brayns
from testapi.simple_test_case import SimpleTestCase


class TestClipPlane(SimpleTestCase):

    def test_remove(self) -> None:
        planes = self._create_planes()
        ids = self._add_planes(planes)
        self.assertEqual(ids, [0, 1, 2])
        brayns.ClipPlane.remove(self.instance, [1, 2])
        id = planes[0].add(self.instance)
        self.assertIn(id, [1, 2])

    def test_clear(self) -> None:
        planes = self._create_planes()
        ids = self._add_planes(planes)
        self.assertEqual(ids, [0, 1, 2])
        brayns.ClipPlane.clear(self.instance)
        ids = self._add_planes(planes)
        self.assertEqual(ids, [0, 1, 2])

    def test_add(self) -> None:
        plane = brayns.ClipPlane(1, 2, 3, 4)
        id = plane.add(self.instance)
        self.assertEqual(id, 0)

    def _create_planes(self) -> list[brayns.ClipPlane]:
        return [
            brayns.ClipPlane(1, 2, 3, 4),
            brayns.ClipPlane(5, 6, 7, 8),
            brayns.ClipPlane(5, 6, 7, 7)
        ]

    def _add_planes(self, planes: list[brayns.ClipPlane]) -> list[int]:
        return [
            id
            for id in (
                plane.add(self.instance)
                for plane in planes
            )
        ]
