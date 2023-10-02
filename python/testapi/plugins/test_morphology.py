# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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
from testapi.render import render_and_validate
from testapi.simple_test_case import SimpleTestCase


class TestMorphology(SimpleTestCase):
    def test_original(self) -> None:
        self.run_tests(
            "original",
            brayns.Morphology(
                load_dendrites=True,
                load_axon=True,
                geometry_type=brayns.GeometryType.ORIGINAL,
            ),
        )

    def test_smooth(self) -> None:
        self.run_tests(
            "smooth",
            brayns.Morphology(
                load_dendrites=True,
            ),
        )

    def test_radius_multiplier(self) -> None:
        self.run_tests(
            "radius_multiplier",
            brayns.Morphology(
                radius_multiplier=3,
                load_dendrites=True,
            ),
        )

    def test_constant_radius(self) -> None:
        self.run_tests(
            "constant_radius",
            brayns.Morphology(
                load_dendrites=True,
                geometry_type=brayns.GeometryType.CONSTANT_RADII,
            ),
        )

    def test_spheres(self) -> None:
        self.run_tests(
            "spheres",
            brayns.Morphology(
                load_dendrites=True,
                geometry_type=brayns.GeometryType.SPHERES,
            ),
        )

    def test_resampling(self) -> None:
        self.run_tests(
            "resampling",
            brayns.Morphology(
                load_dendrites=True,
                resampling=0.99,
            ),
        )

    def test_subsampling(self) -> None:
        self.run_tests(
            "subsampling",
            brayns.Morphology(
                load_dendrites=True,
                subsampling=10,
            ),
        )

    def run_tests(self, ref: str, morphology: brayns.Morphology) -> None:
        loader = brayns.MorphologyLoader(morphology)
        models = loader.load_models(self.instance, self.morphology_file)
        self.assertEqual(len(models), 1)
        brayns.set_model_color(self.instance, models[0].id, brayns.Color4.red)
        render_and_validate(self, ref)
