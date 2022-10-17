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
from testapi.quick_render import prepare_quick_export
from testapi.simple_test_case import SimpleTestCase


class TestFrameExporter(SimpleTestCase):

    @property
    def output(self) -> pathlib.Path:
        folder = pathlib.Path(__file__).parent
        return folder / 'frames'

    @property
    def ref(self) -> pathlib.Path:
        return self.asset_folder / 'frames'

    def test_export_frames(self) -> None:
        self._load_circuit()
        exporter = self._prepare_export()
        exporter.export_frames(self.instance, str(self.output))
        self._check_frames()

    def test_export_frames_task(self) -> None:
        self._load_circuit()
        exporter = self._prepare_export()
        task = exporter.export_frames_task(self.instance, str(self.output))
        self.assertEqual(len(list(task)), 5)
        task.wait_for_result()
        self._check_frames()

    def _load_circuit(self) -> None:
        loader = brayns.BbpLoader(
            report=brayns.BbpReport.compartment('somas'),
            morphology=brayns.Morphology(radius_multiplier=10),
        )
        loader.load_models(self.instance, self.bbp_circuit)

    def _prepare_export(self) -> brayns.FrameExporter:
        frames = brayns.MovieFrames(
            fps=5,
            slowing_factor=100,
        )
        simulation = brayns.get_simulation(self.instance)
        indices = frames.get_indices(simulation)
        self.output.mkdir(exist_ok=True)
        return prepare_quick_export(self.instance, indices)

    def _check_frames(self) -> None:
        errors = list[str]()
        validator = ImageValidator()
        for test in self.output.glob('*.png'):
            ref = self.ref / test.name
            try:
                validator.validate_file(test, ref)
            except RuntimeError as e:
                errors.append(str(e))
        self.output.rmdir()
        self.assertFalse(errors)
