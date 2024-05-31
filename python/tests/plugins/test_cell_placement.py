# Copyright (c) 2015-2024 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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


class TestCellPlacementLoader(unittest.TestCase):
    def test_name(self) -> None:
        self.assertEqual(brayns.CellPlacementLoader.name, "Cell placement loader")

    def test_get_properties(self) -> None:
        loader = brayns.CellPlacementLoader(
            morphologies_folder="/a/random/folder",
            density=0.7,
            extension="ext",
            ids=[1, 2, 3],
            morphology=brayns.Morphology(
                load_soma=True,
                load_dendrites=True,
                load_axon=True,
                resampling=2,
                subsampling=1,
            ),
        )
        self.assertEqual(
            loader.get_properties(),
            {
                "morphology_folder": "/a/random/folder",
                "percentage": 0.7,
                "extension": "ext",
                "ids": [1, 2, 3],
                "morphology_parameters": {
                    "geometry_type": "smooth",
                    "load_axon": True,
                    "load_dendrites": True,
                    "load_soma": True,
                    "radius_multiplier": 1.0,
                    "resampling": 2,
                    "subsampling": 1,
                    "growth": 1,
                },
            },
        )
