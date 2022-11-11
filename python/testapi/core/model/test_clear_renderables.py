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


class TestClearRenderables(SimpleTestCase):

    def test_clear_renderables(self) -> None:
        models = [
            brayns.add_geometries(self.instance, [brayns.Sphere(i)])
            for i in range(1, 4)
        ]

        brayns.add_light(self.instance, brayns.AmbientLight())

        brayns.clear_renderables(self.instance)
        
        for model in models:
            with self.assertRaises(brayns.JsonRpcError):
                brayns.get_model(self.instance, model.id)

        scene = brayns.get_scene(self.instance)
        self.assertEqual(len(scene.models), 1)
        self.assertEqual(scene.models[0].type, 'light')
