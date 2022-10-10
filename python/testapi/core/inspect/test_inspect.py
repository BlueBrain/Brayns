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

from typing import cast

import brayns
from testapi.simple_test_case import SimpleTestCase


class TestInspect(SimpleTestCase):

    def test_inspect(self) -> None:
        self._prepare_scene()
        test = brayns.inspect(self.instance, brayns.Vector2(0.5, 0.5))
        self.assertIsNotNone(test)
        test = cast(brayns.InspectResult, test)
        position = brayns.Vector3(x=38.38946, y=999.41394, z=56.914795)
        self.assertEqual(test.position, position)
        self.assertEqual(test.model_id, 0)
        self.assertEqual(test.metadata, {'neuron_id': 559})

    def test_inspect_empty_scene(self) -> None:
        test = brayns.inspect(self.instance, brayns.Vector2(0.5, 0.5))
        self.assertIsNone(test)

    def _prepare_scene(self) -> None:
        loader = brayns.BbpLoader(
            morphology=brayns.Morphology(
                radius_multiplier=10,
            ),
        )
        models = loader.load_models(self.instance, self.circuit)
        target = models[0].bounds
        resolution = brayns.Resolution.full_hd
        camera = brayns.look_at(target, resolution.aspect_ratio)
        brayns.update_application(self.instance, resolution)
        brayns.set_camera(self.instance, camera)
