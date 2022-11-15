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
from testapi.simple_test_case import SimpleTestCase


class TestImage(SimpleTestCase):

    def test_save_jpeg(self) -> None:
        self._prepare_render()
        image = brayns.Image()
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'test_render_image.jpg'
            test = image.save(self.instance, str(path))
            self.assertEqual(test.accumulation, 1)
            self.assertEqual(test.max_accumulation, 1)
            self.assertTrue(test.data)
            self.assertTrue(test.full_quality)
            validator = ImageValidator()
            ref = self.folder / 'render_image.jpg'
            validator.validate_file(path, ref)

    def test_download_png(self) -> None:
        self._prepare_render()
        image = brayns.Image()
        test = image.download(self.instance, brayns.ImageFormat.PNG)
        self.assertEqual(test.accumulation, 1)
        self.assertEqual(test.max_accumulation, 1)
        self.assertTrue(test.data)
        self.assertTrue(test.full_quality)
        validator = ImageValidator()
        ref = self.folder / 'render_image.png'
        validator.validate_data(test.data, ref)

    def test_render(self) -> None:
        self._prepare_render()
        image = brayns.Image()
        test = image.render(self.instance)
        self.assertEqual(test.accumulation, 1)
        self.assertEqual(test.max_accumulation, 1)
        self.assertFalse(test.data)
        self.assertTrue(test.full_quality)

    def test_download_no_accumulation(self) -> None:
        self._prepare_render()
        renderer = brayns.InteractiveRenderer(3)
        brayns.set_renderer(self.instance, renderer)
        image = brayns.Image(accumulate=False, force_download=False)
        test = image.download(self.instance)
        self.assertEqual(test.accumulation, 1)
        self.assertEqual(test.max_accumulation, 3)
        self.assertTrue(test.data)
        self.assertFalse(test.full_quality)
        test = image.download(self.instance)
        self.assertEqual(test.accumulation, 2)
        self.assertEqual(test.max_accumulation, 3)
        self.assertTrue(test.data)
        self.assertFalse(test.full_quality)
        test = image.download(self.instance)
        self.assertEqual(test.accumulation, 3)
        self.assertEqual(test.max_accumulation, 3)
        self.assertTrue(test.data)
        self.assertTrue(test.full_quality)
        test = image.download(self.instance)
        self.assertEqual(test.accumulation, 3)
        self.assertEqual(test.max_accumulation, 3)
        self.assertFalse(test.data)
        self.assertTrue(test.full_quality)

    def test_download_accumulation(self) -> None:
        self._prepare_render()
        renderer = brayns.InteractiveRenderer(3)
        brayns.set_renderer(self.instance, renderer)
        image = brayns.Image(force_download=False)
        test = image.download(self.instance)
        self.assertEqual(test.accumulation, 3)
        self.assertEqual(test.max_accumulation, 3)
        self.assertTrue(test.data)
        self.assertTrue(test.full_quality)
        test = image.download(self.instance)
        self.assertEqual(test.accumulation, 3)
        self.assertEqual(test.max_accumulation, 3)
        self.assertFalse(test.data)
        self.assertTrue(test.full_quality)

    def test_force_download(self) -> None:
        self._prepare_render()
        image = brayns.Image(force_download=False)
        test = image.download(self.instance)
        self.assertTrue(test.data)
        test = image.download(self.instance)
        self.assertFalse(test.data)
        image.force_download = True
        test = image.download(self.instance)
        self.assertTrue(test.data)

    def _prepare_render(self) -> None:
        self.add_light()
        self.add_sphere()
        brayns.set_resolution(self.instance, self.resolution)
        brayns.set_renderer(self.instance, self.renderer)
        brayns.set_camera(self.instance, self.get_default_camera())
