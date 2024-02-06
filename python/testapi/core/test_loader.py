# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

import inspect

import brayns
from testapi.simple_test_case import SimpleTestCase


def raw_volumne_loader() -> brayns.RawVolumeLoader:
    return brayns.RawVolumeLoader(
        dimensions=brayns.Vector3(256, 256, 112),
        spacing=brayns.Vector3(1.16, 1.16, 2.5),
        data_type=brayns.VolumeDataType.FLOAT,
    )


class TestLoader(SimpleTestCase):
    def test_get_loaders(self) -> None:
        refs = self._get_api_loaders()
        tests = brayns.get_loaders(self.instance)
        names = [test.name for test in tests]
        for ref in refs:
            self.assertIn(ref, names)

    def _get_api_loaders(self) -> set[str]:
        return {
            obj.name
            for obj in brayns.__dict__.values()
            if inspect.isclass(obj)
            and issubclass(obj, brayns.Loader)
            and obj is not brayns.Loader
        }


class TestLoadAndUpload(SimpleTestCase):
    def test_load_models_task(self) -> None:
        path = self.asset_folder / "chest_ct_scan.raw"
        loader = raw_volumne_loader()
        task = loader.load_models_task(self.instance, str(path))
        progresses = list(task)
        self.assertEqual(len(progresses), 3)
        models = task.wait_for_result()
        self.assertEqual(len(models), 1)

    def test_upload_models(self) -> None:
        path = self.asset_folder / "chest_ct_scan.raw"
        loader = raw_volumne_loader()
        with path.open("rb") as file:
            data = file.read()
        models = loader.upload_models(self.instance, loader.RAW, data)
        self.assertEqual(len(models), 1)

    def test_upload_models_task(self) -> None:
        path = self.asset_folder / "chest_ct_scan.raw"
        loader = raw_volumne_loader()
        with path.open("rb") as file:
            data = file.read()
        task = loader.upload_models_task(self.instance, loader.RAW, data)
        progresses = list(task)
        self.assertEqual(len(progresses), 4)
        models = task.wait_for_result()
        self.assertEqual(len(models), 1)


class TestMeshLoader(SimpleTestCase):
    def test_load_models_obj(self) -> None:
        path = self.asset_folder / "cube.obj"
        loader = brayns.MeshLoader()
        models = loader.load_models(self.instance, str(path))
        self.assertEqual(len(models), 1)
        model = models[0]
        ref = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        self.assertEqual(model.bounds, ref)

    def test_load_models_off(self) -> None:
        path = self.asset_folder / "cube.off"
        loader = brayns.MeshLoader()
        models = loader.load_models(self.instance, str(path))
        self.assertEqual(len(models), 1)
        model = models[0]
        ref = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        self.assertEqual(model.bounds, ref)

    def test_load_models_ply(self) -> None:
        path = self.asset_folder / "cube.ply"
        loader = brayns.MeshLoader()
        models = loader.load_models(self.instance, str(path))
        self.assertEqual(len(models), 1)
        model = models[0]
        ref = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        self.assertEqual(model.bounds, ref)

    def test_load_models_stl(self) -> None:
        path = self.asset_folder / "cube.stl"
        loader = brayns.MeshLoader()
        models = loader.load_models(self.instance, str(path))
        self.assertEqual(len(models), 1)
        model = models[0]
        ref = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        self.assertEqual(model.bounds, ref)

    def test_load_models_stl_binary(self) -> None:
        path = self.asset_folder / "cube_binary.stl"
        loader = brayns.MeshLoader()
        models = loader.load_models(self.instance, str(path))
        self.assertEqual(len(models), 1)
        model = models[0]
        min = brayns.Vector3(-0.5, -0.5, 0)
        max = brayns.Vector3(0.5, 0.5, 1)
        ref = brayns.Bounds(min, max)
        self.assertEqual(model.bounds, ref)


class TestMhdVolumeLoader(SimpleTestCase):
    def test_load_model(self) -> None:
        path = self.asset_folder / "chest_ct_scan.mhd"
        loader = brayns.MhdVolumeLoader()
        models = loader.load_models(self.instance, str(path))
        self.assertEqual(len(models), 1)
        model = models[0]
        min_bound = brayns.Vector3.zero
        max_bound = brayns.Vector3(296.96, 296.96, 280)
        ref = brayns.Bounds(min_bound, max_bound)
        self.assertEqual(model.bounds, ref)


class TestRawVolumeLoader(SimpleTestCase):
    def test_load_model(self) -> None:
        path = self.asset_folder / "chest_ct_scan.raw"
        loader = raw_volumne_loader()
        models = loader.load_models(self.instance, str(path))
        self.assertEqual(len(models), 1)
        model = models[0]
        min_bound = brayns.Vector3.zero
        max_bound = brayns.Vector3(296.96, 296.96, 280)
        ref = brayns.Bounds(min_bound, max_bound)
        self.assertEqual(model.bounds, ref)
