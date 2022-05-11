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

import base64
import unittest

from brayns.core.camera.camera_view import CameraView
from brayns.core.snapshot.image_format import ImageFormat
from brayns.core.snapshot.snapshot import Snapshot
from tests.core.camera.mock_camera import MockCamera
from tests.core.renderer.mock_renderer import MockRenderer
from tests.instance.mock_instance import MockInstance


class TestSnapshot(unittest.TestCase):

    def test_save_remotely(self) -> None:
        instance = MockInstance()
        snapshot = Snapshot()
        path = 'test.jpg'
        snapshot.save_remotely(instance, path)
        self.assertEqual(instance.method, 'snapshot')
        self.assertEqual(instance.params, {
            'path': path,
            'image_settings': {
                'format': 'jpg',
                'quality': 100
            }
        })

    def test_download(self) -> None:
        data = b'test'
        reply = {'data': base64.b64encode(data)}
        instance = MockInstance(reply)
        snapshot = Snapshot(
            jpeg_quality=50,
            resolution=(1920, 1080),
            frame=12,
            view=CameraView(),
            camera=MockCamera(),
            renderer=MockRenderer()
        )
        test = snapshot.download(instance, ImageFormat.JPEG)
        self.assertEqual(test, data)
        self.assertEqual(instance.method, 'snapshot')
        self.assertEqual(instance.params, {
            'image_settings': {
                'format': 'jpg',
                'quality': 50,
                'size': [1920, 1080]
            },
            'animation_settings': {
                'frame': 12,
            },
            'camera_view': CameraView().serialize(),
            'camera': MockCamera().serialize_with_name(),
            'renderer': MockRenderer().serialize_with_name()
        })


if __name__ == '__main__':
    unittest.main()
