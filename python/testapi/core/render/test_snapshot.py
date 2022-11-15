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


class TestSnapshot(SimpleTestCase):

    @property
    def ref_png(self) -> pathlib.Path:
        return self.folder / 'snapshot.png'

    @property
    def ref_jpg(self) -> pathlib.Path:
        return self.folder / 'snapshot.jpg'

    def test_save(self) -> None:
        snapshot = self._prepare_snapshot()
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'test_save.png'
            snapshot.save(self.instance, str(path))
            validator = ImageValidator()
            validator.validate_file(path, self.ref_png)

    def test_save_task(self) -> None:
        snapshot = self._prepare_snapshot()
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'test_save_task.jpg'
            task = snapshot.save_task(self.instance, str(path))
            self.assertTrue(list(task))
            task.wait_for_result()
            validator = ImageValidator()
            validator.validate_file(path, self.ref_jpg)

    def test_save_remotely(self) -> None:
        snapshot = self._prepare_snapshot()
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'test_save_remotely.png'
            snapshot.save_remotely(self.instance, str(path))
            validator = ImageValidator()
            validator.validate_file(path, self.ref_png)

    def test_save_remotely_task(self) -> None:
        snapshot = self._prepare_snapshot()
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'test_save_remotely_task.jpg'
            task = snapshot.save_remotely_task(self.instance, str(path))
            self.assertTrue(list(task))
            task.wait_for_result()
            validator = ImageValidator()
            validator.validate_file(path, self.ref_jpg)

    def test_download(self) -> None:
        snapshot = self._prepare_snapshot()
        test = snapshot.download(self.instance)
        validator = ImageValidator()
        validator.validate_data(test, self.ref_png)

    def test_download_task(self) -> None:
        snapshot = self._prepare_snapshot()
        task = snapshot.download_task(self.instance)
        self.assertTrue(list(task))
        test = task.wait_for_result()
        validator = ImageValidator()
        validator.validate_data(test, self.ref_png)

    def test_cancel(self) -> None:
        snapshot = self._prepare_snapshot()
        snapshot.renderer = brayns.InteractiveRenderer(1000)
        task = snapshot.download_task(self.instance)
        task.cancel()
        with self.assertRaises(brayns.JsonRpcError):
            task.wait_for_result()

    def _prepare_snapshot(self) -> brayns.Snapshot:
        self.add_light()
        self.add_sphere()
        return self.snapshot()
