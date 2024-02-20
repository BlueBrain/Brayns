# Copyright (c) 2015-2024 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Authors:
#   Adrien Christian Fleury <adrien.fleury@epfl.ch>
#   Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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


class TestGBufferExporter(unittest.TestCase):
    def test_save_remotely(self) -> None:
        instance = MockInstance()
        self.mock_exporter().save_remotely(instance, self.mock_path())
        self.assertEqual(instance.method, "export-gbuffers")
        ref = self.mock_exporter_message() | {"file_path": self.mock_path()}
        self.assertEqual(instance.params, ref)

    def test_save_remotely_task(self) -> None:
        instance = MockInstance()
        task = self.mock_exporter().save_remotely_task(instance, self.mock_path())
        task.wait_for_result()
        self.assertEqual(instance.method, "export-gbuffers")
        ref = self.mock_exporter_message() | {"file_path": self.mock_path()}
        self.assertEqual(instance.params, ref)

    def test_download(self) -> None:
        data = b"test"
        instance = MockInstance(None, data)
        test = self.mock_exporter().download(instance)
        self.assertEqual(test, data)
        self.assertEqual(instance.method, "export-gbuffers")
        self.assertEqual(instance.params, self.mock_exporter_message())

    def test_download_task(self) -> None:
        data = b"test"
        instance = MockInstance(None, data)
        task = self.mock_exporter().download_task(instance)
        test = task.wait_for_result()
        self.assertEqual(test, data)
        self.assertEqual(instance.method, "export-gbuffers")
        self.assertEqual(instance.params, self.mock_exporter_message())

    def mock_path(self) -> str:
        return "test.exr"

    def mock_exporter(self) -> brayns.GBufferExporter:
        return brayns.GBufferExporter(
            channels=[brayns.GBufferChannel.COLOR],
            resolution=brayns.Resolution(1920, 1080),
            camera=brayns.Camera(view=mock_view(), near_clipping_distance=1.5),
            renderer=brayns.ProductionRenderer(),
            frame=12,
        )

    def mock_exporter_message(self) -> dict[str, Any]:
        return {
            "channels": [brayns.GBufferChannel.COLOR.value],
            "resolution": [1920, 1080],
            "camera_view": mock_view_message(),
            "camera": brayns.PerspectiveProjection().get_properties_with_name(),
            "camera_near_clip": 1.5,
            "renderer": brayns.ProductionRenderer().get_properties_with_name(),
            "simulation_frame": 12,
        }
