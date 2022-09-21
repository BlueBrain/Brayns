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
from typing import cast

import brayns
from testapi.image_validator import ImageValidator
from testapi.simple_test_case import SimpleTestCase


class TestBbpLoader(SimpleTestCase):

    @property
    def ref(self) -> pathlib.Path:
        return self.asset_folder / 'bbp.png'

    @property
    def output(self) -> pathlib.Path:
        folder = pathlib.Path(__file__).parent
        return folder / 'bbp.png'

    def test_load(self) -> None:
        loader = brayns.BbpLoader(
            cells=brayns.BbpCells.from_density(0.5),
            report=brayns.BbpReport.compartment('somas'),
            morphology=brayns.Morphology(
                load_dendrites=True,
            )
        )
        models = loader.load(self.instance, self.circuit)
        self.assertEqual(len(models), 1)
        model = models[0]
        self._snapshot(model.bounds)
        validator = ImageValidator()
        validator.validate_file(self.output, self.ref)

    def _snapshot(self, bounds: brayns.Bounds) -> None:
        snapshot = self._create_snapshot(bounds)
        view = cast(brayns.View, snapshot.view)
        self._adjust_lights(view)
        snapshot.save(self.instance, str(self.output))

    def _create_snapshot(self, bounds: brayns.Bounds) -> brayns.Snapshot:
        projection = brayns.PerspectiveProjection()
        view = projection.fovy.get_front_view(bounds)
        renderer = brayns.InteractiveRenderer()
        return brayns.Snapshot(
            resolution=brayns.Resolution.full_hd,
            frame=50,
            view=view,
            camera=projection,
            renderer=renderer,
        )

    def _adjust_lights(self, view: brayns.View) -> None:
        light = brayns.DirectionalLight(
            intensity=5,
            direction=view.direction
        )
        brayns.add_light(self.instance, light)
