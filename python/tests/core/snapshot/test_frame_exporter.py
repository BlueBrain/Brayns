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

import brayns

from tests.network.mock_instance import MockInstance


class TestFrameExporter(unittest.TestCase):

    def test_export_frames(self) -> None:
        instance = MockInstance()
        path = 'test'
        frames = brayns.KeyFrame.from_indices([0, 1])
        exporter = brayns.FrameExporter(frames)
        ref = exporter.serialize(path)
        exporter.export_frames(instance, path)
        self.assertEqual(instance.method, 'export-frames')
        self.assertEqual(instance.params, ref)

    def test_serialize(self) -> None:
        exporter = brayns.FrameExporter(
            frames=brayns.KeyFrame.from_indices([0, 1], brayns.View()),
            format=brayns.ImageFormat.JPEG,
            jpeg_quality=50,
            resolution=brayns.Resolution(600, 900),
            camera=brayns.PerspectiveCamera(),
            renderer=brayns.ProductionRenderer()
        )
        path = 'test'
        ref = {
            'path': path,
            'key_frames': [
                {
                    'frame_index': 0,
                    'camera_view': brayns.View().serialize()
                },
                {
                    'frame_index': 1,
                    'camera_view': brayns.View().serialize()
                }
            ],
            'image_settings': {
                'format': 'jpg',
                'quality': 50,
                'size': [600, 900]
            },
            'camera': brayns.PerspectiveCamera().serialize_with_name(),
            'renderer': brayns.ProductionRenderer().serialize_with_name()
        }
        test = exporter.serialize(path)
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
