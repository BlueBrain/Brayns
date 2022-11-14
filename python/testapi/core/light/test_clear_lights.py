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


class TestClearLights(SimpleTestCase):

    def test_clear_lights(self) -> None:
        models = [
            self.add_sphere(),
            self.add_clip_plane(),
            self.add_light(),
            self.add_light(),
        ]
        brayns.clear_lights(self.instance)
        brayns.get_model(self.instance, models[0].id)
        brayns.get_model(self.instance, models[1].id)
        with self.assertRaises(brayns.JsonRpcError):
            brayns.get_model(self.instance, models[2].id)
        with self.assertRaises(brayns.JsonRpcError):
            brayns.get_model(self.instance, models[3].id)
