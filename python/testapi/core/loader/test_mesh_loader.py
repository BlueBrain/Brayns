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

import brayns
from testapi.simple_test_case import SimpleTestCase


class TestMeshLoader(SimpleTestCase):

    def test_load_obj(self) -> None:
        path = self.asset_folder / 'cube.obj'
        loader = brayns.MeshLoader()
        models = loader.load(self.instance, str(path))
        self.assertEqual(len(models), 1)
        model = models[0]
        ref = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        self.assertEqual(model.bounds, ref)

    def test_load_off(self) -> None:
        path = self.asset_folder / 'cube.off'
        loader = brayns.MeshLoader()
        models = loader.load(self.instance, str(path))
        self.assertEqual(len(models), 1)
        model = models[0]
        ref = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        self.assertEqual(model.bounds, ref)

    def test_load_ply(self) -> None:
        path = self.asset_folder / 'cube.ply'
        loader = brayns.MeshLoader()
        models = loader.load(self.instance, str(path))
        self.assertEqual(len(models), 1)
        model = models[0]
        ref = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        self.assertEqual(model.bounds, ref)

    def test_load_stl(self) -> None:
        path = self.asset_folder / 'cube.stl'
        loader = brayns.MeshLoader()
        models = loader.load(self.instance, str(path))
        self._check_stl(models)

    def test_load_stl_binary(self) -> None:
        path = self.asset_folder / 'cube_binary.stl'
        loader = brayns.MeshLoader()
        models = loader.load(self.instance, str(path))
        self._check_stl_binary(models)

    def test_load_binary_stl(self) -> None:
        path = self.asset_folder / 'cube.stl'
        loader = brayns.MeshLoader()
        with path.open('rb') as file:
            data = file.read()
        models = loader.load_binary(self.instance, loader.STL, data)
        self._check_stl(models)

    def test_load_binary_stl_binary(self) -> None:
        path = self.asset_folder / 'cube_binary.stl'
        loader = brayns.MeshLoader()
        with path.open('rb') as file:
            data = file.read()
        models = loader.load_binary(self.instance, loader.STL, data)
        self._check_stl_binary(models)

    def _check_stl(self, models: list[brayns.Model]) -> None:
        self.assertEqual(len(models), 1)
        model = models[0]
        ref = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        self.assertEqual(model.bounds, ref)

    def _check_stl_binary(self, models: list[brayns.Model]) -> None:
        self.assertEqual(len(models), 1)
        model = models[0]
        min = brayns.Vector3(-0.5, -0.5, 0)
        max = brayns.Vector3(0.5, 0.5, 1)
        ref = brayns.Bounds(min, max)
        self.assertEqual(model.bounds, ref)
