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

import brayns
from testapi.loading import add_clip_plane, add_light, add_sphere
from testapi.render import render_and_validate
from testapi.simple_test_case import SimpleTestCase


class TestModel(SimpleTestCase):
    def test_get_model(self) -> None:
        model = add_sphere(self)
        test = brayns.get_model(self.instance, model.id)
        self.assertEqual(test, model)

    def test_get_scene(self) -> None:
        models = [
            add_sphere(self),
            add_light(self),
            add_clip_plane(self),
        ]
        test = brayns.get_scene(self.instance)
        self.assertEqual(test.bounds, models[0].bounds)
        self.assertEqual(test.models, models)

    def test_remove_models(self) -> None:
        models = [
            add_sphere(self),
            add_sphere(self),
            add_light(self),
            add_light(self),
            add_clip_plane(self),
            add_clip_plane(self),
        ]
        brayns.remove_models(self.instance, [model.id for model in models[::2]])
        for model in models[::2]:
            with self.assertRaises(brayns.JsonRpcError):
                brayns.get_model(self.instance, model.id)
        for model in models[1::2]:
            brayns.get_model(self.instance, model.id)

    def test_clear_models(self) -> None:
        models = [
            add_sphere(self),
            add_light(self),
            add_clip_plane(self),
        ]
        brayns.clear_models(self.instance)
        for model in models:
            with self.assertRaises(brayns.JsonRpcError):
                brayns.get_model(self.instance, model.id)

    def test_clear_renderables(self) -> None:
        models = [
            add_sphere(self),
            add_sphere(self),
            add_light(self),
            add_clip_plane(self),
        ]
        brayns.clear_renderables(self.instance)
        with self.assertRaises(brayns.JsonRpcError):
            brayns.get_model(self.instance, models[0].id)
        with self.assertRaises(brayns.JsonRpcError):
            brayns.get_model(self.instance, models[1].id)
        brayns.get_model(self.instance, models[2].id)
        brayns.get_model(self.instance, models[3].id)

    def test_instantiate_model(self) -> None:
        sphere = (brayns.Sphere(20), brayns.Color4.red)
        model = brayns.add_geometries(self.instance, [sphere])
        ref_position = brayns.Vector3(0, -30, 0)
        ref = (brayns.Sphere(5, ref_position), brayns.Color4.blue)
        brayns.add_geometries(self.instance, [ref])
        transform = brayns.Transform(brayns.Vector3(50, 0, 0))
        instantiated = brayns.instantiate_model(self.instance, model.id, [transform])
        self.assertEqual(instantiated[0].transform, transform)
        render_and_validate(self, "instantiate_model")

    def test_update_bounds(self) -> None:
        ref = add_sphere(self)
        translation = brayns.Vector3.one
        transform = brayns.Transform(translation)
        test = brayns.update_model(self.instance, ref.id, transform)
        ref.transform = transform
        ref.bounds.min += translation
        ref.bounds.max += translation
        self.assertEqual(test, ref)

    def test_update_transform(self) -> None:
        add_sphere(self)
        model = brayns.add_geometries(
            self.instance,
            [
                (
                    brayns.BoundedPlane(
                        equation=brayns.PlaneEquation(0, 0, 1),
                        bounds=brayns.Bounds(
                            min=brayns.Vector3.zero,
                            max=brayns.Vector3.one,
                        ),
                    ),
                    brayns.Color4.blue,
                )
            ],
        )
        transform = brayns.Transform(
            translation=brayns.Vector3.one,
            rotation=brayns.euler(0, 0, 90, degrees=True),
            scale=brayns.Vector3(1, 2, 3),
        )
        brayns.update_model(self.instance, model.id, transform)
        render_and_validate(self, "update_transform")

    def test_update_visibility(self) -> None:
        model = add_sphere(self)
        brayns.update_model(self.instance, model.id, visible=False)
        render_and_validate(self, "update_invisible")
        brayns.update_model(self.instance, model.id, visible=True)
        render_and_validate(self, "update_visible")
