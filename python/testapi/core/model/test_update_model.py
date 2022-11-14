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


class TestUpdateModel(SimpleTestCase):

    def test_bounds(self) -> None:
        ref = self.add_sphere()
        translation = brayns.Vector3.one
        transform = brayns.Transform(translation)
        test = brayns.update_model(self.instance, ref.id, transform)
        ref.transform = transform
        ref.bounds.min += translation
        ref.bounds.max += translation
        self.assertEqual(test, ref)

    def test_transform(self) -> None:
        self.add_sphere()
        model = brayns.add_geometries(self.instance, [brayns.BoundedPlane(
            equation=brayns.PlaneEquation(0, 0, 1),
            bounds=brayns.Bounds(
                min=brayns.Vector3.zero,
                max=brayns.Vector3.one,
            ),
        ).with_color(brayns.Color4.blue)])
        transform = brayns.Transform(
            translation=brayns.Vector3.one,
            rotation=brayns.euler(0, 0, 90, degrees=True),
            scale=brayns.Vector3(1, 2, 3),
        )
        brayns.update_model(self.instance, model.id, transform)
        ref = self.folder / 'transform.png'
        self.quick_validation(ref)

    def test_visible(self) -> None:
        model = self.add_sphere()
        brayns.update_model(self.instance, model.id, visible=False)
        ref = self.folder / 'invisible.png'
        self.quick_validation(ref)
        brayns.update_model(self.instance, model.id, visible=True)
        ref = self.folder / 'visible.png'
        self.quick_validation(ref)
