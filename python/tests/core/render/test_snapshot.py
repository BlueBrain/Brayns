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

import unittest
from typing import Any

import brayns
from tests.mock_instance import MockInstance
from tests.mock_view import MockView


class TestSnapshot(unittest.TestCase):

    @classmethod
    @property
    def path(cls) -> str:
        return 'test.jpg'

    @classmethod
    @property
    def snapshot(cls) -> brayns.Snapshot:
        return brayns.Snapshot(
            resolution=brayns.Resolution(1920, 1080),
            frame=12,
            camera=brayns.Camera(view=MockView.view),
            renderer=brayns.ProductionRenderer(),
            jpeg_quality=50,
        )

    @classmethod
    @property
    def message(cls) -> dict[str, Any]:
        return {
            'image_settings': {
                'format': 'jpg',
                'size': [1920, 1080],
                'quality': 50,
            },
            'simulation_frame': 12,
            'camera_view': MockView.message,
            'camera': brayns.PerspectiveProjection().get_properties_with_name(),
            'renderer': brayns.ProductionRenderer().get_properties_with_name(),
        }

    def test_save_remotely(self) -> None:
        reply = self._reply(0)
        instance = MockInstance(reply)
        self.snapshot.save_remotely(instance, self.path)
        self.assertEqual(instance.method, 'snapshot')
        ref = self.message | {'file_path': self.path}
        self.assertEqual(instance.params, ref)

    def test_save_remotely_task(self) -> None:
        reply = self._reply(0)
        instance = MockInstance(reply)
        task = self.snapshot.save_remotely_task(instance, self.path)
        task.wait_for_result()
        self.assertEqual(instance.method, 'snapshot')
        ref = self.message | {'file_path': self.path}
        self.assertEqual(instance.params, ref)

    def test_download(self) -> None:
        data = b'test'
        reply = self._reply(len(data))
        instance = MockInstance(reply, data)
        test = self.snapshot.download(instance, brayns.ImageFormat.JPEG)
        self.assertEqual(test, data)
        self.assertEqual(instance.method, 'snapshot')
        self.assertEqual(instance.params, self.message)

    def test_download_task(self) -> None:
        data = b'test'
        reply = self._reply(len(data))
        instance = MockInstance(reply, data)
        task = self.snapshot.download_task(instance, brayns.ImageFormat.JPEG)
        test = task.wait_for_result()
        self.assertEqual(test, data)
        self.assertEqual(instance.method, 'snapshot')
        self.assertEqual(instance.params, self.message)

    def _reply(self, size: int) -> dict[str, Any]:
        return {
            'color_buffer': {
                'offset': 0,
                'size': size,
            }
        }


if __name__ == '__main__':
    unittest.main()
