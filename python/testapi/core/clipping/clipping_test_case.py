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

import inspect
import pathlib

import brayns
from testapi.simple_test_case import SimpleTestCase


class ClippingTestCase(SimpleTestCase):

    @property
    def ref(self) -> pathlib.Path:
        filename = inspect.getfile(type(self))
        path = pathlib.Path(filename)
        name = path.stem.replace('test_', '') + '.png'
        return path.parent / name

    def run_tests(self, geometry: brayns.ClippingGeometry) -> None:
        self._check_get(geometry)
        self._check_remove(geometry)
        self._check_clear(geometry)
        self._check_render(geometry)

    def _add_geometry(self, geometry: brayns.ClippingGeometry) -> brayns.Model:
        return brayns.add_clipping_geometry(self.instance, geometry)

    def _check_get(self, geometry: brayns.ClippingGeometry) -> None:
        test = self._add_geometry(geometry)
        ref = brayns.get_model(self.instance, test.id)
        self.assertEqual(test, ref)
        brayns.clear_models(self.instance)

    def _check_remove(self, geometry: brayns.ClippingGeometry) -> None:
        test = self._add_geometry(geometry)
        brayns.remove_models(self.instance, [test.id])
        with self.assertRaises(brayns.JsonRpcError):
            brayns.get_model(self.instance, test.id)
        brayns.clear_models(self.instance)

    def _check_clear(self, geometry: brayns.ClippingGeometry) -> None:
        test = self._add_geometry(geometry)
        remain = [
            brayns.add_geometries(self.instance, [brayns.Sphere(1)]),
            brayns.add_light(self.instance, brayns.AmbientLight(1)),
        ]
        brayns.clear_clipping_geometries(self.instance)
        with self.assertRaises(brayns.JsonRpcError):
            brayns.get_model(self.instance, test.id)
        for model in remain:
            brayns.get_model(self.instance, model.id)
        brayns.clear_models(self.instance)

    def _check_render(self, geometry: brayns.ClippingGeometry) -> None:
        self._add_geometry(geometry)
        brayns.add_geometries(self.instance, [brayns.Sphere(1)])
        self.quick_validation(self.ref)
        brayns.clear_models(self.instance)
