# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
from tests.mock_messages import mock_view, mock_view_message


class TestSnapshot(unittest.TestCase):
    def test_save_remotely(self) -> None:
        reply = self.mock_reply(size=0)
        instance = MockInstance(reply)
        self.mock_snapshot().save_remotely(instance, self.mock_path())
        self.assertEqual(instance.method, "snapshot")
        ref = self.mock_snapshot_message() | {"file_path": self.mock_path()}
        self.assertEqual(instance.params, ref)

    def test_save_remotely_task(self) -> None:
        reply = self.mock_reply(size=0)
        instance = MockInstance(reply)
        task = self.mock_snapshot().save_remotely_task(instance, self.mock_path())
        task.wait_for_result()
        self.assertEqual(instance.method, "snapshot")
        ref = self.mock_snapshot_message() | {"file_path": self.mock_path()}
        self.assertEqual(instance.params, ref)

    def test_download(self) -> None:
        data = b"test"
        reply = self.mock_reply(size=len(data))
        instance = MockInstance(reply, data)
        test = self.mock_snapshot().download(instance, brayns.ImageFormat.JPEG)
        self.assertEqual(test, data)
        self.assertEqual(instance.method, "snapshot")
        self.assertEqual(instance.params, self.mock_snapshot_message())

    def test_download_task(self) -> None:
        data = b"test"
        reply = self.mock_reply(size=len(data))
        instance = MockInstance(reply, data)
        task = self.mock_snapshot().download_task(instance, brayns.ImageFormat.JPEG)
        test = task.wait_for_result()
        self.assertEqual(test, data)
        self.assertEqual(instance.method, "snapshot")
        self.assertEqual(instance.params, self.mock_snapshot_message())

    def mock_path(self) -> str:
        return "test.jpg"

    def mock_snapshot(self) -> brayns.Snapshot:
        metadata = brayns.ImageMetadata(
            title="A title",
            description="A description",
            where_used="A place",
            keywords=["key1, key2"],
        )
        return brayns.Snapshot(
            resolution=brayns.Resolution(1920, 1080),
            camera=brayns.Camera(view=mock_view(), near_clipping_distance=1.5),
            renderer=brayns.ProductionRenderer(),
            frame=12,
            jpeg_quality=50,
            metadata=metadata,
        )

    def mock_snapshot_message(self) -> dict[str, Any]:
        return {
            "image_settings": {
                "format": "jpg",
                "size": [1920, 1080],
                "quality": 50,
            },
            "camera_view": mock_view_message(),
            "camera": brayns.PerspectiveProjection().get_properties_with_name(),
            "camera_near_clip": 1.5,
            "image_start": [0, 1],
            "image_end": [1, 0],
            "renderer": brayns.ProductionRenderer().get_properties_with_name(),
            "simulation_frame": 12,
            "metadata": {
                "title": "A title",
                "description": "A description",
                "where_used": "A place",
                "keywords": ["key1, key2"],
            },
        }

    def mock_reply(self, size: int) -> dict[str, Any]:
        return {
            "color_buffer": {
                "offset": 0,
                "size": size,
            }
        }
