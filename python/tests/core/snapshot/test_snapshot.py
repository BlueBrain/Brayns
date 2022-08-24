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

import brayns

from tests.network.mock_instance import MockInstance


class TestSnapshot(unittest.TestCase):

    def test_save_remotely(self) -> None:
        instance = MockInstance()
        snapshot = brayns.Snapshot()
        path = 'test.jpg'
        ref = snapshot.serialize_with_path(path)
        snapshot.save_remotely(instance, path)
        self.assertEqual(instance.method, 'snapshot')
        self.assertEqual(instance.params, ref)

    def test_download(self) -> None:
        data = b'test'
        reply = {'data': base64.b64encode(data)}
        instance = MockInstance(reply)
        snapshot = brayns.Snapshot()
        test = snapshot.download(instance, brayns.ImageFormat.JPEG)
        ref = snapshot.serialize_with_format(brayns.ImageFormat.JPEG)
        self.assertEqual(test, data)
        self.assertEqual(instance.method, 'snapshot')
        self.assertEqual(instance.params, ref)

    def test_serialize_with_format(self) -> None:
        snapshot = brayns.Snapshot(
            jpeg_quality=50,
            resolution=brayns.Resolution(1920, 1080),
            frame=12,
            view=brayns.View(),
            camera=brayns.PerspectiveCamera(),
            renderer=brayns.ProductionRenderer()
        )
        ref = {
            'image_settings': {
                'format': 'jpg',
                'quality': 50,
                'size': [1920, 1080]
            },
            'simulation_frame': 12,
            'camera_view': brayns.View().serialize(),
            'camera': brayns.PerspectiveCamera().serialize_with_name(),
            'renderer': brayns.ProductionRenderer().serialize_with_name()
        }
        test = snapshot.serialize_with_format(brayns.ImageFormat.JPEG)
        self.assertEqual(test, ref)

    def test_serialize_with_path(self) -> None:
        snapshot = brayns.Snapshot()
        path = 'test.png'
        ref = {
            'file_path': path
        }
        test = snapshot.serialize_with_path(path)
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
