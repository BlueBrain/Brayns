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


class TestOrthographicCamera(SimpleTestCase):

    def test_all(self) -> None:
        camera = brayns.OrthographicCamera()
        camera.use_as_main_camera(self.instance)
        name = brayns.Camera.get_main_camera_name(self.instance)
        self.assertEqual(name, camera.name)
        current = brayns.OrthographicCamera.is_main_camera(self.instance)
        self.assertTrue(current)
        test = brayns.OrthographicCamera.from_instance(self.instance)
        self.assertEqual(test, camera)
