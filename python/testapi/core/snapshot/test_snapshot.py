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
from testapi.simple_test_case import SimpleTestCase


class TestSnapshot(SimpleTestCase):

    @property
    def output(self) -> pathlib.Path:
        folder = pathlib.Path(__file__).parent
        return folder / 'test_snapshot.png'

    @property
    def ref(self) -> pathlib.Path:
        return self.asset_folder / 'snapshot.png'

    def test_save(self) -> None:
        snapshot = self._prepare_snapshot()
        snapshot.save(self.instance, str(self.output))
        self._check_snapshot()

    def test_save_remotely(self) -> None:
        snapshot = self._prepare_snapshot()
        snapshot.save_remotely(self.instance, str(self.output))
        self._check_snapshot()

    def test_download(self) -> None:
        snapshot = self._prepare_snapshot()
        data = snapshot.download(self.instance)
        self._check_snapshot_data(data)

    def _check_snapshot(self) -> None:
        with self.output.open('rb') as file:
            data = file.read()
        self.output.unlink()
        self._check_snapshot_data(data)

    def _check_snapshot_data(self, data: bytes) -> None:
        with self.ref.open('rb') as file:
            ref = file.read()
        self.assertEqual(data, ref)

    def _prepare_snapshot(self) -> brayns.Snapshot:
        model = self._load_model()
        snapshot = self._create_snapshot(model.bounds)
        self._add_light(snapshot.view.direction)
        return snapshot

    def _add_light(self, direction: brayns.Vector3) -> None:
        light = brayns.DirectionalLight(
            intensity=5,
            direction=direction
        )
        light.add(self.instance)

    def _load_model(self) -> brayns.Model:
        path = self.asset_folder / 'cube.ply'
        loader = brayns.MeshLoader()
        models = loader.load(self.instance, str(path))
        return models[0]

    def _create_snapshot(self, bounds: brayns.Bounds) -> brayns.Snapshot:
        camera = brayns.PerspectiveCamera()
        view = camera.get_full_screen_view(bounds)
        view.position += brayns.Vector3.forward
        renderer = brayns.InteractiveRenderer()
        return brayns.Snapshot(
            resolution=brayns.Resolution.full_hd,
            frame=0,
            view=view,
            camera=camera,
            renderer=renderer
        )
