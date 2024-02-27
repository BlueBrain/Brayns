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
from brayns.plugins.morphology import serialize_morphology


class TestMorphology(unittest.TestCase):
    def test_serialize_morphology(self) -> None:
        test = brayns.Morphology(
            radius_multiplier=3,
            load_soma=False,
            load_axon=True,
            load_dendrites=True,
            geometry_type=brayns.GeometryType.CONSTANT_RADII,
            resampling=0.5,
            subsampling=5,
        )
        self.assertEqual(
            serialize_morphology(test),
            {
                "radius_multiplier": 3,
                "load_soma": False,
                "load_axon": True,
                "load_dendrites": True,
                "geometry_type": brayns.GeometryType.CONSTANT_RADII.value,
                "resampling": 0.5,
                "subsampling": 5,
            },
        )


class TestMorphologyLoader(unittest.TestCase):
    def test_name(self) -> None:
        self.assertEqual(brayns.MorphologyLoader.name, "Neuron Morphology loader")

    def test_properties(self) -> None:
        loader = brayns.MorphologyLoader(
            morphology=brayns.Morphology(
                radius_multiplier=3,
                load_soma=True,
                load_axon=True,
                load_dendrites=True,
                geometry_type=brayns.GeometryType.ORIGINAL,
                resampling=2,
                subsampling=1,
            )
        )
        properties = {
            "radius_multiplier": 3.0,
            "load_soma": True,
            "load_axon": True,
            "load_dendrites": True,
            "geometry_type": "original",
            "resampling": 2.0,
            "subsampling": 1,
        }
        self.assertEqual(loader.get_properties(), properties)
