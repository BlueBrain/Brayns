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
from testapi.image_validator import ImageValidator
from testapi.simple_test_case import SimpleTestCase


class TestStaticFramebuffer(SimpleTestCase):

    def test_static_framebuffer(self) -> None:
        self.add_light()
        model = self.add_sphere()

        renderer = brayns.InteractiveRenderer(2, 3, brayns.Color4.white)
        brayns.set_renderer(self.instance, renderer)

        camera = brayns.CameraController(model.bounds).camera
        brayns.set_camera(self.instance, camera)

        validator = ImageValidator()
        image = brayns.Image(False)

        frame = image.download(self.instance)
        ref = self.folder / 'static_frame.png'
        validator.validate_data(frame.data, ref)
