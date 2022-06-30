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


class TestModel(SimpleTestCase):

    def test_from_instance(self) -> None:
        for model in self._add_models():
            test = brayns.Model.from_instance(self.instance, model.id)
            self.assertEqual(test, model)

    def test_get_all(self) -> None:
        models = self._add_models()
        tests = brayns.Model.get_all(self.instance)
        self.assertEqual(models, tests)

    def test_get_bounds(self) -> None:
        models = self._add_models()
        bounds = brayns.Model.get_bounds(self.instance)
        ref = models[0].bounds
        self.assertEqual(bounds, ref)

    def test_remove(self) -> None:
        models = self._add_models()
        brayns.Model.remove(self.instance, [0, 1])
        refs = [
            model
            for model in models
            if model.id not in [0, 1]
        ]
        tests = brayns.Model.get_all(self.instance)
        self.assertEqual(tests, refs)

    def test_clear(self) -> None:
        self._add_models()
        brayns.Model.clear(self.instance)
        tests = brayns.Model.get_all(self.instance)
        self.assertFalse(tests)

    def test_update(self) -> None:
        models = self._add_models()
        model = models[0]
        transform = brayns.Transform(
            translation=brayns.Vector3.one,
            rotation=brayns.Quaternion.identity,
            scale=2 * brayns.Vector3.one
        )
        model = brayns.Model.update(
            self.instance,
            model.id,
            transform=transform
        )
        self.assertEqual(model.transform, transform)
        bounds = brayns.Model.get_bounds(self.instance)
        ref = brayns.Bounds(
            -brayns.Vector3.one,
            3 * brayns.Vector3.one
        )
        self.assertEqual(bounds, ref)

    def _add_models(self) -> list[brayns.Model]:
        boxes = brayns.Boxes([
            (
                brayns.Box(-brayns.Vector3.one, brayns.Vector3.one),
                brayns.Color4.red
            )
        ])
        return [
            boxes.add(self.instance)
            for _ in range(3)
        ]
