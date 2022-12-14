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

import pathlib

import brayns
from testapi.simple_test_case import SimpleTestCase


class TestMorphologyLoader(SimpleTestCase):
    def test_original(self) -> None:
        self._load_and_render(
            morphology=brayns.Morphology(
                load_dendrites=True,
                load_axon=True,
                geometry_type=brayns.GeometryType.ORIGINAL,
            ),
            ref=self.folder / "original.png",
        )

    def test_smooth(self) -> None:
        self._load_and_render(
            morphology=brayns.Morphology(
                load_dendrites=True,
            ),
            ref=self.folder / "smooth.png",
        )

    def test_radius_multiplier(self) -> None:
        self._load_and_render(
            morphology=brayns.Morphology(
                radius_multiplier=3,
                load_dendrites=True,
            ),
            ref=self.folder / "radius_multiplier.png",
        )

    def test_constant_radius(self) -> None:
        self._load_and_render(
            morphology=brayns.Morphology(
                load_dendrites=True,
                geometry_type=brayns.GeometryType.CONSTANT_RADII,
            ),
            ref=self.folder / "constant_radius.png",
        )

    def test_resampling(self) -> None:
        self._load_and_render(
            morphology=brayns.Morphology(
                load_dendrites=True,
                resampling=0.99,
            ),
            ref=self.folder / "resampling.png",
        )

    def test_subsampling(self) -> None:
        self._load_and_render(
            morphology=brayns.Morphology(
                load_dendrites=True,
                subsampling=10,
            ),
            ref=self.folder / "subsampling.png",
        )

    def _load_and_render(
        self, morphology: brayns.Morphology, ref: pathlib.Path
    ) -> None:
        loader = brayns.MorphologyLoader(morphology)
        models = loader.load_models(self.instance, self.morphology_file)
        self.assertEqual(len(models), 1)
        brayns.set_model_color(self.instance, models[0].id, brayns.Color4.red)
        self.quick_validation(ref)
