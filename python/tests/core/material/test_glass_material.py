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


class TestGlassMaterial(unittest.TestCase):
    def test_name(self) -> None:
        self.assertEqual(brayns.GlassMaterial.name, "glass")

    def test_get_properties(self) -> None:
        test = brayns.GlassMaterial(2.5)
        self.assertEqual(test.get_properties(), {"index_of_refraction": 2.5})

    def test_update_properties(self) -> None:
        test = brayns.GlassMaterial()
        test.update_properties({"index_of_refraction": 2.5})
        self.assertEqual(test.refraction_index, 2.5)
