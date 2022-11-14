# Copyright (c) 2015-2022 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: nadir.romanguerrero@epfl.ch
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

from __future__ import annotations

import inspect
import pathlib

import brayns
from testapi.simple_test_case import SimpleTestCase


class GeometryTestCase(SimpleTestCase):

    @property
    def ref(self) -> pathlib.Path:
        filename = inspect.getfile(type(self))
        path = pathlib.Path(filename)
        name = path.stem.replace('test_add_', '') + '.png'
        return path.parent / name

    def _check_model(self, model: brayns.Model, bounds: brayns.Bounds | None) -> None:
        ref = brayns.get_model(self.instance, model.id)
        self.assertEqual(model, ref)
        if bounds is not None:
            self.assertEqual(model.bounds, bounds)
        self.assertEqual(model.info, {})
        self.assertEqual(model.visible, True)
        self.assertEqual(model.transform, brayns.Transform.identity)

    def run_tests(self, geometries: list[brayns.Geometry], bounds: brayns.Bounds | None = None) -> None:
        self._check_get(geometries, bounds)
        self._check_remove(geometries)
        self._check_clear(geometries)
        self._check_render(geometries)

    def _add_geometries(self, geometries: list[brayns.Geometry]) -> brayns.Model:
        return brayns.add_geometries(self.instance, geometries)

    def _check_get(self, geometries: list[brayns.Geometry], bounds: brayns.Bounds | None) -> None:
        test = self._add_geometries(geometries)
        ref = brayns.get_model(self.instance, test.id)
        self.assertEqual(test, ref)
        if bounds is not None:
            self.assertEqual(test.bounds, bounds)
        self.assertEqual(test.info, {})
        self.assertEqual(test.visible, True)
        self.assertEqual(test.transform, brayns.Transform.identity)
        brayns.clear_models(self.instance)

    def _check_remove(self, geometries: list[brayns.Geometry]) -> None:
        test = self._add_geometries(geometries)
        brayns.remove_models(self.instance, [test.id])
        with self.assertRaises(brayns.JsonRpcError):
            brayns.get_model(self.instance, test.id)
        brayns.clear_models(self.instance)

    def _check_clear(self, geometries: list[brayns.Geometry]) -> None:
        test = self._add_geometries(geometries)
        plane = brayns.ClipPlane(brayns.PlaneEquation(1, 2, 3))
        remain = [
            brayns.add_clipping_geometry(self.instance, plane),
            brayns.add_light(self.instance, brayns.AmbientLight(1)),
        ]
        brayns.clear_renderables(self.instance)
        with self.assertRaises(brayns.JsonRpcError):
            brayns.get_model(self.instance, test.id)
        for model in remain:
            brayns.get_model(self.instance, model.id)
        brayns.clear_models(self.instance)

    def _check_render(self, geometries: list[brayns.Geometry]) -> None:
        self._add_geometries(geometries)
        self.quick_validation(self.ref)
