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

import pathlib

import brayns
from testapi.simple_test_case import SimpleTestCase


class GeometryTestCase(SimpleTestCase):
    @property
    def ref(self) -> pathlib.Path:
        name = self.filename.replace("test_add_", "") + ".png"
        return self.folder / name

    def run_tests(
        self, geometries: list[brayns.Geometry], bounds: brayns.Bounds | None = None
    ) -> None:
        model = brayns.add_geometries(self.instance, geometries)
        self._check_model(model, bounds)
        self.quick_validation(self.ref)

    def _check_model(self, model: brayns.Model, bounds: brayns.Bounds | None) -> None:
        ref = brayns.get_model(self.instance, model.id)
        self.assertEqual(model, ref)
        if bounds is not None:
            self.assertEqual(model.bounds, bounds)
        self.assertEqual(model.info, {})
        self.assertEqual(model.visible, True)
        self.assertEqual(model.transform, brayns.Transform.identity)
