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


class TestFrameExporter(unittest.TestCase):

    @classmethod
    @property
    def path(cls) -> str:
        return 'path'

    @classmethod
    @property
    def exporter(cls) -> brayns.FrameExporter:
        return brayns.FrameExporter(
            frames=[brayns.KeyFrame(i, MockView.view) for i in range(2)],
            format=brayns.ImageFormat.JPEG,
            jpeg_quality=50,
            resolution=brayns.Resolution(600, 900),
            projection=brayns.PerspectiveProjection(),
            renderer=brayns.ProductionRenderer(),
        )

    @classmethod
    @property
    def message(cls) -> dict[str, Any]:
        return {
            'path': cls.path,
            'key_frames': [
                {
                    'frame_index': 0,
                    'camera_view': MockView.message,
                },
                {
                    'frame_index': 1,
                    'camera_view': MockView.message,
                }
            ],
            'image_settings': {
                'format': 'jpg',
                'quality': 50,
                'size': [600, 900],
            },
            'camera': brayns.PerspectiveProjection().get_properties_with_name(),
            'renderer': brayns.ProductionRenderer().get_properties_with_name(),
        }

    def test_export_frames(self) -> None:
        instance = MockInstance()
        self.exporter.export_frames(instance, self.path)
        self.assertEqual(instance.method, 'export-frames')
        self.assertEqual(instance.params, self.message)

    def test_export_frames_task(self) -> None:
        instance = MockInstance()
        task = self.exporter.export_frames_task(instance, self.path)
        task.wait_for_result()
        self.assertEqual(instance.method, 'export-frames')
        self.assertEqual(instance.params, self.message)


if __name__ == '__main__':
    unittest.main()
