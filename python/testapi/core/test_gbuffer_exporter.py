# Copyright (c) 2015-2024 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Authors:
#   Adrien Christian Fleury <adrien.fleury@epfl.ch>
#   Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

import tempfile
from pathlib import Path

import brayns
from testapi.loading import add_sphere
from testapi.render import (
    RenderSettings,
    get_camera,
    prepare_lights,
    validate_file,
    validate_from_file,
)
from testapi.simple_test_case import SimpleTestCase


class TestGBufferExporter(SimpleTestCase):
    @property
    def ref(self) -> Path:
        return self.asset_folder / "export.exr"

    def test_save(self) -> None:
        exporter = self._prepare_export()
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "export.exr"
            exporter.save(self.instance, str(path))
            validate_file(path, self.ref)

    def test_save_task(self) -> None:
        exporter = self._prepare_export()
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "test_save_task.exr"
            task = exporter.save_task(self.instance, str(path))
            self.assertTrue(list(task))
            task.wait_for_result()
            validate_file(path, self.ref)

    def test_save_remotely(self) -> None:
        exporter = self._prepare_export()
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "test_save_remotely.exr"
            exporter.save_remotely(self.instance, str(path))
            validate_file(path, self.ref)

    def test_save_remotely_task(self) -> None:
        exporter = self._prepare_export()
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "test_save_remotely_task.exr"
            task = exporter.save_remotely_task(self.instance, str(path))
            self.assertTrue(list(task))
            task.wait_for_result()
            validate_file(path, self.ref)

    def test_download(self) -> None:
        exporter = self._prepare_export()
        test = exporter.download(self.instance)
        validate_from_file(test, self.ref)

    def test_download_task(self) -> None:
        exporter = self._prepare_export()
        task = exporter.download_task(self.instance)
        self.assertTrue(list(task))
        test = task.wait_for_result()
        validate_from_file(test, self.ref)

    def test_cancel(self) -> None:
        exporter = self._prepare_export()
        exporter.renderer = brayns.InteractiveRenderer(1000)
        task = exporter.download_task(self.instance)
        task.cancel()
        with self.assertRaises(brayns.JsonRpcError):
            task.wait_for_result()

    def _prepare_export(self) -> brayns.GBufferExporter:
        add_sphere(self)
        prepare_lights(self.instance)

        settings = RenderSettings()
        channels = [
            brayns.GBufferChannel.ALBEDO,
            brayns.GBufferChannel.COLOR,
            brayns.GBufferChannel.DEPTH,
            brayns.GBufferChannel.NORMAL,
        ]
        return brayns.GBufferExporter(
            channels=channels,
            resolution=settings.resolution,
            camera=get_camera(self.instance, settings),
            renderer=settings.renderer,
            frame=settings.frame,
        )
