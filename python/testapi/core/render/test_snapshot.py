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
from testapi.quick_render import prepare_quick_snapshot
from testapi.simple_test_case import SimpleTestCase


class TestSnapshot(SimpleTestCase):

    @property
    def ref(self) -> pathlib.Path:
        return self.asset_folder / 'snapshot.png'

    def test_save(self) -> None:
        snapshot = self._prepare_snapshot()
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'test_save.png'
            snapshot.save(self.instance, str(path))
            validator = ImageValidator()
            validator.validate_file(path, self.ref)

    def test_save_task(self) -> None:
        snapshot = self._prepare_snapshot()
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'test_save_task.png'
            task = snapshot.save_task(self.instance, str(path))
            self.assertTrue(list(task))
            task.wait_for_result()
            validator = ImageValidator()
            validator.validate_file(path, self.ref)

    def test_save_remotely(self) -> None:
        snapshot = self._prepare_snapshot()
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'test_save_remotely.png'
            snapshot.save_remotely(self.instance, str(path))
            validator = ImageValidator()
            validator.validate_file(path, self.ref)

    def test_save_remotely_task(self) -> None:
        snapshot = self._prepare_snapshot()
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'test_save_remotely_task.png'
            task = snapshot.save_remotely_task(self.instance, str(path))
            self.assertTrue(list(task))
            task.wait_for_result()
            validator = ImageValidator()
            validator.validate_file(path, self.ref)

    def test_download(self) -> None:
        snapshot = self._prepare_snapshot()
        test = snapshot.download(self.instance)
        validator = ImageValidator()
        validator.validate_data(test, self.ref)

    def test_download_task(self) -> None:
        snapshot = self._prepare_snapshot()
        task = snapshot.download_task(self.instance)
        self.assertTrue(list(task))
        test = task.wait_for_result()
        validator = ImageValidator()
        validator.validate_data(test, self.ref)

    def test_cancel(self) -> None:
        loader = brayns.BbpLoader()
        loader.load_models(self.instance, self.bbp_circuit)
        snapshot = brayns.Snapshot(
            renderer=brayns.InteractiveRenderer(2000),
        )
        task = snapshot.download_task(self.instance)
        task.cancel()
        with self.assertRaises(brayns.JsonRpcError):
            task.wait_for_result()

    def _prepare_snapshot(self) -> brayns.Snapshot:
        path = self.asset_folder / 'cube.ply'
        loader = brayns.MeshLoader()
        loader.load_models(self.instance, str(path))
        return prepare_quick_snapshot(self.instance)
