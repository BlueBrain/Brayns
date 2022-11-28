# Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

import pathlib

import brayns
from testapi.image_validator import ImageValidator
from testapi.simple_test_case import SimpleTestCase


class TestProgressiveFramebuffer(SimpleTestCase):

    def test_progressive_framebuffer(self) -> None:
        self.add_light()
        model = self.add_sphere()
        renderer = brayns.InteractiveRenderer(2, 3, brayns.Color4.white)
        camera = brayns.CameraController(model.bounds).camera
        brayns.set_renderer(self.instance, renderer)
        brayns.set_framebuffer(self.instance, brayns.ProgressiveFramebuffer())
        brayns.set_camera(self.instance, camera)
        self._check_frame(0)
        self._check_frame(1)

    def _check_frame(self, frame_index: int):
        image = brayns.Image(False)
        frame = image.download(self.instance)
        ref = self.folder / f'progressive_frame_{frame_index}.png'
        self._check_dimensions(frame.data, ref)
        validator = ImageValidator()
        validator.validate_data(frame.data, ref)

    def _check_dimensions(self, render_data: bytes, ref: pathlib.Path):
        with ref.open('rb') as file:
            data = file.read()
            self.assertEqual(len(render_data), len(data))
