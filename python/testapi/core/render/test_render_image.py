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
import tempfile

import brayns
from testapi.image_validator import ImageValidator
from testapi.quick_render import prepare_quick_render_image
from testapi.simple_test_case import SimpleTestCase


class TestRenderImage(SimpleTestCase):

    @property
    def ref_jpg(self) -> pathlib.Path:
        return self.asset_folder / 'render_image.jpg'

    @property
    def ref_png(self) -> pathlib.Path:
        return self.asset_folder / 'render_image.png'

    def test_render_image(self) -> None:
        self._prepare_render()
        image = brayns.render_image(self.instance)
        self.assertTrue(image.received)
        self.assertTrue(image.finished)
        self.assertEqual(image.accumulation, 1)
        self.assertEqual(image.max_accumulation, 1)
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'test_render_image.jpg'
            image.save(str(path))
            validator = ImageValidator()
            validator.validate_file(path, self.ref_jpg)

    def test_render_image_png(self) -> None:
        self._prepare_render()
        image = brayns.render_image(
            self.instance,
            format=brayns.ImageFormat.PNG
        )
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'test_render_image.png'
            image.save(str(path))
            validator = ImageValidator()
            validator.validate_file(path, self.ref_png)

    def test_render_image_accumulation(self) -> None:
        self._prepare_render()
        renderer = brayns.InteractiveRenderer(3)
        brayns.set_renderer(self.instance, renderer)
        image = brayns.render_image(self.instance)
        self.assertTrue(image.received)
        self.assertFalse(image.finished)
        self.assertEqual(image.accumulation, 1)
        self.assertEqual(image.max_accumulation, 3)
        image = brayns.render_image(self.instance)
        self.assertTrue(image.received)
        self.assertFalse(image.finished)
        self.assertEqual(image.accumulation, 2)
        self.assertEqual(image.max_accumulation, 3)
        image = brayns.render_image(self.instance)
        self.assertTrue(image.received)
        self.assertTrue(image.finished)
        self.assertEqual(image.accumulation, 3)
        self.assertEqual(image.max_accumulation, 3)
        image = brayns.render_image(self.instance)
        self.assertFalse(image.received)
        self.assertTrue(image.finished)
        self.assertEqual(image.accumulation, 3)
        self.assertEqual(image.max_accumulation, 3)

    def test_render_image_params(self) -> None:
        self._prepare_render()
        image = brayns.render_image(self.instance, send=False)
        self.assertFalse(image.received)
        image = brayns.render_image(self.instance)
        self.assertFalse(image.received)
        image = brayns.render_image(self.instance, force=True)
        self.assertTrue(image.received)

    def _prepare_render(self) -> None:
        self._load_model()
        prepare_quick_render_image(self.instance)

    def _load_model(self) -> None:
        path = self.asset_folder / 'cube.ply'
        loader = brayns.MeshLoader()
        loader.load_models(self.instance, str(path))
