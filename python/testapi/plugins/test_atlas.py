# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
from testapi.render import RenderSettings, render_and_validate
from testapi.simple_test_case import SimpleTestCase


class TestAtlas(SimpleTestCase):
    def test_density(self) -> None:
        model = self.load_volume(brayns.VoxelType.SCALAR, self.nrrd_density)
        usecases = ["Density", "Highlight column", "Outline mesh shell"]
        self.check_usecases(model, usecases)
        usecase = brayns.AtlasDensity()
        usecase.run(self.instance, model.id)
        self.render_and_validate("atlas_density")

    def test_flatmap(self) -> None:
        model = self.load_volume(brayns.VoxelType.FLATMAP, self.nrrd_flatmap)
        usecases = ["Flatmap areas", "Highlight column", "Outline mesh shell"]
        self.check_usecases(model, usecases)
        usecase = brayns.AtlasFlatmapAreas()
        usecase.run(self.instance, model.id)
        self.render_and_validate("atlas_flatmap")

    def test_orientation(self) -> None:
        model = self.load_volume(brayns.VoxelType.ORIENTATION, self.nrrd_orientation)
        usecases = ["Orientation field", "Highlight column", "Outline mesh shell"]
        self.check_usecases(model, usecases)
        usecase = brayns.AtlasOrientationField()
        usecase.run(self.instance, model.id)
        self.render_and_validate("atlas_orientation")

    def load_volume(self, voxel_type: brayns.VoxelType, path: str) -> brayns.Model:
        loader = brayns.NrrdLoader(voxel_type)
        models = loader.load_models(self.instance, path)
        self.assertEqual(len(models), 1)
        return models[0]

    def check_usecases(self, model: brayns.Model, ref: list[str]) -> None:
        usecases = brayns.get_atlas_usecases(self.instance, model.id)
        self.assertEqual(set(item[0] for item in usecases), set(ref))

    def render_and_validate(self, ref: str) -> None:
        settings = RenderSettings()
        render_and_validate(self, ref, settings)
