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
from testapi.image_validator import ImageValidator
from testapi.quick_render import quick_snapshot
from testapi.simple_test_case import SimpleTestCase


class TestBbpLoader(SimpleTestCase):

    @property
    def ref(self) -> pathlib.Path:
        return self.asset_folder / 'bbp.png'

    @property
    def output(self) -> pathlib.Path:
        folder = pathlib.Path(__file__).parent
        return folder / 'bbp.png'

    def test_load_models(self) -> None:
        loader = brayns.BbpLoader(
            cells=brayns.BbpCells.from_density(0.5),
            report=brayns.BbpReport.compartment('somas'),
            morphology=brayns.Morphology(load_dendrites=True),
        )
        models = loader.load_models(self.instance, self.bbp_circuit)
        self._validate_result(models)

    def _validate_result(self, models: list[brayns.Model]) -> None:
        self.assertEqual(len(models), 1)
        quick_snapshot(self.instance, str(self.output), 50)
        validator = ImageValidator()
        validator.validate_file(self.output, self.ref)
